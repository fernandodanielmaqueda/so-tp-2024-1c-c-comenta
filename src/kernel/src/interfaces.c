/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "interfaces.h"

t_list *LIST_INTERFACES;
t_Drain_Ongoing_Resource_Sync INTERFACES_SYNC;

void *kernel_client_handler_for_io(void *new_client_parameter) {
	t_Client *new_client = (t_Client *) new_client_parameter;

	e_Port_Type port_type;

	receive_port_type(&port_type, new_client->fd_client);

	if(port_type != IO_PORT_TYPE) {
		log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] No reconocido");
		send_port_type(TO_BE_IDENTIFIED_PORT_TYPE, new_client->fd_client);
		close(new_client->fd_client);
		return NULL;
	}

	log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
	send_port_type(KERNEL_PORT_TYPE, new_client->fd_client);

	t_Interface interface;
    interface_init(&interface, new_client);

    wait_draining_requests(&SCHEDULING_SYNC);
        wait_ongoing_locking(&INTERFACES_SYNC);

            if(list_add_unless_matches_with_any(LIST_INTERFACES, &interface, (bool (*)(void *, void *)) interface_names_match)){
                log_warning(MODULE_LOGGER, "Error al agregar [Interfaz] Entrada/Salida: Nombre de interfaz ya existente");
                send_return_value_with_header(INTERFACE_DATA_REQUEST_HEADER, 1, new_client->fd_client);
                interface_destroy(&interface);
                return NULL;
            }

            send_return_value_with_header(INTERFACE_DATA_REQUEST_HEADER, 0, interface.client->fd_client);

        signal_ongoing_unlocking(&INTERFACES_SYNC);
    signal_draining_requests(&SCHEDULING_SYNC);

    t_PID pid;
    t_Return_Value return_value;

    t_PCB *pcb;

	while(1) {

        if(receive_io_operation_finished(&pid, &return_value, interface.client->fd_client)) {
            // Caso de desconexión/error de comunicación
            wait_draining_requests(&SCHEDULING_SYNC);

                wait_ongoing_locking(&INTERFACES_SYNC);
                    list_remove_by_condition_with_comparation(LIST_INTERFACES, (bool (*)(void *, void *)) interface_names_match, &(interface));
                signal_ongoing_unlocking(&INTERFACES_SYNC);

                interface_exit(&interface);
                interface_destroy(&interface);
                
            signal_draining_requests(&SCHEDULING_SYNC);
        }

        wait_draining_requests(&SCHEDULING_SYNC);
            wait_draining_requests(&INTERFACES_SYNC);
                pthread_mutex_lock(&(interface.shared_list_blocked.mutex));

                    pcb = (t_PCB *) list_remove_by_condition_with_comparation(interface.shared_list_blocked.list, (bool (*)(void *, void *)) pcb_matches_pid, &(pid));
                    if(pcb != NULL) {
                        payload_destroy(pcb->instruction);
                        if(return_value) {
                            pcb->exit_reason = UNEXPECTED_ERROR_EXIT_REASON;
                            switch_process_state(pcb, EXIT_STATE);
                        } else {
                            switch_process_state(pcb, READY_STATE);
                        }
                    }


                    if((interface.shared_list_blocked.list)->head == NULL) {
                        sem_post(&(interface.sem_concurrency));
                        pthread_mutex_unlock(&(interface.shared_list_blocked.mutex));
                        signal_draining_requests(&INTERFACES_SYNC);
                        signal_draining_requests(&SCHEDULING_SYNC);
                        continue;
                    }

                    pcb = (t_PCB *) list_get(interface.shared_list_blocked.list, 0);

                pthread_mutex_unlock(&(interface.shared_list_blocked.mutex));
            signal_draining_requests(&INTERFACES_SYNC);
        signal_draining_requests(&SCHEDULING_SYNC);
	}

	return NULL;
}

void interface_init(t_Interface *interface, t_Client *client) {
    interface->client = client;

	send_header(INTERFACE_DATA_REQUEST_HEADER, interface->client->fd_client);
	receive_interface_data(&(interface->name), &(interface->io_type), interface->client->fd_client);

    interface->shared_list_blocked.list = list_create();
    pthread_mutex_init(&(interface->shared_list_blocked.mutex), NULL);

    sem_init(&(interface->sem_concurrency), 0, 1);
}

void interface_exit(t_Interface *interface) {
    t_PCB *pcb;

    pthread_mutex_lock(&(interface->shared_list_blocked.mutex));
        while((interface->shared_list_blocked.list)->elements_count > 0) {
            pcb = (t_PCB *) list_remove(interface->shared_list_blocked.list, 0);
            payload_destroy(pcb->instruction);
            pcb->exit_reason = INVALID_INTERFACE_EXIT_REASON;
            switch_process_state(pcb, EXIT_STATE);
        }
    pthread_mutex_unlock(&(interface->shared_list_blocked.mutex));
}

void interface_destroy(t_Interface *interface) {
    close(interface->client->fd_client);
    free(interface->client);
    free(interface->name);
    list_destroy_and_destroy_elements(interface->shared_list_blocked.list, free);
    pthread_mutex_destroy(&(interface->shared_list_blocked.mutex));
    sem_destroy(&(interface->sem_concurrency));
}

bool interface_name_matches(t_Interface *interface, char *name) {
	return (strcmp(interface->name, name) == 0);
}

bool interface_names_match(t_Interface *interface_1, t_Interface *interface_2) {
	return (strcmp(interface_1->name, interface_2->name) == 0);
}

void io_operation_dispatcher(t_PCB *pcb, t_Interface interface) {
    sem_trywait(&(interface.sem_concurrency));

    wait_draining_requests(&SCHEDULING_SYNC);
        pthread_mutex_lock(&(interface.shared_list_blocked.mutex));

            if((interface.shared_list_blocked.list)->head == NULL) {
                pthread_mutex_unlock(&(interface.shared_list_blocked.mutex));
                signal_draining_requests(&SCHEDULING_SYNC);
                return;
            }

            //t_PCB *pcb = (t_PCB *) (interface.shared_list_blocked.list)->head->data;

        pthread_mutex_unlock(&(interface.shared_list_blocked.mutex));

        send_io_operation_dispatch(pcb->exec_context.PID, *(pcb->instruction), interface.client->fd_client);
    signal_draining_requests(&SCHEDULING_SYNC);
}