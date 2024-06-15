#include "scheduler.h"

t_Scheduling_Algorithm SCHEDULING_ALGORITHMS[] = {
	{ .name = "FIFO", .function = FIFO_scheduling_algorithm },
	{ .name = "RR", .function = RR_scheduling_algorithm },
	{ .name = "VRR", .function = VRR_scheduling_algorithm },
	{ .name = NULL, .function = NULL }
};

t_Scheduling_Algorithm *SCHEDULING_ALGORITHM;

t_list *START_PROCESS;
pthread_mutex_t MUTEX_LIST_START_PROCESS;

// Listas globales de estados
t_list *LIST_NEW;
t_list *LIST_READY;
t_list *LIST_READY_PRIORITARY;
t_list *LIST_EXECUTING;
t_list *LIST_BLOCKED;
t_list *LIST_EXIT;

pthread_mutex_t mutex_PID;
pthread_mutex_t mutex_LIST_NEW;
pthread_mutex_t mutex_LIST_READY;
pthread_mutex_t mutex_LIST_BLOCKED;
pthread_mutex_t mutex_LIST_EXECUTING;
pthread_mutex_t mutex_LIST_EXIT;

sem_t sem_detener_execute;
sem_t sem_detener_new_ready;
sem_t sem_detener_block_ready;
sem_t sem_detener_block;
sem_t sem_detener_planificacion;

pthread_t THREAD_LONG_TERM_SCHEDULER;
pthread_t THREAD_SHORT_TERM_SCHEDULER;
pthread_t hilo_mensajes_cpu;
pthread_t THREAD_INTERRUPT;

sem_t SEM_LONG_TERM_SCHEDULER;
sem_t SEM_SHORT_TERM_SCHEDULER;
sem_t SEM_MULTIPROGRAMMING_LEVEL; // 20 procesos en sim
sem_t SEM_PROCESS_READY; // Al principio en 0

int QUANTUM;
int MULTIPROGRAMMING_LEVEL;

t_temporal *VAR_TEMP_QUANTUM = NULL;

//consola interactiva
pthread_mutex_t MUTEX_PID_DETECTED;
int IDENTIFIER_PID = 1;

int PID_COUNTER;

t_Scheduling_Algorithm *find_scheduling_algorithm(char *name) {
	for (register int i = 0; SCHEDULING_ALGORITHMS[i].name != NULL; i++) {
		if (!strcmp(SCHEDULING_ALGORITHMS[i].name, name)) {
			return (&SCHEDULING_ALGORITHMS[i]);
		}
	}
	return NULL;
}

void initialize_long_term_scheduler(void) {
	pthread_create(&THREAD_LONG_TERM_SCHEDULER, NULL, long_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador largo plazo");
	pthread_detach(THREAD_LONG_TERM_SCHEDULER);
}

void initialize_short_term_scheduler(void) { //ESTADO RUNNIG - MULTIPROCESAMIENTO
	pthread_create(&THREAD_SHORT_TERM_SCHEDULER, NULL, short_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador corto plazo");
	pthread_detach(THREAD_SHORT_TERM_SCHEDULER);
}

void *long_term_scheduler(void *parameter) {

	char *abspath;
	t_PCB *pcb;

    t_Package* package;
	while(1) {
		sem_wait(&SEM_LONG_TERM_SCHEDULER);
		sem_wait(&SEM_MULTIPROGRAMMING_LEVEL);

		pthread_mutex_lock(&MUTEX_LIST_START_PROCESS);
			abspath = (char *) list_remove(START_PROCESS, 0);
		pthread_mutex_unlock(&MUTEX_LIST_START_PROCESS);

		pcb = pcb_create();

		pthread_mutex_lock(&mutex_LIST_NEW);
			list_add(LIST_NEW, pcb);
		pthread_mutex_unlock(&mutex_LIST_NEW);
		
		package = package_create_with_header(SUBHEADER_HEADER);
		pcb_serialize(package->payload, pcb);
		payload_enqueue_string(package->payload, abspath);
		package_send(package, CONNECTION_MEMORY.fd_connection);
		package_destroy(package);

		free(abspath);

		// Recibo paquete de memoria
		//package = package_receive(CONNECTION_MEMORY.fd_connection);			
	
	     switch_process_state(pcb, READY_STATE);
	}

	return NULL;
}

void *short_term_scheduler(void *parameter) {

	t_PCB* pcb;

	while(1) {
		sem_wait(&SEM_SHORT_TERM_SCHEDULER);

		pcb = SCHEDULING_ALGORITHM->function();

		switch_process_state(pcb, EXECUTING_STATE);

		pcb_send(pcb, CONNECTION_CPU_DISPATCH.fd_connection);
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



t_PCB *kernel_get_priority_list(void) { //como sacar el pcb de las listas?
	// ponele que puede ser un list_get
}

t_PCB *kernel_get_normal_list(void) {
	
}

t_PCB *VRR_scheduling_algorithm(void){
	t_PCB *pcb;

	sem_wait(&SEM_PROCESS_READY);

	pcb = kernel_get_priority_list();

	if(pcb == NULL) {

		pcb = kernel_get_normal_list();
	}

	// Mandar el PCB a CPU

//ACA CREAR  UN HILO... REVISDR
/*
	switch(pcb->interrupt_cause) {
		case INTERRUPT_CAUSE:
			if(pcb->quantum > 0) {
				list_add(LIST_READY_PRIORITARY, pcb);
			} else {
				list_add(LIST_READY, pcb);
			}
			sem_post(&SEM_PROCESS_READY);
	}

  if(pcb->quantum > 0 && pcb->interrupt_cause == INTERRUPTION_CAUSE){
		list_add(LIST_READY_PRIORITARY, pcb);
  }
  else {
	RR_scheduling_algorithm();
  }
  return pcb; */
}

/*
void update_pcb_q(t_pcb *pcb)
{
    t_config_kernel *cfg = get_config();
    if (!!strcmp(cfg->ALGORITMO_PLANIFICACION, "VRR"))
    {
        return;
    }

    temporal_stop(VAR_TEMP_QUANTUM);
    int time_elapsed = (int)temporal_gettime(VAR_TEMP_QUANTUM);
    int time_remaining = pcb->quantum - time_elapsed;
    temporal_destroy(VAR_TEMP_QUANTUM);

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


void switch_process_state(t_PCB* pcb, int new_state) {
	int previous_state = pcb->current_state;
	pcb->current_state = new_state;
	char* global_previous_state;
	
	//t_Package* package;
	
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
	
			sem_post(&SEM_LONG_TERM_SCHEDULER);
			break;
		}
		case READY_STATE:
		{
			pcb -> arrival_READY = current_time();

			pthread_mutex_lock(&mutex_LIST_READY);
			log_info(MODULE_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->PID, global_previous_state);
			list_add(LIST_READY, pcb);
			pthread_mutex_unlock(&mutex_LIST_READY);
			sem_post(&SEM_SHORT_TERM_SCHEDULER);
			
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

			sem_post(&SEM_MULTIPROGRAMMING_LEVEL);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
		
			break;
		}
		/*
		case INVALID_RESOURCE:{
			
			log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", pcb->PID);
			

			sem_post(&SEM_MULTIPROGRAMMING_LEVEL);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		case INVALID_WRITE:{
			
			log_info(MODULE_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>", pcb->PID);
		
			sem_post(&SEM_MULTIPROGRAMMING_LEVEL);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		*/
	}



}

//POR REVISAR
t_PCB *pcb_create() {
	//FALTA AGREGAR ATRIBUTOS AL PCB

	t_PCB *pcb = malloc(sizeof(t_PCB));

	pcb->PID = PID_COUNTER++;
    pcb->PC = 0; 
    pcb->AX = 0;
    pcb->BX = 0;
    pcb->CX = 0;
    pcb->DX = 0;
    pcb->EAX = 0;
    pcb->EBX = 0;
    pcb->ECX = 0;
    pcb->EDX = 0;
    pcb->RAX = 0;
    pcb->RBX = 0;
    pcb->RCX = 0;
    pcb->RDX = 0;
    pcb->SI = 0;
    pcb->DI = 0;
	pcb->quantum = 0;
	pcb->current_state = 0;
    pcb->arrival_READY = 0;
    pcb->arrival_RUNNING = 0;

	// pcb->recurso_solicitado = string_new();

	// pcb->primera_aparicion = true;

	return pcb;
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

    pthread_mutex_lock(&MUTEX_PID_DETECTED);
    unsigned int value_pid = IDENTIFIER_PID;
    IDENTIFIER_PID++;
    pthread_mutex_unlock(&MUTEX_PID_DETECTED);

    return value_pid;
}


void send_interrupt(int socket)
{
    int dummy = 1;
    send(socket, &dummy, sizeof(dummy), 0);
}


void* start_quantum_VRR(t_PCB *pcb)
{
    VAR_TEMP_QUANTUM = temporal_create();
    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(pcb->quantum * 1000); //en milisegundos
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection); 
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %i milisegundos", QUANTUM);

	return NULL;
}


void* start_quantum(void)
{
    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(QUANTUM * 1000); //en milisegundos
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection); 
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %i milisegundos", QUANTUM);

	return NULL;
}

void stop_planificacion(void) {
	sem_wait(&sem_detener_planificacion);
	sem_wait(&sem_detener_new_ready);
	sem_wait(&sem_detener_block_ready);
	sem_wait(&sem_detener_block);
	sem_wait(&sem_detener_execute);
	sem_post(&sem_detener_planificacion);
}

void init_planificacion(void) {
	sem_wait(&sem_detener_planificacion);
	sem_post(&sem_detener_new_ready);
	sem_post(&sem_detener_block_ready);
	sem_post(&sem_detener_block);
	sem_post(&sem_detener_execute);
	sem_post(&sem_detener_planificacion);
}

void free_strv(char** array) {
    if (array == NULL)
		return;
	
	for (int i = 0; array[i] != NULL; i++)
		free(array[i]);

	free(array);
}