/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "kernel.h"

char *MODULE_NAME = "kernel";
char *MODULE_LOG_PATHNAME = "kernel.log";
char *MODULE_CONFIG_PATHNAME = "kernel.config";

t_log *MODULE_LOGGER;
extern t_log *CONNECTIONS_LOGGER;
t_config *MODULE_CONFIG;

// Listas globales de estados
t_list *LIST_NEW;
t_list *LIST_READY;
t_list *LIST_EXECUTING;
t_list *LIST_BLOCKED;
t_list *LIST_EXIT;

pthread_mutex_t mutex_PID;
pthread_mutex_t mutex_LIST_NEW;
pthread_mutex_t mutex_LIST_READY;
pthread_mutex_t mutex_LIST_BLOCKED;
pthread_mutex_t mutex_LIST_EXECUTING;
pthread_mutex_t mutex_LIST_EXIT;

//consola interactiva
pthread_mutex_t mutex_pid_detected;
int identifier_pid=1;
//

pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;
pthread_t hilo_mensajes_cpu;
pthread_t thread_interrupt;

sem_t sem_long_term_scheduler;
sem_t sem_short_term_scheduler;
sem_t sem_multiprogramming_level;

Server COORDINATOR_IO;
Connection CONNECTION_MEMORY;
Connection CONNECTION_CPU_DISPATCH;
Connection CONNECTION_CPU_INTERRUPT;

char *SCHEDULING_ALGORITHM;
int QUANTUM;
char **RESOURCES;
char **RESOURCE_INSTANCES;
int MULTIPROGRAMMING_LEVEL;
int pidContador;

size_t bytes;

int module(int argc, char *argv[]) {

	initialize_loggers();
	initialize_configs();
	initialize_sockets();
	pidContador = 0;
	
	t_pcb pcb = {
        .pid = 1234,
        .pc = 5678,
        .AX = 1,
        .BX = 2,
        .CX = 3,
        .DX = 4,
        .EAX = 10,
        .EBX = 20,
        .ECX = 30,
        .EDX = 40,
        .RAX = 100,
        .RBX = 200,
        .RCX = 300,
        .RDX = 400,
        .SI = 500,
        .DI = 600,
        .quantum = 2,
        .current_state = NEW,
        //.fd_conexion = 2,
        .arrival_READY = 123.456,
        .arrival_RUNNING = 789.012
    };
	send_pcb(CONNECTION_CPU_DISPATCH.fd_connection, &pcb);
	log_info(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	initialize_interactive_console();
	
	sem_init(&sem_long_term_scheduler, 0, 0);
	sem_init(&sem_short_term_scheduler, 0, 0);
	sem_init(&sem_multiprogramming_level, 0, MULTIPROGRAMMING_LEVEL);

	LIST_NEW = list_create();
	LIST_READY = list_create();
	LIST_EXECUTING = list_create();
	LIST_BLOCKED = list_create();
	LIST_EXIT = list_create();

	//UN HILO PARA CADA PROCESO
	initialize_long_term_scheduler();
	initialize_short_term_scheduler();
	initialize_cpu_command_line_interface();

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();

    return EXIT_SUCCESS;
}

void read_module_config(t_config *MODULE_CONFIG) {
	COORDINATOR_IO = (struct Server) {.server_type = KERNEL_TYPE, .clients_type = IO_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA")};
	CONNECTION_MEMORY = (struct Connection) {.client_type = KERNEL_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
	CONNECTION_CPU_DISPATCH = (struct Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_DISPATCH_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_CPU"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_CPU_DISPATCH")};
	CONNECTION_CPU_INTERRUPT = (struct Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_INTERRUPT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_CPU"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_CPU_INTERRUPT")};
	SCHEDULING_ALGORITHM = config_get_string_value(MODULE_CONFIG, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(MODULE_CONFIG, "QUANTUM");
	RESOURCES = config_get_array_value(MODULE_CONFIG, "RECURSOS");
	RESOURCE_INSTANCES = config_get_array_value(MODULE_CONFIG, "INSTANCIAS_RECURSOS");
	MULTIPROGRAMMING_LEVEL = config_get_int_value(MODULE_CONFIG, "GRADO_MULTIPROGRAMACION");
}

void initialize_sockets(void) {
	pthread_t thread_kernel_start_server_for_io;
	pthread_t thread_kernel_connect_to_memory;
	pthread_t thread_kernel_connect_to_cpu_dispatch;
	pthread_t thread_kernel_connect_to_cpu_interrupt;

	// [Server] Kernel <- [Cliente(s)] Entrada/Salida
	pthread_create(&thread_kernel_start_server_for_io, NULL, kernel_start_server_for_io, (void*) &COORDINATOR_IO);
	// [Client] Kernel -> [Server] Memoria
	pthread_create(&thread_kernel_connect_to_memory, NULL, client_thread_connect_to_server, (void*) &CONNECTION_MEMORY);
	// [Client] Kernel -> [Server] CPU (Dispatch Port)
	pthread_create(&thread_kernel_connect_to_cpu_dispatch, NULL, client_thread_connect_to_server, (void*) &CONNECTION_CPU_DISPATCH);
	// [Client] Kernel -> [Server] CPU (Interrupt Port)
	pthread_create(&thread_kernel_connect_to_cpu_interrupt, NULL, client_thread_connect_to_server, (void*) &CONNECTION_CPU_INTERRUPT);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_kernel_connect_to_memory, NULL);
	pthread_join(thread_kernel_connect_to_cpu_dispatch, NULL);
	pthread_join(thread_kernel_connect_to_cpu_interrupt, NULL);
}

void finish_sockets(void) {
	close(COORDINATOR_IO.fd_listen);
	close(CONNECTION_MEMORY.fd_connection);
	close(CONNECTION_CPU_DISPATCH.fd_connection);
	close(CONNECTION_CPU_INTERRUPT.fd_connection);
}

void *kernel_start_server_for_io(void *server_parameter) {
	Server *server = (Server*) server_parameter;

	int *fd_new_client;
	pthread_t thread_new_client;

	server_start(server);

	while(1) {
		fd_new_client = malloc(sizeof(int));
		log_info(CONNECTIONS_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		*fd_new_client = server_accept(server->fd_listen);

		if(*fd_new_client == -1) {
			log_warning(CONNECTIONS_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			free(fd_new_client);
			continue;
		}

		log_info(CONNECTIONS_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		pthread_create(&thread_new_client, NULL, kernel_client_handler_for_io, (void*) fd_new_client);
		pthread_detach(thread_new_client);
	}

	return NULL;
}

void *kernel_client_handler_for_io(void *fd_new_client_parameter) {
	int* fd_new_client = (int*) fd_new_client_parameter;

	size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    bytes = recv(*fd_new_client, &handshake, sizeof(int32_t), MSG_WAITALL);

    switch((enum PortType) handshake) {
        case IO_TYPE:
            log_info(CONNECTIONS_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            // Lógica de manejo de cliente Entrada/Salida
            free(fd_new_client);
        break;
        default:
            log_warning(CONNECTIONS_LOGGER, "Error Handshake con [Cliente] No reconocido");
            bytes = send(*fd_new_client, &resultError, sizeof(int32_t), 0);
            free(fd_new_client);
        break;
    }

	return NULL;
}

void initialize_long_term_scheduler(void) {
	pthread_create(&hilo_largo_plazo, NULL, (void *) long_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador largo plazo");
	pthread_detach(hilo_largo_plazo);
}

void initialize_short_term_scheduler(void) { //ESTADO RUNNIG - MULTIPROCESAMIENTO
	pthread_create(&hilo_corto_plazo, NULL, (void *) short_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador corto plazo");
	pthread_detach(hilo_corto_plazo);
}

void initialize_cpu_command_line_interface(void) {
	pthread_create(&hilo_mensajes_cpu, NULL, (void *)receptor_mensajes_cpu, NULL);
	//log_info(MODULE_LOGGER, "Inicio mensajes cpu");
	pthread_detach(hilo_mensajes_cpu);
}

void long_term_scheduler(void) {

	while(1) {
		sem_wait(&sem_long_term_scheduler);
		sem_wait(&sem_multiprogramming_level);
		t_pcb *pcb = list_get(LIST_NEW, 0);

		//ACA VAN OTRAS COSAS QUE HACE EL PLANIFICADOR DE LARGO PLAZO (MENSAJES CON OTROS MODULOS, ETC)

	     switch_process_state(pcb, READY);
		
	}
}

void short_term_scheduler(void) {

	t_pcb* pcb;

	while(1) {
		sem_wait(&sem_short_term_scheduler);	

		if(!strcmp(SCHEDULING_ALGORITHM, "VRR")) {
			//pcb = algoritmo_VRR();
		} else if (!strcmp(SCHEDULING_ALGORITHM, "FIFO")){
			pcb = FIFO_scheduling_algorithm();
		} else if (!strcmp(SCHEDULING_ALGORITHM, "RR")){
			//pcb = RR_scheduling_algorithm();

	    	//pthread_create(&thread_interrupt, NULL, start_quantum, NULL); // thread interrupt
			//pthread_join(&thread_interrupt, NULL);
		} else {
			// log_error(MODULE_LOGGER, "El algoritmo de planificacion ingresado no existe\n");
		}

		switch_process_state(pcb, EXECUTING);

		//FALTA SERIALIZAR PCB
		//FALTA ENVIAR PAQUETE A CPU
	}
}

t_pcb *FIFO_scheduling_algorithm(void) {
	pthread_mutex_lock(&mutex_LIST_READY);
		t_pcb *pcb = (t_pcb *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&mutex_LIST_READY);

	return pcb;
}

/*t_pcb *RR_scheduling_algorithm(void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;
	
    pthread_t generador_de_interrupciones;
    while(1)
    {
        sem_wait(&procesos_en_ready);
        sem_wait(&planificacion_corto_plazo);
        sem_post(&planificacion_corto_plazo);
        //log_info(logger,"Hice wait del gdmp");
        sem_wait(&mutex_LISTA_READY);
        //log_info(logger,"Hice wait de la cola de new: %i",cola_new);

        t_pcb *pcb = (t_pcb *)list_remove(LISTA_EXEC, 0);
        sem_post(&mutex_LISTA_READY);
        
		return pcb;

        log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);
  
        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        //log_info(logger, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        enviar_contexto_de_ejecucion(execute->contexto, arg_h->socket_dispatch);

        execute->contexto = recibir_contexto_de_ejecucion(arg_h->socket_dispatch);
        motivo = recibir_motivo_desalojo(arg_h->socket_dispatch);
        evaluar_motivo_desalojo(logger, motivo, arg);
                
    }
}
*/

void receptor_mensajes_cpu() {
	// Package *package;

	while(1) {
		
		//FALTA RECIBIR PAQUETE DE LA CONEXION CON CPU
		//FALTA DESERIALIZAR PCB
		
		/*
		switch (package->codigo_operacion)
		{
			case EXIT:
			{
				switch_process_state(pcb, EXITED);

				break;
			}
			case SET:
			{

			}
			case MOVE_IN:
			{

			}
			case MOVE_OUT:
			{

			}
			case SUM:
			{

			}
			case SUB:
			{

			}
			case JNZ:
			{

			}
			case RESIZE:
			{
				//mando mensaje a memoria
				//Recibo mensaje de memorai
				//mando mensaje a fs
				//recibo mensaje de fs
				//mando a cpu de vuelta
			}
			case COPY_STRING:
			{

			}
			case WAIT:
			{

			}
			case SIGNAL:
			{

			}
			case IO_GEN_SLEEP:
			{

			}
			case IO_STDIN_READ:
			{

			}
			case IO_STDOUT_WRITE:
			{

			}
			case IO_FS_CREATE:
			{

			}
			case IO_FS_DELETE:
			{

			}
			case IO_FS_TRUNCATE:
			{

			}
			case IO_FS_WRITE:
			{

			}
			case IO_FS_READ:
			{

			}
			case EXIT:
			{

			}
			// ACA SIGO AGREGANDO CASES SEGUN LA INSTRUCCION
		}
		*/
	}
}

void switch_process_state(t_pcb* pcb, int new_state) {
	int previous_state = pcb->current_state;
	pcb->current_state = new_state;
	char* global_previous_state;
	
	Package* package;
	
	bool _remover_por_pid(void* elemento) {
			return (((t_pcb*)elemento)->pid == pcb->pid);
	}

	switch (previous_state){ //! ESTADO ANTERIOR
		case NEW:
			global_previous_state="NEW";
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_remove_by_condition(LIST_NEW, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_NEW);
			break;
		case READY:
			global_previous_state="READY";
			pthread_mutex_lock(&mutex_LIST_READY);
			list_remove_by_condition(LIST_READY, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_READY);
			break;
		case EXECUTING:
		{
			global_previous_state="EXECUTING";
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_remove_by_condition(LIST_EXECUTING, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			break;
		}
		case BLOCKED:
		{
			global_previous_state="BLOCKED";
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_remove_by_condition(LIST_BLOCKED, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);		
			break;
		}
		}
	switch(new_state){ // ! ESTADO NUEVO
		case NEW:
		{
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_add(LIST_NEW, pcb);
			// log_info(MODULE_LOGGER, "Se crea el proceso <%d> en NEW" ,pcb->pid);
			pthread_mutex_unlock(&mutex_LIST_NEW);
	
			sem_post(&sem_long_term_scheduler);
			break;
		}
		case READY:
		{
			pcb -> arrival_READY = current_time();

			pthread_mutex_lock(&mutex_LIST_READY);
			// log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->pid, global_previous_state);
			list_add(LIST_READY, pcb);
			pthread_mutex_unlock(&mutex_LIST_READY);
			sem_post(&sem_short_term_scheduler);
			
			break;
		}
		case EXECUTING:
		{
			pcb -> arrival_RUNNING = current_time();
			
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_add(LIST_EXECUTING, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			// log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXECUTING>",pcb->pid, global_previous_state);
	
			break;
		}
		case BLOCKED:
		{
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_add(LIST_BLOCKED, pcb);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);

			// log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>",pcb->pid, global_previous_state);

			break;
		}
		//Todos los casos de salida de un proceso.
		case EXIT:
		{
			
			// log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", pcb->pid);

			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
		
			break;
		}
		/*
		case INVALID_RESOURCE:{
			
			log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", pcb->pid);
			

			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		case INVALID_WRITE:{
			
			log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>", pcb->pid);
		
			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		*/
	}
}

//POR REVISAR
t_pcb *create_pcb() {
	//FALTA AGREGAR ATRIBUTOS AL PCB

	t_pcb *nuevoPCB = malloc(sizeof(t_pcb));

	nuevoPCB->pid = pidContador++;
    nuevoPCB->pc = 0; 
    nuevoPCB->AX = 0;
    nuevoPCB->BX = 0;
    nuevoPCB->CX = 0;
    nuevoPCB->DX = 0;
    nuevoPCB->EAX = 0;
    nuevoPCB->EBX = 0;
    nuevoPCB->ECX = 0;
    nuevoPCB->EDX = 0;
    nuevoPCB->RAX = 0;
    nuevoPCB->RBX = 0;
    nuevoPCB->RCX = 0;
    nuevoPCB->RDX = 0;
    nuevoPCB->SI = 0;
    nuevoPCB->DI = 0;
	nuevoPCB->quantum = 0;
	nuevoPCB->current_state = 0;
    //nuevoPCB->fd_conexion = 0; //CORREGIR y agregar arg socket cliente en la definición
    nuevoPCB->arrival_READY = 0;
    nuevoPCB->arrival_RUNNING = 0;

	// nuevoPCB->recurso_solicitado = string_new();

	// nuevoPCB->primera_aparicion = true;

	return nuevoPCB;
}

int current_time(void) {
	time_t now = time(NULL);
	struct tm *local = localtime(&now);
	int hours, minutes, seconds; //

	hours = local->tm_hour;
	minutes = local->tm_min;
	seconds = local->tm_sec;

	int total_seconds = hours * 60 * 60 + minutes * 60 + seconds;
	return total_seconds;
}

int asignar_PID(void) {

    pthread_mutex_lock(&mutex_pid_detected);
    unsigned int value_pid = identifier_pid;
    identifier_pid++;
    pthread_mutex_unlock(&mutex_pid_detected);

    return value_pid;
}

/*
void* start_quantum(void* arg)
{
    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(QUANTUM * 1000); //en milisegundos
    send_interrupt(thread_cpu_interrupt_start_server_for_kernel); //PREGUNTAR
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %i milisegundos", QUANTUM);
}
*/