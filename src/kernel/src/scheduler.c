#include "scheduler.h"

t_Scheduling_Algorithm SCHEDULING_ALGORITHMS[] = {
	{ .name = "FIFO", .type = FIFO_SCHEDULING_ALGORITHM, .function_fetcher = FIFO_scheduling_algorithm , .function_reprogrammer = FIFO_scheduling_reprogrammer},
	{ .name = "RR",.type = RR_SCHEDULING_ALGORITHM,.function_fetcher = RR_scheduling_algorithm , .function_reprogrammer = RR_scheduling_reprogrammer },
	{ .name = "VRR",.type =VRR_SCHEDULING_ALGORITHM , .function_fetcher = VRR_scheduling_algorithm , .function_reprogrammer = VRR_scheduling_reprogrammer },
	{ .name = NULL }
};

t_Scheduling_Algorithm *SCHEDULING_ALGORITHM;

int QUANTUM_INTERRUPT;
pthread_mutex_t MUTEX_QUANTUM_INTERRUPT;

t_list *START_PROCESS;
pthread_mutex_t MUTEX_LIST_START_PROCESS;

t_temporal *TEMPORAL_DISPATCHED;

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
pthread_mutex_t MUTEX_LIST_READY_PRIORITARY;
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
pthread_t THREAD_QUANTUM_INTERRUPT;

sem_t SEM_LONG_TERM_SCHEDULER;
sem_t SEM_SHORT_TERM_SCHEDULER;
sem_t SEM_MULTIPROGRAMMING_LEVEL; // 20 procesos en sim
sem_t SEM_PROCESS_READY; // Al principio en 0



uint64_t QUANTUM;
int MULTIPROGRAMMING_LEVEL;

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
		
		package = package_create_with_header(PROCESS_NEW);
		text_serialize(package->payload, abspath);
		payload_enqueue(package->payload, &(pcb->PID), sizeof(pcb->PID));
		package_send(package, CONNECTION_MEMORY.fd_connection);
		package_destroy(package);

		free(abspath);

	    switch_process_state(pcb, READY_STATE);

		sem_post(&SEM_SHORT_TERM_SCHEDULER);
	}

	return NULL;
}

void *short_term_scheduler(void *parameter) {

	t_PCB* pcb;
	e_Eviction_Reason *eviction_reason;
	t_Payload *syscall_instruction;
	t_Package *package;
	int exit_status;
	uint64_t cpu_burst;

	while(1) {
		sem_wait(&SEM_SHORT_TERM_SCHEDULER);

		pcb = SCHEDULING_ALGORITHM->function_fetcher();

		switch_process_state(pcb, EXECUTING_STATE);

		int PCB_EXECUTE = 1;
		while(PCB_EXECUTE) {

			send_pcb(pcb, CONNECTION_CPU_DISPATCH.fd_connection);

			switch(SCHEDULING_ALGORITHM->type) {
				case RR_SCHEDULING_ALGORITHM:
				case VRR_SCHEDULING_ALGORITHM:
					QUANTUM_INTERRUPT = 0;
					TEMPORAL_DISPATCHED = temporal_create();

					pthread_create(&THREAD_QUANTUM_INTERRUPT, NULL, start_quantum, (void *) &(pcb->quantum));
					pthread_detach(THREAD_QUANTUM_INTERRUPT);
					break;
				case FIFO_SCHEDULING_ALGORITHM:
					break;
			}
			// pcb_free(pcb);
		
			package = package_receive(CONNECTION_CPU_DISPATCH.fd_connection);
			switch(package->header) {
			case SUBHEADER_HEADER:

			switch(SCHEDULING_ALGORITHM->type) {
				case RR_SCHEDULING_ALGORITHM:
				case VRR_SCHEDULING_ALGORITHM:
					temporal_stop(TEMPORAL_DISPATCHED);
					cpu_burst = temporal_gettime(TEMPORAL_DISPATCHED);
					temporal_destroy(TEMPORAL_DISPATCHED);

					pthread_mutex_lock(&MUTEX_QUANTUM_INTERRUPT);
					if(!QUANTUM_INTERRUPT)
						pthread_cancel(THREAD_QUANTUM_INTERRUPT);
					pthread_mutex_unlock(&MUTEX_QUANTUM_INTERRUPT);
					pthread_join(THREAD_QUANTUM_INTERRUPT, NULL);
					break;
				case FIFO_SCHEDULING_ALGORITHM:
					break;
			}

				pcb = pcb_deserialize(package->payload);
				eviction_reason = eviction_reason_deserialize(package->payload);
				syscall_instruction = subpayload_deserialize(package->payload);
				break;
			default:
				log_error(SERIALIZE_LOGGER, "HeaderCode pcb %d desconocido", package->header);
				exit(EXIT_FAILURE);
				break;
			}
			package_destroy(package);

			switch(SCHEDULING_ALGORITHM->type) {
				case RR_SCHEDULING_ALGORITHM:
					pcb->quantum = QUANTUM;
					break;
				case VRR_SCHEDULING_ALGORITHM:
					pcb->quantum -= cpu_burst;
					if(pcb->quantum <= 0)
						pcb->quantum = QUANTUM;
					break;
				case FIFO_SCHEDULING_ALGORITHM:
					break;
			}

			switch(*eviction_reason) {
				case ERROR_EVICTION_REASON:
				case EXIT_EVICTION_REASON:
					switch_process_state(pcb, EXIT_STATE);
					PCB_EXECUTE = 0;
					break;

				case INTERRUPTION_EVICTION_REASON:
					// FALTARÍA DISTINGUIR SI LA INTERRUPCIÓN FUE POR FIN DE QUANTUM O POR KILL
					switch_process_state(pcb, READY_STATE);
					PCB_EXECUTE = 0;
					break;
					
				case SYSCALL_EVICTION_REASON:
					SYSCALL_PCB = pcb;
					exit_status = syscall_execute(syscall_instruction);

					if(exit_status) {
						switch_process_state(pcb, EXIT_STATE);
						PCB_EXECUTE = 0;
						break;
					}

					if(BLOCKING_SYSCALL) {
						switch_process_state(SYSCALL_PCB, BLOCKED_STATE);
						PCB_EXECUTE = 0;
						break;
					}

					// En caso de que sea una syscall no bloqueante
					break;
			}

			// pcb_free(pcb)
			// eviction_reason_free(eviction_reason);
			// instruction_free(instruction);
		}
	}

	return NULL;
}

t_PCB *FIFO_scheduling_algorithm(void) {
	pthread_mutex_lock(&mutex_LIST_READY);
		t_PCB *pcb = (t_PCB *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&mutex_LIST_READY);

	return pcb;
}

t_PCB *RR_scheduling_algorithm(void) {
	
	t_PCB *pcb;

	pthread_mutex_lock(&mutex_LIST_READY);
		pcb = (t_PCB *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&mutex_LIST_READY);

	return pcb;
}

t_PCB *VRR_scheduling_algorithm(void) {
	t_PCB *pcb;
	
	if(list_size(LIST_READY_PRIORITARY)) {
		pthread_mutex_lock(&MUTEX_LIST_READY_PRIORITARY);
			pcb = (t_PCB *) list_remove(LIST_READY_PRIORITARY, 0);
		pthread_mutex_unlock(&MUTEX_LIST_READY_PRIORITARY);
	} else {
		pthread_mutex_lock(&mutex_LIST_READY);
			pcb = (t_PCB *) list_remove(LIST_READY, 0);
		pthread_mutex_unlock(&mutex_LIST_READY);
	}

	return pcb;

	// VA EN LISTEN CPU VRR
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

t_PCB *FIFO_scheduling_reprogrammer(t_PCB *pcb) {
	return NULL;
}



t_PCB *RR_scheduling_reprogrammer(t_PCB *pcb) {
	return NULL;
}

t_PCB *VRR_scheduling_reprogrammer(t_PCB *pcb){
	return NULL;
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
		case NEW_STATE: {
			pthread_mutex_lock(&mutex_LIST_NEW);
			list_add(LIST_NEW, pcb);
			log_info(MINIMAL_LOGGER, "Se crea el proceso <%d> en NEW" ,pcb->PID);
			pthread_mutex_unlock(&mutex_LIST_NEW);
	
			sem_post(&SEM_LONG_TERM_SCHEDULER);
			break;
		}
		case READY_STATE:
		{
			pcb -> arrival_READY = current_time();

			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->PID, global_previous_state);
			switch(SCHEDULING_ALGORITHM->type) {
				case VRR_SCHEDULING_ALGORITHM:

					if(pcb->quantum < QUANTUM) {
						pthread_mutex_lock(&MUTEX_LIST_READY_PRIORITARY);
							list_add(LIST_READY_PRIORITARY, pcb);
						pthread_mutex_unlock(&MUTEX_LIST_READY_PRIORITARY);
						sem_post(&SEM_SHORT_TERM_SCHEDULER);
					} else {
						pthread_mutex_lock(&mutex_LIST_READY);
							list_add(LIST_READY, pcb);
						pthread_mutex_unlock(&mutex_LIST_READY);
						sem_post(&SEM_SHORT_TERM_SCHEDULER);
					}
					break;
				case RR_SCHEDULING_ALGORITHM:
				case FIFO_SCHEDULING_ALGORITHM:
					pthread_mutex_lock(&mutex_LIST_READY);
						list_add(LIST_READY, pcb);
					pthread_mutex_unlock(&mutex_LIST_READY);
					sem_post(&SEM_SHORT_TERM_SCHEDULER);
					break;
			}			
			break;
		}
		case EXECUTING_STATE: {
			pcb -> arrival_RUNNING = current_time();
			
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
			list_add(LIST_EXECUTING, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXECUTING>",pcb->PID, global_previous_state);
	
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
			list_add(LIST_BLOCKED, pcb);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);

		
			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>",pcb->PID, global_previous_state);

			break;
		}
		//Todos los casos de salida de un proceso.
		case EXIT_STATE:
		{
			 log_info(MINIMAL_LOGGER, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", pcb->PID);

			sem_post(&SEM_MULTIPROGRAMMING_LEVEL);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
		
			break;
		}
		/*
		case INVALID_RESOURCE:{
			
			log_info(MINIMAL_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", pcb->PID);
			

			sem_post(&SEM_MULTIPROGRAMMING_LEVEL);

			pthread_mutex_lock(&mutex_LIST_EXIT);
			list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			
			break;
		}
		case INVALID_WRITE:{
			
			log_info(MINIMAL_LOGGER, "Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>", pcb->PID);
		
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
	pcb->quantum = QUANTUM;
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

void *thread_send_cpu_interrupt(void *arguments) {
	/*t_PCB *pcb = (t_PCB *) pcb_parameter;
	sem_wait(&SEM_CPU_INTERRUPT);

    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(pcb- * 1000); //en milisegundos
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection); 
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %i milisegundos", QUANTUM);
	*/

	return NULL;
}

void *start_quantum(void *pcb_parameter) {

	t_PCB *pcb = (t_PCB *) pcb_parameter;

    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(pcb->quantum * 1000); // en milisegundos
	// ENVIAR LA INTERRUPCIÓN SÓLO SI HAY MÁS PROCESOS EN READY
	// sem_wait()
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection);
    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %li milisegundos", pcb->quantum);

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
