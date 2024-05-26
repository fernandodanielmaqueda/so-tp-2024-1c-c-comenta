/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "kernel.h"

char *module_name = "kernel";
char *module_log_pathname = "kernel.log";
char *module_config_pathname = "kernel.config";

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

sem_t sem_long_term_scheduler;
sem_t sem_short_term_scheduler;
sem_t sem_multiprogramming_level;

t_log *module_logger;
extern t_log *connections_logger;
t_config *module_config;

Server COORDINATOR_IO;
Connection CONNECTION_MEMORY;
Connection CONNECTION_CPU_DISPATCH;
Connection CONNECTION_CPU_INTERRUPT;

char *SCHEDULING_ALGORITHM;
int QUANTUM;
char **RESOURCES;
char **RESOURCE_INSTANCES;
int MULTIPROGRAMMING_LEVEL;

size_t bytes;

int module(int argc, char *argv[]) {

	initialize_loggers();
	initialize_configs();
	initialize_sockets();

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
        .quantum = 50,
        .estado_actual = 1,
        .fd_conexion = 2,
        .llegada_ready = 123.456,
        .llegada_running = 789.012
    };
	enviar_pcb(fd_cpu_dispatch, &pcb);
	log_info(module_logger, "Modulo %s inicializado correctamente\n", module_name);

	//Voy a inciializar la consola activa
	initalize_console_interactive();
	
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

void read_module_config(t_config *module_config) {
	COORDINATOR_IO = (struct Server) {.server_type = KERNEL_TYPE, .clients_type = IO_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA")};
	CONNECTION_MEMORY = (struct Connection) {.client_type = KERNEL_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(module_config, "IP_MEMORIA"), .port = config_get_string_value(module_config, "PUERTO_MEMORIA")};
	CONNECTION_CPU_DISPATCH = (struct Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_DISPATCH_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_DISPATCH")};
	CONNECTION_CPU_INTERRUPT = (struct Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_INTERRUPT_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_INTERRUPT")};
	SCHEDULING_ALGORITHM = config_get_string_value(module_config, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(module_config, "QUANTUM");
	RESOURCES = config_get_array_value(module_config, "RECURSOS");
	RESOURCE_INSTANCES = config_get_array_value(module_config, "INSTANCIAS_RECURSOS");
	MULTIPROGRAMMING_LEVEL = config_get_int_value(module_config, "GRADO_MULTIPROGRAMACION");
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
		log_info(connections_logger, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		*fd_new_client = server_accept(server->fd_listen);

		if(*fd_new_client == -1) {
			log_warning(connections_logger, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			free(fd_new_client);
			continue;
		}

		log_info(connections_logger, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
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
            log_info(connections_logger, "OK Handshake con [Cliente(s)] %s", "Entrada/Salida");
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            // Lógica de manejo de cliente Entrada/Salida
            free(fd_new_client);
        break;
        default:
            log_warning(connections_logger, "Error Handshake con [Cliente(s)] %s", "No reconocido");
            bytes = send(*fd_new_client, &resultError, sizeof(int32_t), 0);
            free(fd_new_client);
        break;
    }

	return NULL;
}

void initialize_long_term_scheduler(void) {
	pthread_create(&hilo_largo_plazo, NULL, (void *) long_term_scheduler, NULL);
	//log_info(module_logger, "Inicio planificador largo plazo");
	pthread_detach(hilo_largo_plazo);
}

void initialize_short_term_scheduler(void) { //ESTADO RUNNIG - MULTIPROCESAMIENTO
	pthread_create(&hilo_corto_plazo, NULL, (void *) short_term_scheduler, NULL);
	//log_info(module_logger, "Inicio planificador corto plazo");
	pthread_detach(hilo_corto_plazo);
}

void initialize_cpu_command_line_interface(void) {
	pthread_create(&hilo_mensajes_cpu, NULL, (void *)receptor_mensajes_cpu, NULL);
	//log_info(module_logger, "Inicio mensajes cpu");
	pthread_detach(hilo_mensajes_cpu);
}

void long_term_scheduler(void) {
	while(1) {
		sem_wait(&sem_long_term_scheduler);
		sem_wait(&sem_multiprogramming_level);
		
		//ACA VAN OTRAS COSAS QUE HACE EL PLANIFICADOR DE LARGO PLAZO (MENSAJES CON OTROS MODULOS, ETC)

		// switch_process_state(pcb, READY);
		
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
			//pcb = algoritmo_RR();
		} else {
			// log_error(module_logger, "El algoritmo de planificacion ingresado no existe\n");
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

/*
1. Ejecutar Script de Operaciones
2. Iniciar proceso
3. Finalizar proceso
4. Iniciar planificacion
5. Detener planificación
6. Listar procesos por estado
*/

void receptor_mensajes_cpu(void) {
	// t_paquete *paquete;

	while (1)
	{
		
		//FALTA RECIBIR PAQUETE DE LA CONEXION CON CPU
		//FALTA DESERIALIZAR PCB
		
		/*
		switch (paquete->codigo_operacion)
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

void switch_process_state(t_pcb* pcb, int estado_nuevo) {
	int estado_anterior = pcb->estado_actual;
	pcb->estado_actual = estado_nuevo;
	char* global_estado_anterior;
	
	// t_paquete* paquete;
	
	bool _remover_por_pid(void* elemento) {
			return (((t_pcb*)elemento)->pid == pcb->pid);
	}

	switch (estado_anterior){ //! ESTADO ANTERIOR
		case NEW:
		{
			global_estado_anterior="NEW";
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_remove_by_condition(LIST_NEW, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_NEW);
			break;
		}
		case READY:{
			global_estado_anterior="READY";
			pthread_mutex_lock(&mutex_LIST_READY);
			list_remove_by_condition(LIST_READY, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_READY);
			break;
		}
		case EXECUTING:
		{
			global_estado_anterior="EXECUTING";
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_remove_by_condition(LIST_EXECUTING, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			break;
		}
		case BLOCKED:
		{
			global_estado_anterior="BLOCKED";
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_remove_by_condition(LIST_BLOCKED, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);		
			break;
		}
		}
	switch(estado_nuevo){ // ! ESTADO NUEVO
		case NEW:
		{
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_add(LIST_NEW, pcb);
			// log_info(module_logger, "Se crea el proceso <%d> en NEW" ,pcb->pid);
			pthread_mutex_unlock(&mutex_LIST_NEW);
	
			sem_post(&sem_long_term_scheduler);
			break;
		}
		case READY:
		{
			pcb -> llegada_ready = current_time();

			pthread_mutex_lock(&mutex_LIST_READY);
			// log_info(module_logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->pid, global_estado_anterior);
			list_add(LIST_READY, pcb);
			pthread_mutex_unlock(&mutex_LIST_READY);
			sem_post(&sem_short_term_scheduler);
			
			break;
		}
		case EXECUTING:
		{
			pcb -> llegada_running = current_time();
			
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_add(LIST_EXECUTING, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			// log_info(module_logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXECUTING>",pcb->pid, global_estado_anterior);
	
			break;
		}
		case BLOCKED:
		{
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_add(LIST_BLOCKED, pcb);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);

			// log_info(module_logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>",pcb->pid, global_estado_anterior);

			break;
		}
		//Todos los casos de salida de un proceso.
		case EXITED:{
			
			// log_info(module_logger, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", pcb->pid);

			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
		
			break;
		}
		/*
		case INVALID_RESOURCE:{
			
			log_info(module_logger, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", pcb->pid);
			

			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		case INVALID_WRITE:{
			
			log_info(module_logger, "Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>", pcb->pid);
		
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
t_pcb *create_pcb(char *instrucciones) {
	//FALTA AGREGAR ATRIBUTOS AL PCB

	t_pcb *nuevoPCB = malloc(sizeof(t_pcb));

	

	nuevoPCB->pc = 0;
	// nuevoPCB->recurso_solicitado = string_new();

	// nuevoPCB->primera_aparicion = true;


	pthread_mutex_lock(&mutex_PID);
	// nuevoPCB->pid = PID;
	// PID++;
	pthread_mutex_unlock(&mutex_PID);

	nuevoPCB->AX = string_new();
	nuevoPCB->BX = string_new();
	nuevoPCB->CX = string_new();
	nuevoPCB->DX = string_new();
	
	nuevoPCB->EAX = string_new();
	nuevoPCB->EBX = string_new();
	nuevoPCB->ECX = string_new();
	nuevoPCB->EDX = string_new();

	nuevoPCB->RAX = string_new();
	nuevoPCB->RBX = string_new();
	nuevoPCB->RCX = string_new();
	nuevoPCB->RDX = string_new();

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