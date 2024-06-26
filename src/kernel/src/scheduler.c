#include "scheduler.h"

t_Scheduling_Algorithm SCHEDULING_ALGORITHMS[] = {
	{ .name = "FIFO", .type = FIFO_SCHEDULING_ALGORITHM, .function_fetcher = FIFO_scheduling_algorithm},
	{ .name = "RR",.type = RR_SCHEDULING_ALGORITHM,.function_fetcher = RR_scheduling_algorithm },
	{ .name = "VRR",.type = VRR_SCHEDULING_ALGORITHM , .function_fetcher = VRR_scheduling_algorithm },
	{ .name = NULL }
};

t_Scheduling_Algorithm *SCHEDULING_ALGORITHM;

const char *EXIT_REASONS[] = {
	[SUCCESS_EXIT_REASON] = "SUCCESS",
	[INVALID_RESOURCE_EXIT_REASON] = "INVALID_RESOURCE",
	[INVALID_INTERFACE_EXIT_REASON] = "INVALID_INTERFACE",
	[OUT_OF_MEMORY_EXIT_REASON] = "OUT_OF_MEMORY",
	[INTERRUPTED_BY_USER_EXIT_REASON] = "INTERRUPTED_BY_USER"
};  

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

pthread_t THREAD_LONG_TERM_SCHEDULER_NEW;
pthread_t THREAD_LONG_TERM_SCHEDULER_EXIT;
pthread_t THREAD_SHORT_TERM_SCHEDULER;
pthread_t hilo_mensajes_cpu;
pthread_t THREAD_QUANTUM_INTERRUPT;

sem_t SEM_LONG_TERM_SCHEDULER_NEW;
sem_t SEM_LONG_TERM_SCHEDULER_EXIT;
sem_t SEM_SHORT_TERM_SCHEDULER;
sem_t SEM_MULTIPROGRAMMING_LEVEL; // 20 procesos en sim
sem_t SEM_PROCESS_READY; // Al principio en 0



t_Quantum QUANTUM;
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
	pthread_create(&THREAD_LONG_TERM_SCHEDULER_NEW, NULL, long_term_scheduler_new, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador largo plazo");
	pthread_detach(THREAD_LONG_TERM_SCHEDULER_NEW);
	pthread_create(&THREAD_LONG_TERM_SCHEDULER_EXIT, NULL, long_term_scheduler_exit, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador largo plazo");
	pthread_detach(THREAD_LONG_TERM_SCHEDULER_EXIT);
}

void initialize_short_term_scheduler(void) { //ESTADO RUNNIG - MULTIPROCESAMIENTO
	pthread_create(&THREAD_SHORT_TERM_SCHEDULER, NULL, short_term_scheduler, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador corto plazo");
	pthread_detach(THREAD_SHORT_TERM_SCHEDULER);
}

void *long_term_scheduler_new(void *parameter) {

	char *path;
	t_PCB *pcb;

    t_Package* package;
	while(1) {
		sem_wait(&SEM_LONG_TERM_SCHEDULER_NEW);
		sem_wait(&SEM_MULTIPROGRAMMING_LEVEL);

		pthread_mutex_lock(&MUTEX_LIST_START_PROCESS);
			path = (char *) list_remove(START_PROCESS, 0);
		pthread_mutex_unlock(&MUTEX_LIST_START_PROCESS);

		pcb = pcb_create();

		pthread_mutex_lock(&mutex_LIST_NEW);
			list_add(LIST_NEW, pcb);
		pthread_mutex_unlock(&mutex_LIST_NEW);
		
		package = package_create_with_header(PROCESS_CREATE_HEADER);
		text_serialize(package->payload, path);
		payload_enqueue(package->payload, &(pcb->PID), sizeof(pcb->PID));
		package_send(package, CONNECTION_MEMORY.fd_connection);
		package_destroy(package);

		t_Return_Value return_value;
		receive_return_value_with_header(PROCESS_CREATE_HEADER, &return_value, CONNECTION_MEMORY.fd_connection);
		if(return_value) {
			log_warning(MODULE_LOGGER, "[Memoria]: No se pudo INICIAR_PROCESO %s", path);
		} else {
			switch_process_state(pcb, READY_STATE);
		}

		//ANALIZAMOS ESTADO EXIT
		//RECIBIR EL MOTIVO DE INTERRUPCION QUE YA ME ENVIO CPU
		/* 
		pthread_mutex_lock(&mutex_LIST_EXIT);
			t_PCB pcb = list_get(LIST_EXIT, 0);
		pthread_mutex_unlock(&mutex_LIST_EXIT);
		*/

		free(path);
	}

	return NULL;
}

void *long_term_scheduler_exit(void *NULL_parameter) {
	t_PCB *pcb;
    t_Package* package;

	while(1) {
		sem_wait(&SEM_LONG_TERM_SCHEDULER_EXIT);

		pthread_mutex_lock(&mutex_LIST_EXIT);
			pcb = (t_PCB *) list_remove(LIST_EXIT, 0);
		pthread_mutex_unlock(&mutex_LIST_EXIT);

		log_info(MINIMAL_LOGGER, "Finaliza el proceso <%d> - Motivo: <%s>", pcb->PID, EXIT_REASONS[pcb->exit_reason]);

		// FALTA LA LÓGICA DE LIBERACIÓN DE PROCESO

		/*
		HACE FALTA ESTE SWITCH O NO ?
		switch(pcb->exit_reason) {
			case OUT_OF_MEMORY:
			{
				t_Package *package = package_create_with_header(PROCESS_DESTROY_HEADER);
				payload_enqueue(package->payload, &(pcb->PID), sizeof(pcb->PID));
				package_send(package, CONNECTION_MEMORY.fd_connection);
				package_destroy(package);

				pthread_mutex_lock(&mutex_LIST_EXIT);
					list_add(LIST_EXIT, pcb);
				pthread_mutex_unlock(&mutex_LIST_EXIT);
				
				sem_post(&SEM_MULTIPROGRAMMING_LEVEL);	
					
				break;
			}
		}
		*/

		sem_post(&SEM_MULTIPROGRAMMING_LEVEL);
		
	}

	return NULL;
}

void *short_term_scheduler(void *parameter) {

	t_PCB *pcb;
	e_Eviction_Reason eviction_reason;
	t_Payload *syscall_instruction = payload_create();
	t_Package *package;
	int exit_status;
	uint64_t cpu_burst;

	while(1) {
		sem_wait(&SEM_SHORT_TERM_SCHEDULER);

		pcb = SCHEDULING_ALGORITHM->function_fetcher();

		switch_process_state(pcb, EXECUTING_STATE);

		int PCB_EXECUTE = 1;
		while(PCB_EXECUTE) {

			send_pcb(*pcb, CONNECTION_CPU_DISPATCH.fd_connection);

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

					pcb_deserialize(package->payload, pcb);
					eviction_reason_deserialize(package->payload, &eviction_reason);
					subpayload_deserialize(package->payload, syscall_instruction);
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

			switch(eviction_reason) {
				case ERROR_EVICTION_REASON:
				case EXIT_EVICTION_REASON:
					pcb->exit_reason = SUCCESS_EXIT_REASON;
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

void switch_process_state(t_PCB *pcb, e_Process_State new_state) {
	e_Process_State previous_state = pcb->current_state;
	pcb->current_state = new_state;
	char *previous_state_name;

bool _remover_por_pid(void *element) {
		return (((t_PCB *) element)->PID == pcb->PID);
}

	switch (previous_state) {
		case NEW_STATE:
			previous_state_name="NEW";
			pthread_mutex_lock(&mutex_LIST_NEW);
				list_remove_by_condition(LIST_NEW, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_NEW);
			break;
		case READY_STATE:
			previous_state_name="READY";
			pthread_mutex_lock(&mutex_LIST_READY);
				list_remove_by_condition(LIST_READY, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_READY);
			break;
		case EXECUTING_STATE:
		{
			previous_state_name="EXECUTING";
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
				list_remove_by_condition(LIST_EXECUTING, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			break;
		}
		case BLOCKED_STATE:
		{
			previous_state_name="BLOCKED";
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
				list_remove_by_condition(LIST_BLOCKED, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);		
			break;
		}
		case EXIT_STATE:
			previous_state_name="EXIT";
			pthread_mutex_lock(&mutex_LIST_EXIT);
				list_remove_by_condition(LIST_EXIT, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LIST_EXIT);		
			break;
		break;
	}

	switch(new_state) {
		case NEW_STATE: {
			pthread_mutex_lock(&mutex_LIST_NEW);
				list_add(LIST_NEW, pcb);
			pthread_mutex_unlock(&mutex_LIST_NEW);
			log_info(MINIMAL_LOGGER, "Se crea el proceso <%d> en NEW" ,pcb->PID);
			break;
		}
		case READY_STATE:
		{

			switch(SCHEDULING_ALGORITHM->type) {
				case VRR_SCHEDULING_ALGORITHM:

					if(pcb->quantum < QUANTUM) {
						pthread_mutex_lock(&MUTEX_LIST_READY_PRIORITARY);
							list_add(LIST_READY_PRIORITARY, pcb);
						pthread_mutex_unlock(&MUTEX_LIST_READY_PRIORITARY);
					} else {
						pthread_mutex_lock(&mutex_LIST_READY);
							list_add(LIST_READY, pcb);
						pthread_mutex_unlock(&mutex_LIST_READY);
					}
					break;
				case RR_SCHEDULING_ALGORITHM:
				case FIFO_SCHEDULING_ALGORITHM:
					pthread_mutex_lock(&mutex_LIST_READY);
						list_add(LIST_READY, pcb);
					pthread_mutex_unlock(&mutex_LIST_READY);
					break;
			}

			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->PID, previous_state_name);
			sem_post(&SEM_SHORT_TERM_SCHEDULER);
			break;
		}
		case EXECUTING_STATE: {
			
			pthread_mutex_lock(&mutex_LIST_EXECUTING);
				list_add(LIST_EXECUTING, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXECUTING);
			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXECUTING>",pcb->PID, previous_state_name);
	
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&mutex_LIST_BLOCKED);
				list_add(LIST_BLOCKED, pcb);
			pthread_mutex_unlock(&mutex_LIST_BLOCKED);
			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>",pcb->PID, previous_state_name);
			break;
		}
		case EXIT_STATE:
		{
			pthread_mutex_lock(&mutex_LIST_EXIT);
				list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LIST_EXIT);
			log_info(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXIT>",pcb->PID, previous_state_name);

			sem_post(&SEM_LONG_TERM_SCHEDULER_EXIT);
			
			break;
		}		
	}
}

t_PCB *pcb_create() {

	t_PCB *pcb = malloc(sizeof(t_PCB));
	if(pcb == NULL) {
		log_error(MODULE_LOGGER, "No se pudo reservar memoria para el PCB");
		exit(EXIT_FAILURE);
	}

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
	pcb->current_state = -1;
	pcb->quantum = QUANTUM;

	return pcb;
}

void send_interrupt(int socket)
{
    int dummy = 1;
    send(socket, &dummy, sizeof(dummy), 0);
}

void *start_quantum(void *pcb_parameter) {

	t_PCB *pcb = (t_PCB *) pcb_parameter;

    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");
    usleep(pcb->quantum * 1000); // en milisegundos
	// ENVIAR LA INTERRUPCIÓN SÓLO SI HAY MÁS PROCESOS EN READY
	// sem_wait()
    send_interrupt(CONNECTION_CPU_INTERRUPT.fd_connection);
	pthread_mutex_lock(&MUTEX_QUANTUM_INTERRUPT);
		QUANTUM_INTERRUPT = 1;
	pthread_mutex_unlock(&MUTEX_QUANTUM_INTERRUPT);
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