/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "kernel.h"

char *MODULE_NAME = "kernel";

t_log *MODULE_LOGGER;
char *MODULE_LOG_PATHNAME = "kernel.log";

t_config *MODULE_CONFIG;
char *MODULE_CONFIG_PATHNAME = "kernel.config";

//t_temporal *var_temp_quantum = NULL;

// Listas globales de estados
t_list *LIST_NEW;
t_list *LIST_READY;
t_list *LIST_EXECUTING;
t_list *LIST_BLOCKED;
t_list *LIST_EXIT;
t_list *priority_list;

pthread_mutex_t mutex_PID;
pthread_mutex_t mutex_LIST_NEW;
pthread_mutex_t mutex_LIST_READY;
pthread_mutex_t mutex_LIST_BLOCKED;
pthread_mutex_t mutex_LIST_EXECUTING;
pthread_mutex_t mutex_LIST_EXIT;

//consola interactiva
pthread_mutex_t mutex_pid_detected;
int identifier_pid = 1;
//

pthread_t THREAD_CONSOLE;
pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;
pthread_t hilo_mensajes_cpu;
pthread_t thread_interrupt;

sem_t sem_long_term_scheduler;
sem_t sem_short_term_scheduler;
sem_t sem_multiprogramming_level; // 20 procesos en sim
sem_t process_ready; // Al principio en 0

char *SCHEDULING_ALGORITHM;
int QUANTUM;
char **RESOURCES;
char **RESOURCE_INSTANCES;
int MULTIPROGRAMMING_LEVEL;
int pidContador;

int module(int argc, char *argv[]) {

	initialize_loggers();
	initialize_configs();
	initialize_sockets();
	pidContador = 0;
	
	t_PCB pcb = {
        .PID = 1234,
        .PC = 5678,
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
        .current_state = NEW_STATE,
        .arrival_READY = 123.456,
        .arrival_RUNNING = 789.012
    };

	pcb_print(&pcb);
	pcb_send(&pcb, CONNECTION_CPU_DISPATCH.fd_connection);
	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);
	
	sem_init(&sem_long_term_scheduler, 0, 0);
	sem_init(&sem_short_term_scheduler, 0, 0);
	sem_init(&sem_multiprogramming_level, 0, MULTIPROGRAMMING_LEVEL);

	LIST_NEW = list_create();
	LIST_READY = list_create();
	LIST_EXECUTING = list_create();
	LIST_BLOCKED = list_create();
	LIST_EXIT = list_create();
	priority_list = list_create();

	//UN HILO PARA CADA PROCESO
	initialize_long_term_scheduler();
	initialize_short_term_scheduler();
	initialize_cpu_command_line_interface();

	initialize_kernel_console(NULL);
	//pthread_create(&THREAD_CONSOLE, NULL, initialize_kernel_console, NULL);
	//pthread_join(THREAD_CONSOLE, NULL);

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();

    return EXIT_SUCCESS;
}

void read_module_config(t_config *MODULE_CONFIG) {
	COORDINATOR_IO = (t_Server) {.server_type = KERNEL_TYPE, .clients_type = IO_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA")};
	CONNECTION_MEMORY = (t_Connection) {.client_type = KERNEL_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
	CONNECTION_CPU_DISPATCH = (t_Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_DISPATCH_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_CPU"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_CPU_DISPATCH")};
	CONNECTION_CPU_INTERRUPT = (t_Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_INTERRUPT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_CPU"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_CPU_INTERRUPT")};
	SCHEDULING_ALGORITHM = config_get_string_value(MODULE_CONFIG, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(MODULE_CONFIG, "QUANTUM");
	RESOURCES = config_get_array_value(MODULE_CONFIG, "RECURSOS");
	RESOURCE_INSTANCES = config_get_array_value(MODULE_CONFIG, "INSTANCIAS_RECURSOS");
	MULTIPROGRAMMING_LEVEL = config_get_int_value(MODULE_CONFIG, "GRADO_MULTIPROGRAMACION");
}

void initialize_long_term_scheduler(void) {
	pthread_create(&hilo_largo_plazo, NULL, long_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador largo plazo");
	pthread_detach(hilo_largo_plazo);
}

void initialize_short_term_scheduler(void) { //ESTADO RUNNIG - MULTIPROCESAMIENTO
	pthread_create(&hilo_corto_plazo, NULL, short_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador corto plazo");
	pthread_detach(hilo_corto_plazo);
}

void initialize_cpu_command_line_interface(void) {
	pthread_create(&hilo_mensajes_cpu, NULL, receptor_mensajes_cpu, NULL);
	//log_info(MODULE_LOGGER, "Inicio mensajes cpu");
	pthread_detach(hilo_mensajes_cpu);
}

void *long_term_scheduler(void *parameter) {

	while(1) {
		sem_wait(&sem_long_term_scheduler);
		sem_wait(&sem_multiprogramming_level);
		t_PCB *pcb = list_get(LIST_NEW, 0);

		//ACA VAN OTRAS COSAS QUE HACE EL PLANIFICADOR DE LARGO PLAZO (MENSAJES CON OTROS MODULOS, ETC)

	     switch_process_state(pcb, READY_STATE);
		
	}

	return NULL;
}

void *short_term_scheduler(void *parameter) {

	t_PCB* pcb;

	while(1) {
		sem_wait(&sem_short_term_scheduler);	

		if(!strcmp(SCHEDULING_ALGORITHM, "VRR")) {
			//pcb = VRR_scheduling_algorithm();
			//pthread_create(&thread_interrupt, NULL, start_quantum_VRR, NULL);
		} else if (!strcmp(SCHEDULING_ALGORITHM, "FIFO")){
			pcb = FIFO_scheduling_algorithm();
		} else if (!strcmp(SCHEDULING_ALGORITHM, "RR")){
			pcb = RR_scheduling_algorithm();

	    	pthread_create(&thread_interrupt, NULL, start_quantum, NULL); // thread interrupt
			//pthread_detach(&thread_interrupt, NULL);
		} else {
			// log_error(MODULE_LOGGER, "El algoritmo de planificacion ingresado no existe\n");
		}

		switch_process_state(pcb, EXECUTING_STATE);

		//FALTA SERIALIZAR PCB
		//FALTA ENVIAR PAQUETE A CPU
	}

	return NULL;
}

t_PCB *FIFO_scheduling_algorithm(void) {
	pthread_mutex_lock(&mutex_LIST_READY);
		t_PCB *pcb = (t_PCB *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&mutex_LIST_READY);

	return pcb;
}



t_PCB *RR_scheduling_algorithm(void ){
	
	t_PCB *pcb;
		
       if(list_size(LIST_READY) > 0) {
            pcb = (t_PCB*)list_get(LIST_READY, 0);
            //log_info(MODULE_LOGGER, "PID: %i - Estado Anterior: READY - Estado Actual: EXECUTE", pcb->id);
        }
        else if(list_size(LIST_NEW) > 0) {
            pcb = (t_PCB*)list_get(LIST_NEW, 0);
           // log_info(MODULE_LOGGER, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->id);
           //log_info(MODULE_LOGGER, "PID: %i - Estado Anterior: READY - Estado Actual: EXECUTE", pcb->id);
        }

		return pcb;
}

/* 
PROBLEMAS.

1- EN QUE MOMENTO SE ACTUALIZA EL QUANTUM
2- EL PCB DONDE SE LO PASO?



*/
/*

pcb *kernel_get_priority_list(void) { //como sacar el pcb de las listas?
	// ponele que puede ser un list_get
}

pcb *kernel_get_normal_list(void) {
	
}
*/
/*
t_PCB *VRR_scheduling_algorithm(void){
	t_PCB *pcb;

	sem_wait(process_ready);

	pcb = kernel_get_priority_list();

	if(pcb == NULL) {

		pcb = kernel_get_normal_list();
	}

	// Mandar el PCB a CPU

//ACA CREAR  UN HILO... REVISDR
	switch(pcb->interrupt_cause) {
		case INTERRUPT_CAUSE:
			if(pcb->quantum > 0) {
				list_add(priority_list, pcb);
			} else {
				list_add(normal_list, pcb);
			}
			sem_post(process_ready);
	}

  /*if(pcb->quantum > 0 && pcb->interrupt_cause == INTERRUPTION_CAUSE){
		list_add(priority_list, pcb);
  }
  else {
	RR_scheduling_algorithm();
  }
  return pcb; 
}
/*
void update_pcb_q(t_pcb *pcb)
{
    t_config_kernel *cfg = get_config();
    if (!!strcmp(cfg->ALGORITMO_PLANIFICACION, "VRR"))
    {
        return;
    }

    temporal_stop(var_temp_quantum);
    int time_elapsed = (int)temporal_gettime(var_temp_quantum);
    int time_remaining = pcb->quantum - time_elapsed;
    temporal_destroy(var_temp_quantum);

    log_trace(get_logger(), "PCB_Q (%i) - TIME_ELAPSED (%i) = time_remaining %i", pcb->quantum, time_elapsed,
              time_remaining);

    if (time_remaining > 0)
    {
        pcb->quantum = time_remaining;
    }
        pcb->quantum = cfg->QUANTUM;
 }

DESCOMENTAR
*/ 
/*
void listen_cpu(int fd_cpu) {
    while(1) {
        e_CPU_Memory_Request memory_request = 0; //enum HeaderCode headerCode = package_receive_header(fd_cpu);
        switch (memory_request) {
            case INSTRUCTION_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de instruccion recibido.");
                seek_instruccion(fd_cpu);
                break;
                
            case FRAME_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de frame recibido.");
                respond_frame_request(fd_cpu);
                break;

            /*
            case DISCONNECTION_HEADERCODE:
                log_warning(MODULE_LOGGER, "Se desconecto CPU.");
                log_destroy(MODULE_LOGGER);
                return;
            
                
            case PAGE_SIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                //message_send(PAGE_SIZE_REQUEST, string_itoa(TAM_PAGINA),FD_CLIENT_CPU);
                break;
            
            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
            }
    }
} */ //tomar como inspiración para la función de abajo

void *receptor_mensajes_cpu(void *parameter) {
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

	return NULL;
}

void switch_process_state(t_PCB* pcb, int new_state) {
	int previous_state = pcb->current_state;
	pcb->current_state = new_state;
	char* global_previous_state;
	
	t_Package* package;
	
	bool _remover_por_pid(void* elemento) {
			return (((t_PCB*)elemento)->PID == pcb->PID);
	}

	switch (previous_state){ //! ESTADO ANTERIOR
		case NEW_STATE:
			global_previous_state="NEW";
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_remove_by_condition(LIST_NEW, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_NEW);
			break;
		case READY_STATE:
			global_previous_state="READY";
			pthread_mutex_lock(&mutex_LIST_READY);
			list_remove_by_condition(LIST_READY, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_READY);
			break;
		case EXECUTING_STATE:
		{
			global_previous_state="EXECUTING";
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_remove_by_condition(LIST_EXECUTING, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			break;
		}
		case BLOCKED_STATE:
		{
			global_previous_state="BLOCKED";
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_remove_by_condition(LIST_BLOCKED, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);		
			break;
		}
		}


	switch(new_state){ // ! ESTADO NUEVO
		case NEW_STATE:
		{
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_add(LIST_NEW, pcb);
			log_info(MODULE_LOGGER, "Se crea el proceso <%d> en NEW" ,pcb->PID);
			pthread_mutex_unlock(&mutex_LIST_NEW);
	
			sem_post(&sem_long_term_scheduler);
			break;
		}
		case READY_STATE:
		{
			pcb -> arrival_READY = current_time();

			pthread_mutex_lock(&mutex_LIST_READY);
			log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->PID, global_previous_state);
			list_add(LIST_READY, pcb);
			pthread_mutex_unlock(&mutex_LIST_READY);
			sem_post(&sem_short_term_scheduler);
			
			break;
		}
		case EXECUTING_STATE:
		{
			pcb -> arrival_RUNNING = current_time();
			
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_add(LIST_EXECUTING, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXECUTING>",pcb->PID, global_previous_state);
	
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_add(LIST_BLOCKED, pcb);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);

		
			log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>",pcb->PID, global_previous_state);

			break;
		}
		//Todos los casos de salida de un proceso.
		case EXIT_STATE:
		{
			
			 log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", pcb->PID);

			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
		
			break;
		}
		/*
		case INVALID_RESOURCE:{
			
			log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", pcb->PID);
			

			sem_post(&sem_multiprogramming_level);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		case INVALID_WRITE:{
			
			log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>", pcb->PID);
		
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
t_PCB *create_pcb() {
	//FALTA AGREGAR ATRIBUTOS AL PCB

	t_PCB *nuevoPCB = malloc(sizeof(t_PCB));

	nuevoPCB->PID = pidContador++;
    nuevoPCB->PC = 0; 
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


void send_interrupt(int socket)
{
    int dummy = 1;
    send(socket, &dummy, sizeof(dummy), 0);
}


void* start_quantum_VRR(t_PCB *pcb)
{
	
    //var_temp_quantum = temporal_create();
    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(pcb->quantum * 1000); //en milisegundos
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection); 
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %i milisegundos", QUANTUM);

	return NULL;
}


void* start_quantum()
{
    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(QUANTUM * 1000); //en milisegundos
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection); 
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %i milisegundos", QUANTUM);

	return NULL;
}

