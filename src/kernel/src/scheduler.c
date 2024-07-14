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

t_Shared_List SHARED_LIST_NEW;
t_Shared_List SHARED_LIST_READY;
t_Shared_List SHARED_LIST_READY_PRIORITARY;
t_Shared_List SHARED_LIST_EXEC;
t_Shared_List SHARED_LIST_BLOCKED;
t_Shared_List SHARED_LIST_EXIT;

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
	[UNEXPECTED_ERROR_EXIT_REASON] = "UNEXPECTED ERROR",

	[SUCCESS_EXIT_REASON] = "SUCCESS",
	[INVALID_RESOURCE_EXIT_REASON] = "INVALID_RESOURCE",
	[INVALID_INTERFACE_EXIT_REASON] = "INVALID_INTERFACE",
	[OUT_OF_MEMORY_EXIT_REASON] = "OUT_OF_MEMORY",
	[INTERRUPTED_BY_USER_EXIT_REASON] = "INTERRUPTED_BY_USER"
};

int KILL_EXECUTING_PROCESS = 0;
pthread_mutex_t MUTEX_KILL_EXECUTING_PROCESS;

unsigned int MULTIPROGRAMMING_LEVEL;
sem_t SEM_MULTIPROGRAMMING_LEVEL;
unsigned int MULTIPROGRAMMING_DIFFERENCE = 0;
pthread_mutex_t MUTEX_MULTIPROGRAMMING_DIFFERENCE;
sem_t SEM_MULTIPROGRAMMING_POSTER;
pthread_t THREAD_MULTIPROGRAMMING_POSTER;

int SCHEDULING_PAUSED;
pthread_mutex_t MUTEX_SCHEDULING_PAUSED;

t_Drain_Ongoing_Resource_Sync SCHEDULING_SYNC;

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

	SHARED_LIST_NEW.list = list_create();
	SHARED_LIST_READY.list = list_create();
	SHARED_LIST_READY_PRIORITARY.list = list_create();
	SHARED_LIST_EXEC.list = list_create();
	SHARED_LIST_BLOCKED.list = list_create();
	SHARED_LIST_EXIT.list = list_create();
	
	LIST_INTERFACES = list_create();

	initialize_long_term_scheduler();
	initialize_short_term_scheduler();
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

    	wait_draining_requests(&SCHEDULING_SYNC);
			pthread_mutex_lock(&(SHARED_LIST_NEW.mutex));

				if((SHARED_LIST_NEW.list)->head == NULL) {
					pthread_mutex_unlock(&(SHARED_LIST_NEW.mutex));
					signal_draining_requests(&SCHEDULING_SYNC);
					continue;
				}

				pcb = (t_PCB *) (SHARED_LIST_NEW.list)->head->data;
			
			pthread_mutex_unlock(&(SHARED_LIST_NEW.mutex));

			switch_process_state(pcb, READY_STATE);
		signal_draining_requests(&SCHEDULING_SYNC);
	}

	return NULL;
}

void *long_term_scheduler_exit(void *NULL_parameter) {
	t_PCB *pcb;
	t_Return_Value return_value;

	while(1) {
		sem_wait(&SEM_LONG_TERM_SCHEDULER_EXIT);

		wait_draining_requests(&SCHEDULING_SYNC);
			pthread_mutex_lock(&(SHARED_LIST_EXIT.mutex));
				pcb = (t_PCB *) list_remove((SHARED_LIST_EXIT.list), 0);
			pthread_mutex_unlock(&(SHARED_LIST_EXIT.mutex));
		signal_draining_requests(&SCHEDULING_SYNC);

		send_process_destroy(pcb->exec_context.PID, CONNECTION_MEMORY.fd_connection);

		// TODO: Falta liberar recursos asignados al proceso

		receive_return_value_with_expected_header(PROCESS_DESTROY_HEADER, &return_value, CONNECTION_MEMORY.fd_connection);
		if(return_value) {
			log_warning(MODULE_LOGGER, "[Memoria]: No se pudo FINALIZAR_PROCESO %" PRIu32, pcb->exec_context.PID);
		} else {
			log_debug(MINIMAL_LOGGER, "Finaliza el proceso <%d> - Motivo: <%s>", pcb->exec_context.PID, EXIT_REASONS[pcb->exec_context.exit_reason]);
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

		wait_draining_requests(&SCHEDULING_SYNC);
			pcb = SCHEDULING_ALGORITHMS[SCHEDULING_ALGORITHM].function_fetcher();
			if(pcb == NULL) {
				signal_draining_requests(&SCHEDULING_SYNC);
				continue;
			}
			switch_process_state(pcb, EXEC_STATE);
		signal_draining_requests(&SCHEDULING_SYNC);

		int PCB_EXECUTE = 1;
		while(PCB_EXECUTE) {

			send_process_dispatch(pcb->exec_context, CONNECTION_CPU_DISPATCH.fd_connection);

			switch(SCHEDULING_ALGORITHM) {
				case RR_SCHEDULING_ALGORITHM:
					QUANTUM_INTERRUPT = 0;
					pthread_create(&THREAD_QUANTUM_INTERRUPT, NULL, start_quantum, (void *) &(pcb->exec_context.quantum));
					break;
				case VRR_SCHEDULING_ALGORITHM:
					TEMPORAL_DISPATCHED = temporal_create();
					QUANTUM_INTERRUPT = 0;
					pthread_create(&THREAD_QUANTUM_INTERRUPT, NULL, start_quantum, (void *) &(pcb->exec_context.quantum));
					break;
				case FIFO_SCHEDULING_ALGORITHM:
					break;
			}

			receive_process_eviction(&(pcb->exec_context), &eviction_reason, syscall_instruction, CONNECTION_CPU_DISPATCH.fd_connection);

			switch(SCHEDULING_ALGORITHM) {
				case RR_SCHEDULING_ALGORITHM:

					pthread_mutex_lock(&MUTEX_QUANTUM_INTERRUPT);
						if(!QUANTUM_INTERRUPT)
							pthread_cancel(THREAD_QUANTUM_INTERRUPT);
					pthread_mutex_unlock(&MUTEX_QUANTUM_INTERRUPT);
					pthread_join(THREAD_QUANTUM_INTERRUPT, NULL);

					break;
				case VRR_SCHEDULING_ALGORITHM:

					pthread_mutex_lock(&MUTEX_QUANTUM_INTERRUPT);
						if(!QUANTUM_INTERRUPT)
							pthread_cancel(THREAD_QUANTUM_INTERRUPT);
					pthread_mutex_unlock(&MUTEX_QUANTUM_INTERRUPT);
					pthread_join(THREAD_QUANTUM_INTERRUPT, NULL);

					temporal_stop(TEMPORAL_DISPATCHED);
						cpu_burst = temporal_gettime(TEMPORAL_DISPATCHED);
					temporal_destroy(TEMPORAL_DISPATCHED);

					pcb->exec_context.quantum -= cpu_burst;
					if(pcb->exec_context.quantum <= 0)
						pcb->exec_context.quantum = QUANTUM;

					break;
				case FIFO_SCHEDULING_ALGORITHM:
					break;
			}

			wait_draining_requests(&SCHEDULING_SYNC);

				switch(eviction_reason) {
					case UNEXPECTED_ERROR_EVICTION_REASON:
						pcb->exec_context.exit_reason = UNEXPECTED_ERROR_EXIT_REASON;
						switch_process_state(pcb, EXIT_STATE);
						PCB_EXECUTE = 0;
						break;

					case EXIT_EVICTION_REASON:
						pcb->exec_context.exit_reason = SUCCESS_EXIT_REASON;
						switch_process_state(pcb, EXIT_STATE);
						PCB_EXECUTE = 0;
						break;

					case KILL_KERNEL_INTERRUPT_EVICTION_REASON:
						switch_process_state(pcb, EXIT_STATE);
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

					case QUANTUM_KERNEL_INTERRUPT_EVICTION_REASON:
						log_debug(MINIMAL_LOGGER, "PID: <%d> - Desalojado por fin de Quantum", (int) pcb->exec_context.PID);
						switch_process_state(pcb, READY_STATE);
						PCB_EXECUTE = 0;
						break;
				}

			signal_draining_requests(&SCHEDULING_SYNC);
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

	pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
		if((SHARED_LIST_READY.list)->head != NULL)
			pcb = (t_PCB *) list_remove((SHARED_LIST_READY.list), 0);
	pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));

	return pcb;
}

t_PCB *RR_scheduling_algorithm(void) {
	t_PCB *pcb = NULL;

	pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
		if((SHARED_LIST_READY.list)->head != NULL)
			pcb = (t_PCB *) list_remove((SHARED_LIST_READY.list), 0);
	pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));

	return pcb;
}

t_PCB *VRR_scheduling_algorithm(void) {
	t_PCB *pcb = NULL;

	pthread_mutex_lock(&(SHARED_LIST_READY_PRIORITARY.mutex));
		if((SHARED_LIST_READY_PRIORITARY.list)->head != NULL)
			pcb = (t_PCB *) list_remove((SHARED_LIST_READY_PRIORITARY.list), 0);
	pthread_mutex_unlock(&(SHARED_LIST_READY_PRIORITARY.mutex));

	if(pcb != NULL)
		return pcb;
	
	pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
		if((SHARED_LIST_READY.list)->head != NULL)
			pcb = (t_PCB *) list_remove((SHARED_LIST_READY.list), 0);
	pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));

	return pcb;
}

void switch_process_state(t_PCB *pcb, e_Process_State new_state) {

	e_Process_State previous_state = pcb->current_state;
	pcb->current_state = new_state;

	switch(previous_state) {
		case NEW_STATE:
			pthread_mutex_lock(&(SHARED_LIST_NEW.mutex));
				remove_pcb_from_list_by_pid((SHARED_LIST_NEW.list), pcb->exec_context.PID);
			pthread_mutex_unlock(&(SHARED_LIST_NEW.mutex));
			break;
		case READY_STATE:
		{
			switch(SCHEDULING_ALGORITHM) {
				case VRR_SCHEDULING_ALGORITHM:

					if(pcb->exec_context.quantum < QUANTUM) {
						pthread_mutex_lock(&(SHARED_LIST_READY_PRIORITARY.mutex));
							remove_pcb_from_list_by_pid((SHARED_LIST_READY_PRIORITARY.list), pcb->exec_context.PID);
						pthread_mutex_unlock(&(SHARED_LIST_READY_PRIORITARY.mutex));
					} else {
						pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
							remove_pcb_from_list_by_pid((SHARED_LIST_READY.list), pcb->exec_context.PID);
						pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));
					}
					break;
				case RR_SCHEDULING_ALGORITHM:
				case FIFO_SCHEDULING_ALGORITHM:
					pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
						remove_pcb_from_list_by_pid((SHARED_LIST_READY.list), pcb->exec_context.PID);
					pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));
					break;
			}
			break;
		}
		case EXEC_STATE:
		{
			pthread_mutex_lock(&(SHARED_LIST_EXEC.mutex));
				remove_pcb_from_list_by_pid((SHARED_LIST_EXEC.list), pcb->exec_context.PID);
			pthread_mutex_unlock(&(SHARED_LIST_EXEC.mutex));
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&(SHARED_LIST_BLOCKED.mutex));
				remove_pcb_from_list_by_pid((SHARED_LIST_BLOCKED.list), pcb->exec_context.PID);
			pthread_mutex_unlock(&(SHARED_LIST_BLOCKED.mutex));		
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

					if(pcb->exec_context.quantum < QUANTUM) {
						pthread_mutex_lock(&(SHARED_LIST_READY_PRIORITARY.mutex));
							list_add((SHARED_LIST_READY_PRIORITARY.list), pcb);
							log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", (int) pcb->exec_context.PID, STATE_NAMES[previous_state]);
							log_state_list(MODULE_LOGGER, "Ready Prioridad", (SHARED_LIST_READY_PRIORITARY.list));
						pthread_mutex_unlock(&(SHARED_LIST_READY_PRIORITARY.mutex));
					} else {
						pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
							list_add((SHARED_LIST_READY.list), pcb);
							log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", (int) pcb->exec_context.PID, STATE_NAMES[previous_state]);
							log_state_list(MODULE_LOGGER, "Cola Ready", (SHARED_LIST_READY.list));
						pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));
					}
					break;
				case RR_SCHEDULING_ALGORITHM:
				case FIFO_SCHEDULING_ALGORITHM:
					pthread_mutex_lock(&(SHARED_LIST_READY.mutex));
						list_add((SHARED_LIST_READY.list), pcb);
						log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", (int) pcb->exec_context.PID, STATE_NAMES[previous_state]);
						log_state_list(MODULE_LOGGER, "Cola Ready", (SHARED_LIST_READY.list));
					pthread_mutex_unlock(&(SHARED_LIST_READY.mutex));
					break;
			}

			sem_post(&SEM_SHORT_TERM_SCHEDULER);
			break;
		}
		case EXEC_STATE:
		{
			
			pthread_mutex_lock(&(SHARED_LIST_EXEC.mutex));
				list_add((SHARED_LIST_EXEC.list), pcb);
			pthread_mutex_unlock(&(SHARED_LIST_EXEC.mutex));
			log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXEC>", (int) pcb->exec_context.PID, STATE_NAMES[previous_state]);
	
			break;
		}
		case BLOCKED_STATE:
		{
			pthread_mutex_lock(&(SHARED_LIST_BLOCKED.mutex));
				list_add((SHARED_LIST_BLOCKED.list), pcb);
			pthread_mutex_unlock(&(SHARED_LIST_BLOCKED.mutex));
			log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>", (int) pcb->exec_context.PID, STATE_NAMES[previous_state]);
			
			break;
		}
		case EXIT_STATE:
		{
			pthread_mutex_lock(&(SHARED_LIST_EXIT.mutex));
				list_add((SHARED_LIST_EXIT.list), pcb);
			pthread_mutex_unlock(&(SHARED_LIST_EXIT.mutex));
			log_debug(MINIMAL_LOGGER, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXIT>", (int) pcb->exec_context.PID, STATE_NAMES[previous_state]);

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
        if (((t_PCB *) (*indirect)->data)->exec_context.PID == pid) {
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
        pid_as_string = string_from_format("%" PRIu32, ((t_PCB *) element->data)->exec_context.PID);
        string_append(destination, pid_as_string);
        free(pid_as_string);
		element = element->next;
        if(element != NULL)
            string_append(destination, ", ");
    }
}

t_PCB *pcb_create(void) {

	t_PCB *pcb = malloc(sizeof(t_PCB));
	if(pcb == NULL) {
		log_error(MODULE_LOGGER, "No se pudo reservar memoria para el PCB");
		exit(EXIT_FAILURE);
	}

	pcb->exec_context.PID = pid_assign(pcb);
    pcb->exec_context.PC = 0;
	pcb->exec_context.quantum = QUANTUM;
    pcb->exec_context.cpu_registers.AX = 0;
    pcb->exec_context.cpu_registers.BX = 0;
    pcb->exec_context.cpu_registers.CX = 0;
    pcb->exec_context.cpu_registers.DX = 0;
    pcb->exec_context.cpu_registers.EAX = 0;
    pcb->exec_context.cpu_registers.EBX = 0;
    pcb->exec_context.cpu_registers.ECX = 0;
    pcb->exec_context.cpu_registers.EDX = 0;
    pcb->exec_context.cpu_registers.RAX = 0;
    pcb->exec_context.cpu_registers.RBX = 0;
    pcb->exec_context.cpu_registers.RCX = 0;
    pcb->exec_context.cpu_registers.RDX = 0;
    pcb->exec_context.cpu_registers.SI = 0;
    pcb->exec_context.cpu_registers.DI = 0;

	pcb->current_state = NEW_STATE;
	pcb->shared_list_state = NULL;

	pcb->instruction = NULL;

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

    usleep((pcb->exec_context.quantum) * 1000); // en milisegundos

	// ENVIAR LA INTERRUPCIÓN SÓLO SI HAY MÁS PROCESOS EN READY
	sem_wait(&SEM_SHORT_TERM_SCHEDULER);

	pthread_mutex_lock(&MUTEX_QUANTUM_INTERRUPT);
		QUANTUM_INTERRUPT = 1;
	pthread_mutex_unlock(&MUTEX_QUANTUM_INTERRUPT);

    send_kernel_interrupt(QUANTUM_KERNEL_INTERRUPT, pcb->exec_context.PID, CONNECTION_CPU_INTERRUPT.fd_connection);

    log_trace(MODULE_LOGGER, "Envie interrupcion por Quantum tras %li milisegundos", pcb->exec_context.quantum);

	return NULL;
}

void wait_ongoing(t_Drain_Ongoing_Resource_Sync *resource_sync) {
    sem_post(&(resource_sync->sem_drain_requests_count));

    int sem_value;
    pthread_mutex_lock(&(resource_sync->mutex_resource));
        while(1) {
            sem_getvalue(&(resource_sync->sem_ongoing_count), &sem_value);
            if(!sem_value)
                break;
            pthread_cond_wait(&(resource_sync->cond_ongoing), &(resource_sync->mutex_resource));
        }
    pthread_mutex_unlock(&(resource_sync->mutex_resource));
}

void init_resource_sync(t_Drain_Ongoing_Resource_Sync *resource_sync) {
	pthread_mutex_init(&(resource_sync->mutex_resource), NULL);
	sem_init(&(resource_sync->sem_drain_requests_count), 0, 0);
	pthread_cond_init(&(resource_sync->cond_drain_requests), NULL);
	sem_init(&(resource_sync->sem_ongoing_count), 0, 0);
	pthread_cond_init(&(resource_sync->cond_ongoing), NULL);
}

void destroy_resource_sync(t_Drain_Ongoing_Resource_Sync *resource_sync) {
	pthread_mutex_destroy(&(resource_sync->mutex_resource));
	sem_destroy(&(resource_sync->sem_drain_requests_count));
	pthread_cond_destroy(&(resource_sync->cond_drain_requests));
	sem_destroy(&(resource_sync->sem_ongoing_count));
	pthread_cond_destroy(&(resource_sync->cond_ongoing));
}

void signal_ongoing(t_Drain_Ongoing_Resource_Sync *resource_sync) {
    sem_wait(&(resource_sync->sem_drain_requests_count));

    // Acá se podría agregar un if para hacer el broadcast sólo si el semáforo efectivamente quedó en 0
    pthread_cond_broadcast(&(resource_sync->cond_drain_requests));
}

void wait_draining_requests(t_Drain_Ongoing_Resource_Sync *resource_sync) {

    int sem_value;
    pthread_mutex_lock(&(resource_sync->mutex_resource));
		while(1) {
			sem_getvalue(&(resource_sync->sem_drain_requests_count), &sem_value);
			if(!sem_value)
				break;
			pthread_cond_wait(&(resource_sync->cond_drain_requests), &(resource_sync->mutex_resource));
		}
		sem_post(&(resource_sync->sem_ongoing_count));
    pthread_mutex_unlock(&(resource_sync->mutex_resource));
}

void signal_draining_requests(t_Drain_Ongoing_Resource_Sync *resource_sync) {
	sem_wait(&(resource_sync->sem_ongoing_count));
	// Acá se podría agregar un if para hacer el signal sólo si el semáforo efectivamente quedó en 0
	pthread_cond_signal(&(resource_sync->cond_ongoing)); // podría ser un broadcast en lugar de un wait si hay más de un comando de consola esperando
}