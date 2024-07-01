#include "scheduler.h"

t_PID PID_COUNTER = 0;
pthread_mutex_t MUTEX_PID_COUNTER;
t_PCB **PCB_ARRAY = NULL;
pthread_mutex_t MUTEX_PCB_ARRAY;
t_list *LIST_RELEASED_PIDS; // LIFO
pthread_mutex_t MUTEX_LIST_RELEASED_PIDS;
pthread_cond_t COND_LIST_RELEASED_PIDS;

const char *STATE_NAMES[] = {
	[NEW_STATE] = "NEW",
	[READY_STATE] = "READY",
	[EXEC_STATE] = "EXEC",
	[BLOCKED_STATE] = "BLOCKED",
	[EXIT_STATE] = "EXIT"
};

t_list *LIST_NEW;
pthread_mutex_t MUTEX_LIST_NEW;
t_list *LIST_READY;
pthread_mutex_t MUTEX_LIST_READY;
t_list *LIST_READY_PRIORITARY;
pthread_mutex_t MUTEX_LIST_READY_PRIORITARY;
t_list *LIST_EXEC;
pthread_mutex_t MUTEX_LIST_EXEC;
t_list *LIST_BLOCKED;
pthread_mutex_t MUTEX_LIST_BLOCKED;
t_list *LIST_EXIT;
pthread_mutex_t MUTEX_LIST_EXIT;

pthread_t THREAD_LONG_TERM_SCHEDULER_NEW;
sem_t SEM_LONG_TERM_SCHEDULER_NEW;
pthread_t THREAD_LONG_TERM_SCHEDULER_EXIT;
sem_t SEM_LONG_TERM_SCHEDULER_EXIT;
pthread_t THREAD_SHORT_TERM_SCHEDULER;
sem_t SEM_SHORT_TERM_SCHEDULER;

t_Scheduling_Algorithm SCHEDULING_ALGORITHMS[] = {
	[FIFO_SCHEDULING_ALGORITHM] = { .name = "FIFO" , .function_fetcher = FIFO_scheduling_algorithm},
	[RR_SCHEDULING_ALGORITHM] = { .name = "RR" , .function_fetcher = RR_scheduling_algorithm },
	[VRR_SCHEDULING_ALGORITHM] = { .name = "VRR" , .function_fetcher = VRR_scheduling_algorithm },
};

e_Scheduling_Algorithm SCHEDULING_ALGORITHM;

t_Quantum QUANTUM;
pthread_t THREAD_QUANTUM_INTERRUPT;
pthread_mutex_t MUTEX_QUANTUM_INTERRUPT;
int QUANTUM_INTERRUPT;

t_temporal *TEMPORAL_DISPATCHED;

const char *EXIT_REASONS[] = {
	[SUCCESS_EXIT_REASON] = "SUCCESS",
	[INVALID_RESOURCE_EXIT_REASON] = "INVALID_RESOURCE",
	[INVALID_INTERFACE_EXIT_REASON] = "INVALID_INTERFACE",
	[OUT_OF_MEMORY_EXIT_REASON] = "OUT_OF_MEMORY",
	[INTERRUPTED_BY_USER_EXIT_REASON] = "INTERRUPTED_BY_USER"
};

unsigned int MULTIPROGRAMMING_LEVEL;
sem_t SEM_MULTIPROGRAMMING_LEVEL;
unsigned int MULTIPROGRAMMING_DIFFERENCE = 0;
pthread_mutex_t MUTEX_MULTIPROGRAMMING_DIFFERENCE;
sem_t SEM_MULTIPROGRAMMING_POSTER;
pthread_t THREAD_MULTIPROGRAMMING_POSTER;

/*
sem_t sem_detener_execute;
sem_t sem_detener_new_ready;
sem_t sem_detener_block_ready;
sem_t sem_detener_block;
sem_t sem_detener_planificacion;
*/

int LIST_PROCESS_STATES = 0;
pthread_mutex_t MUTEX_LIST_PROCESS_STATES;
pthread_cond_t COND_LIST_PROCESS_STATES;
sem_t SEM_SWITCHING_STATES_COUNT;
pthread_cond_t COND_SWITCHING_STATES;

int find_scheduling_algorithm(char *name, e_Scheduling_Algorithm *destination) {

    if(name == NULL || destination == NULL)
        return 1;
    
    size_t scheduling_algorithms_number = sizeof(SCHEDULING_ALGORITHMS) / sizeof(SCHEDULING_ALGORITHMS[0]);
    for (register e_Scheduling_Algorithm scheduling_algorithm = 0; scheduling_algorithm < scheduling_algorithms_number; scheduling_algorithm++)
        if (!strcmp(SCHEDULING_ALGORITHMS[scheduling_algorithm].name, name)) {
            *destination = scheduling_algorithm;
            return 0;
        }

    return 1;
}

void initialize_scheduling(void) {

	LIST_RELEASED_PIDS = list_create();

	LIST_NEW = list_create();
	LIST_READY = list_create();
	LIST_READY_PRIORITARY = list_create();
	LIST_EXEC = list_create();
	LIST_BLOCKED = list_create();
	LIST_EXIT = list_create();

	initialize_long_term_scheduler();
	initialize_short_term_scheduler();
}

void pause_scheduling(void) {
	/*
	sem_wait(&sem_detener_planificacion);
	sem_wait(&sem_detener_new_ready);
	sem_wait(&sem_detener_block_ready);
	sem_wait(&sem_detener_block);
	sem_wait(&sem_detener_execute);
	sem_post(&sem_detener_planificacion);
	*/
}

void resume_scheduling(void) {
	/*
	sem_wait(&sem_detener_planificacion);
	sem_post(&sem_detener_new_ready);
	sem_post(&sem_detener_block_ready);
	sem_post(&sem_detener_block);
	sem_post(&sem_detener_execute);
	sem_post(&sem_detener_planificacion);
	*/
}

void finish_scheduling(void) {
	free(PCB_ARRAY);
	list_destroy_and_destroy_elements(LIST_RELEASED_PIDS, free);
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
	pthread_create(&THREAD_MULTIPROGRAMMING_POSTER, NULL, multiprogramming_poster, NULL);
	//log_info(MODULE_LOGGER, "Inicio planificador corto plazo");
	pthread_detach(THREAD_MULTIPROGRAMMING_POSTER);
}

void *long_term_scheduler_new(void *parameter) {

	t_PCB *pcb;

	while(1) {
		sem_wait(&SEM_LONG_TERM_SCHEDULER_NEW);
		sem_wait(&SEM_MULTIPROGRAMMING_LEVEL);

    	wait_list_process_states();
			pthread_mutex_lock(&MUTEX_LIST_NEW);

				if(LIST_NEW->head == NULL) {
					pthread_mutex_unlock(&MUTEX_LIST_NEW);
					signal_list_process_states();
					continue;
				}

				pcb = (t_PCB *) LIST_NEW->head->data;
			pthread_mutex_unlock(&MUTEX_LIST_NEW);

			switch_process_state(pcb, READY_STATE);
		signal_list_process_states();
	}

	return NULL;
}

void *long_term_scheduler_exit(void *NULL_parameter) {
	t_PCB *pcb;
	t_Return_Value return_value;

	while(1) {
		sem_wait(&SEM_LONG_TERM_SCHEDULER_EXIT);

		wait_list_process_states();
			pthread_mutex_lock(&MUTEX_LIST_EXIT);
				pcb = (t_PCB *) list_remove(LIST_EXIT, 0);
			pthread_mutex_unlock(&MUTEX_LIST_EXIT);
		signal_list_process_states();

		send_process_destroy(pcb->PID, CONNECTION_MEMORY.fd_connection); 

		receive_return_value_with_header(PROCESS_DESTROY_HEADER, &return_value, CONNECTION_MEMORY.fd_connection);
		if(return_value) {
			log_warning(MODULE_LOGGER, "[Memoria]: No se pudo FINALIZAR_PROCESO %" PRIu32, pcb->PID);
		} else {
			log_debug(MINIMAL_LOGGER, "Finaliza el proceso <%d> - Motivo: <%s>", pcb->PID, EXIT_REASONS[pcb->exit_reason]);
		}

		//pid_release(pcb->PID);
		free(pcb);
		sem_post(&SEM_MULTIPROGRAMMING_POSTER);
		
	}

	return NULL;
}

void *short_term_scheduler(void *parameter) {
 
	t_PCB *pcb;
	e_Eviction_Reason eviction_reason;
	t_Payload *syscall_instruction = payload_create();
	int exit_status;
	int64_t cpu_burst;

	while(1) {
		sem_wait(&SEM_SHORT_TERM_SCHEDULER);

		wait_list_process_states();
			pcb = SCHEDULING_ALGORITHMS[SCHEDULING_ALGORITHM].function_fetcher();
			if(pcb == NULL) {
				signal_list_process_states();
				continue;
			}
			switch_process_state(pcb, EXEC_STATE);
		signal_list_process_states();

		int PCB_EXECUTE = 1;
		while(PCB_EXECUTE) {

			send_process_dispatch(*pcb, CONNECTION_CPU_DISPATCH.fd_connection);

			switch(SCHEDULING_ALGORITHM) {
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

			receive_process_eviction(pcb, &eviction_reason, syscall_instruction, CONNECTION_CPU_DISPATCH.fd_connection);

			switch(SCHEDULING_ALGORITHM) {
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

			switch(SCHEDULING_ALGORITHM) {
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

			wait_list_process_states();

				switch(eviction_reason) {
					case ERROR_EVICTION_REASON:
					case EXIT_EVICTION_REASON:
						pcb->exit_reason = SUCCESS_EXIT_REASON;
						switch_process_state(pcb, EXIT_STATE);
						PCB_EXECUTE = 0;
						break;

					case INTERRUPTION_EVICTION_REASON:
						// FALTARÍA DISTINGUIR SI LA INTERRUPCIÓN FUE POR FIN DE QUANTUM O POR KILL
						log_debug(MINIMAL_LOGGER, "PID: <%d> - Desalojado por fin de Quantum", (int) pcb->PID);
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

			signal_list_process_states();
			// instruction_free(instruction);
		}
	}

	return NULL;
}

void *multiprogramming_poster(void *NULL_argument) {

    while(1) {
        sem_wait(&SEM_MULTIPROGRAMMING_POSTER);

        pthread_mutex_lock(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);

            if(MULTIPROGRAMMING_DIFFERENCE == 0)
                sem_post(&SEM_MULTIPROGRAMMING_LEVEL);
            else
                MULTIPROGRAMMING_DIFFERENCE--;

        pthread_mutex_unlock(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
    }

    return NULL;
}

t_PCB *FIFO_scheduling_algorithm(void) {
	t_PCB *pcb = NULL;

	pthread_mutex_lock(&MUTEX_LIST_READY);
		if(LIST_READY->head != NULL)
			pcb = (t_PCB *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&MUTEX_LIST_READY);

	return pcb;
}

t_PCB *RR_scheduling_algorithm(void) {
	t_PCB *pcb = NULL;

	pthread_mutex_lock(&MUTEX_LIST_READY);
		if(LIST_READY->head != NULL)
			pcb = (t_PCB *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&MUTEX_LIST_READY);

	return pcb;
}

t_PCB *VRR_scheduling_algorithm(void) {
	t_PCB *pcb = NULL;

	pthread_mutex_lock(&MUTEX_LIST_READY_PRIORITARY);
		if(LIST_READY_PRIORITARY->head != NULL)
			pcb = (t_PCB *) list_remove(LIST_READY_PRIORITARY, 0);
	pthread_mutex_unlock(&MUTEX_LIST_READY_PRIORITARY);

	if(pcb != NULL)
		return pcb;
	
	pthread_mutex_lock(&MUTEX_LIST_READY);
		if(LIST_READY->head != NULL)
			pcb = (t_PCB *) list_remove(LIST_READY, 0);
	pthread_mutex_unlock(&MUTEX_LIST_READY);

	return pcb;
}

void switch_process_state(t_PCB *pcb, e_Process_State new_state) {

	e_Process_State previous_state = pcb->current_state;
	pcb->current_state = new_state;

	switch(previous_state) {
		case NEW_STATE:
			pthread_mutex_lock(&MUTEX_LIST_NEW);
				remove_pcb_from_list_by_pid(LIST_NEW, pcb->PID);
			pthread_mutex_unlock(&MUTEX_LIST_NEW);
			break;
		case READY_STATE:
		{
			switch(SCHEDULING_ALGORITHM) {
				case VRR_SCHEDULING_ALGORITHM:

					if(pcb->quantum < QUANTUM) {
						pthread_mutex_lock(&MUTEX_LIST_READY_PRIORITARY);
							remove_pcb_from_list_by_pid(LIST_READY_PRIORITARY, pcb->PID);
						pthread_mutex_unlock(&MUTEX_LIST_READY_PRIORITARY);
					} else {
						pthread_mutex_lock(&MUTEX_LIST_READY);
							remove_pcb_from_list_by_pid(LIST_READY, pcb->PID);
						pthread_mutex_unlock(&MUTEX_LIST_READY);
					}
					break;
				case RR_SCHEDULING_ALGORITHM:
				case FIFO_SCHEDULING_ALGORITHM:
					pthread_mutex_lock(&MUTEX_LIST_READY);
						remove_pcb_from_list_by_pid(LIST_READY, pcb->PID);
					pthread_mutex_unlock(&MUTEX_LIST_READY);
					break;
			}
			break;
		}
		case EXEC_STATE:
		{
			pthread_mutex_lock(&MUTEX_LIST_EXEC);
				remove_pcb_from_list_by_pid(LIST_EXEC, pcb->PID);
			pthread_mutex_unlock(&MUTEX_LIST_EXEC);
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&MUTEX_LIST_BLOCKED);
				remove_pcb_from_list_by_pid(LIST_BLOCKED, pcb->PID);
			pthread_mutex_unlock(&MUTEX_LIST_BLOCKED);		
			break;
		}
		default:

			break;
	}

	switch(new_state) {
		case READY_STATE:
		{

			switch(SCHEDULING_ALGORITHM) {
				case VRR_SCHEDULING_ALGORITHM:

					if(pcb->quantum < QUANTUM) {
						pthread_mutex_lock(&MUTEX_LIST_READY_PRIORITARY);
							list_add(LIST_READY_PRIORITARY, pcb);
							log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", (int) pcb->PID, STATE_NAMES[previous_state]);
							log_state_list(MODULE_LOGGER, "Ready Prioridad", LIST_READY_PRIORITARY);
						pthread_mutex_unlock(&MUTEX_LIST_READY_PRIORITARY);
					} else {
						pthread_mutex_lock(&MUTEX_LIST_READY);
							list_add(LIST_READY, pcb);
							log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", (int) pcb->PID, STATE_NAMES[previous_state]);
							log_state_list(MODULE_LOGGER, "Cola Ready", LIST_READY);
						pthread_mutex_unlock(&MUTEX_LIST_READY);
					}
					break;
				case RR_SCHEDULING_ALGORITHM:
				case FIFO_SCHEDULING_ALGORITHM:
					pthread_mutex_lock(&MUTEX_LIST_READY);
						list_add(LIST_READY, pcb);
						log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", (int) pcb->PID, STATE_NAMES[previous_state]);
						log_state_list(MODULE_LOGGER, "Cola Ready", LIST_READY);
					pthread_mutex_unlock(&MUTEX_LIST_READY);
					break;
			}

			sem_post(&SEM_SHORT_TERM_SCHEDULER);
			break;
		}
		case EXEC_STATE:
		{
			
			pthread_mutex_lock(&MUTEX_LIST_EXEC);
				list_add(LIST_EXEC, pcb);
			pthread_mutex_unlock(&MUTEX_LIST_EXEC);
			log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXEC>", (int) pcb->PID, STATE_NAMES[previous_state]);
	
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&MUTEX_LIST_BLOCKED);
				list_add(LIST_BLOCKED, pcb);
			pthread_mutex_unlock(&MUTEX_LIST_BLOCKED);
			log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>", (int) pcb->PID, STATE_NAMES[previous_state]);
			break;
		}
		case EXIT_STATE:
		{
			pthread_mutex_lock(&MUTEX_LIST_EXIT);
				list_add(LIST_EXIT, pcb);
			pthread_mutex_unlock(&MUTEX_LIST_EXIT);
			log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXIT>", (int) pcb->PID, STATE_NAMES[previous_state]);

			sem_post(&SEM_LONG_TERM_SCHEDULER_EXIT);
			
			break;
		}
		default:
			break;
	}
}

t_PCB *remove_pcb_from_list_by_pid(t_list *pcb_list, t_PID pid) {
	t_link_element **indirect = &(pcb_list->head);
    while (*indirect != NULL) {
        if (((t_PCB *) (*indirect)->data)->PID == pid) {
            t_link_element *removed_element = *indirect;

            *indirect = (*indirect)->next;
            pcb_list->elements_count--;

			t_PCB *pcb = (t_PCB *) removed_element->data;
            free(removed_element);
			return pcb;
        }
        indirect = &((*indirect)->next);
	}
	return NULL;
}

void log_state_list(t_log *logger, const char *state_name, t_list *pcb_list) {
	char *pid_string = string_new();
	pcb_list_to_pid_string(pcb_list, &pid_string);
	log_info(logger, "%s: %s", state_name, pid_string);
	free(pid_string);
}

void pcb_list_to_pid_string(t_list *pcb_list, char **destination) {
	if(destination == NULL || *destination == NULL || pcb_list == NULL)
		return;

	t_link_element *element = pcb_list->head;

	if(**destination && element != NULL)
		string_append(destination, ", ");

	char *pid_as_string;
	while(element != NULL) {
        pid_as_string = string_from_format("%" PRIu32, ((t_PCB *) element->data)->PID);
        string_append(destination, pid_as_string);
        free(pid_as_string);
		element = element->next;
        if(element != NULL)
            string_append(destination, ", ");
    }
}

t_PCB *pcb_create() {

	t_PCB *pcb = malloc(sizeof(t_PCB));
	if(pcb == NULL) {
		log_error(MODULE_LOGGER, "No se pudo reservar memoria para el PCB");
		exit(EXIT_FAILURE);
	}

	pcb->PID = pid_assign(pcb);
    pcb->PC = 0; 
    pcb->cpu_registers.AX = 0;
    pcb->cpu_registers.BX = 0;
    pcb->cpu_registers.CX = 0;
    pcb->cpu_registers.DX = 0;
    pcb->cpu_registers.EAX = 0;
    pcb->cpu_registers.EBX = 0;
    pcb->cpu_registers.ECX = 0;
    pcb->cpu_registers.EDX = 0;
    pcb->cpu_registers.RAX = 0;
    pcb->cpu_registers.RBX = 0;
    pcb->cpu_registers.RCX = 0;
    pcb->cpu_registers.RDX = 0;
    pcb->cpu_registers.SI = 0;
    pcb->cpu_registers.DI = 0;
	pcb->current_state = NEW_STATE;
	pcb->quantum = QUANTUM;

	return pcb;
}

t_PID pid_assign(t_PCB *pcb) {

	pthread_mutex_lock(&MUTEX_LIST_RELEASED_PIDS);
	if(LIST_RELEASED_PIDS->head == NULL && PID_COUNTER <= PID_MAX) {
		pthread_mutex_unlock(&MUTEX_LIST_RELEASED_PIDS);

		pthread_mutex_lock(&MUTEX_PCB_ARRAY);
			t_PCB **new_pcb_array = realloc(PCB_ARRAY, sizeof(t_PCB *) * (PID_COUNTER + 1));
			if(new_pcb_array == NULL) {
				log_error(MODULE_LOGGER, "No se pudo reservar memoria para el array de PCBs");
				exit(EXIT_FAILURE);
			}
			PCB_ARRAY = new_pcb_array;

			PCB_ARRAY[PID_COUNTER] = pcb;
		pthread_mutex_unlock(&MUTEX_PCB_ARRAY);

		return PID_COUNTER++;
	}

	while(LIST_RELEASED_PIDS->head == NULL)
		pthread_cond_wait(&COND_LIST_RELEASED_PIDS, &MUTEX_LIST_RELEASED_PIDS);

	t_link_element *element = LIST_RELEASED_PIDS->head;

	LIST_RELEASED_PIDS->head = element->next;
	LIST_RELEASED_PIDS->elements_count--;

	pthread_mutex_unlock(&MUTEX_LIST_RELEASED_PIDS);

	t_PID pid = (*(t_PID *) element->data);

	free(element->data);
	free(element);

	pthread_mutex_lock(&MUTEX_PCB_ARRAY);
		PCB_ARRAY[pid] = pcb;
	pthread_mutex_unlock(&MUTEX_PCB_ARRAY);

	return pid;

}

void pid_release(t_PID pid) {
	pthread_mutex_lock(&MUTEX_PCB_ARRAY);
		PCB_ARRAY[pid] = NULL;
	pthread_mutex_unlock(&MUTEX_PCB_ARRAY);

	t_link_element *element = malloc(sizeof(t_link_element));
		if(element == NULL) {
			log_error(MODULE_LOGGER, "No se pudo reservar memoria para el elemento de la lista de PID libres");
			exit(EXIT_FAILURE);
		}

		element->data = malloc(sizeof(t_PID));
		if(element->data == NULL) {
			log_error(MODULE_LOGGER, "No se pudo reservar memoria para el PID");
			exit(EXIT_FAILURE);
		}
		(*(t_PID *) element->data) = pid;

		pthread_mutex_lock(&MUTEX_LIST_RELEASED_PIDS);
			element->next = LIST_RELEASED_PIDS->head;
			LIST_RELEASED_PIDS->head = element;
		pthread_mutex_unlock(&MUTEX_LIST_RELEASED_PIDS);

		pthread_cond_signal(&COND_LIST_RELEASED_PIDS);
}

void *start_quantum(void *pcb_parameter) {

	t_PCB *pcb = (t_PCB *) pcb_parameter;

    log_trace(MODULE_LOGGER, "Se crea hilo para INTERRUPT");

    usleep(pcb->quantum * 1000); // en milisegundos

	// ENVIAR LA INTERRUPCIÓN SÓLO SI HAY MÁS PROCESOS EN READY
	sem_wait(&SEM_SHORT_TERM_SCHEDULER);

	pthread_mutex_lock(&MUTEX_QUANTUM_INTERRUPT);
		QUANTUM_INTERRUPT = 1;
	pthread_mutex_unlock(&MUTEX_QUANTUM_INTERRUPT);

    send_kernel_interrupt(QUANTUM_KERNEL_INTERRUPT, pcb->PID, CONNECTION_CPU_INTERRUPT.fd_connection);

    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %li milisegundos", pcb->quantum);

	return NULL;
}