/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "interfaces.h"

t_list *LIST_INTERFACES;
t_Drain_Ongoing_Resource_Sync INTERFACES_SYNC;

void *kernel_client_handler_for_io(t_Client *new_client) {

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

	t_Interface *interface = interface_create(new_client);

    wait_draining_requests(&SCHEDULING_SYNC);
        wait_ongoing_locking(&INTERFACES_SYNC);

            if(list_add_unless_any(LIST_INTERFACES, interface, (bool (*)(void *, void *)) interface_name_matches, interface->name)) {
                log_warning(MODULE_LOGGER, "Error al agregar [Interfaz] Entrada/Salida: Nombre de interfaz ya existente");
                send_return_value_with_header(INTERFACE_DATA_REQUEST_HEADER, 1, new_client->fd_client);
                interface_destroy(interface);
                return NULL;
            }

            send_return_value_with_header(INTERFACE_DATA_REQUEST_HEADER, 0, interface->client->fd_client);

        signal_ongoing_unlocking(&INTERFACES_SYNC);
    signal_draining_requests(&SCHEDULING_SYNC);
	
	kernel_io_interface_listener(interface);

	return NULL;
}

void *kernel_io_interface_dispatcher(t_Interface *interface) {

    t_PCB *pcb;

    while(1) {
        sem_wait(&(interface->sem_scheduler));
            pthread_cleanup_push((void (*)(void *)) sem_post, (void *) &(interface->sem_scheduler));
        sem_wait(&(interface->sem_concurrency));
            pthread_cleanup_push((void (*)(void *)) sem_post, (void *) &(interface->sem_concurrency));

        wait_draining_requests(&SCHEDULING_SYNC);
            pthread_cleanup_push((void (*)(void *)) signal_draining_requests, (void *) &SCHEDULING_SYNC);
            wait_draining_requests(&INTERFACES_SYNC);
                pthread_cleanup_push((void (*)(void *)) signal_draining_requests, (void *) &INTERFACES_SYNC);

                pthread_mutex_lock(&(interface->shared_list_blocked_ready.mutex));
                    pthread_cleanup_push((void (*)(void *)) pthread_mutex_unlock, (void *) &(interface->shared_list_blocked_ready.mutex));
                    pcb = (t_PCB *) list_remove(interface->shared_list_blocked_ready.list, 0);
                pthread_cleanup_pop(1);

                pthread_mutex_lock(&(interface->shared_list_blocked_exec.mutex));
                    pthread_cleanup_push((void (*)(void *)) pthread_mutex_unlock, (void *) &(interface->shared_list_blocked_exec.mutex));
                    list_add(interface->shared_list_blocked_exec.list, pcb);
                pthread_cleanup_pop(1);

                pthread_testcancel();

                if(send_io_operation_dispatch(pcb->exec_context.PID, pcb->io_operation, interface->client->fd_client)) {
                    log_warning(MODULE_LOGGER, "Error al enviar operacion de [Interfaz] Entrada/Salida");
                    pthread_exit(NULL);
                }

            pthread_cleanup_pop(1);
        pthread_cleanup_pop(1);
        
        pthread_cleanup_pop(1);
        pthread_cleanup_pop(1);
    }

    return NULL;
}

void kernel_io_interface_listener(t_Interface *interface) {

    t_PID pid;
    t_Return_Value return_value;

    t_PCB *pcb;

	while(1) {

        if(receive_io_operation_finished(&pid, &return_value, interface->client->fd_client)) {
            pthread_cancel(interface->thread_io_interface_dispatcher);
            pthread_join(interface->thread_io_interface_dispatcher, NULL);

            wait_draining_requests(&SCHEDULING_SYNC);

                wait_ongoing_locking(&INTERFACES_SYNC);
                    list_remove_by_condition_with_comparation(LIST_INTERFACES, (bool (*)(void *, void *)) interface_name_matches, interface->name);
                signal_ongoing_unlocking(&INTERFACES_SYNC);

                interface_exit(interface);
                interface_destroy(interface);
                
            signal_draining_requests(&SCHEDULING_SYNC);
			return;
        }

        wait_draining_requests(&SCHEDULING_SYNC);
            wait_draining_requests(&INTERFACES_SYNC);
                pthread_mutex_lock(&(interface->shared_list_blocked_exec.mutex));

                    pcb = (t_PCB *) list_remove_by_condition_with_comparation(interface->shared_list_blocked_exec.list, (bool (*)(void *, void *)) pcb_matches_pid, &(pid));
                    if(pcb != NULL) {
                        payload_destroy(&(pcb->io_operation));
                        if(return_value) {
                            pcb->exit_reason = UNEXPECTED_ERROR_EXIT_REASON;
                            switch_process_state(pcb, EXIT_STATE);
                        }
                        else {
                            switch_process_state(pcb, READY_STATE);
                        }
                    }

                    sem_post(&(interface->sem_concurrency));

                pthread_mutex_unlock(&(interface->shared_list_blocked_exec.mutex));
            signal_draining_requests(&INTERFACES_SYNC);
        signal_draining_requests(&SCHEDULING_SYNC);
	}

}

t_Interface *interface_create(t_Client *client) {
	
    t_Interface *interface = malloc(sizeof(t_Interface));
    if(interface == NULL) {
        log_error(MODULE_LOGGER, "malloc: Error al reservar memoria para [Interfaz] Entrada/Salida");
        return NULL;
    }
	
    interface->client = client;

	send_header(INTERFACE_DATA_REQUEST_HEADER, interface->client->fd_client);
	receive_interface_data(&(interface->name), &(interface->io_type), interface->client->fd_client);

    interface->shared_list_blocked_ready.list = list_create();
    pthread_mutex_init(&(interface->shared_list_blocked_ready.mutex), NULL);

    sem_init(&(interface->sem_scheduler), 0, 0);
    sem_init(&(interface->sem_concurrency), 0, 1);

    pthread_create(&(interface->thread_io_interface_dispatcher), NULL, (void *(*)(void *)) kernel_io_interface_dispatcher, (void *) interface);

    return interface;
}

void interface_exit(t_Interface *interface) {
    t_PCB *pcb;

    pthread_mutex_lock(&(interface->shared_list_blocked_ready.mutex));
        while((interface->shared_list_blocked_ready.list)->elements_count > 0) {
            pcb = (t_PCB *) list_remove(interface->shared_list_blocked_ready.list, 0);
            payload_destroy(&(pcb->io_operation));
            pcb->exit_reason = INVALID_INTERFACE_EXIT_REASON;
            switch_process_state(pcb, EXIT_STATE);
        }
    pthread_mutex_unlock(&(interface->shared_list_blocked_ready.mutex));
}

void interface_destroy(t_Interface *interface) {
    close(interface->client->fd_client);
    free(interface->client);

    free(interface->name);

    list_destroy_and_destroy_elements(interface->shared_list_blocked_ready.list, (void (*)(void *)) pcb_free);

    pthread_mutex_destroy(&(interface->shared_list_blocked_ready.mutex));
    pthread_mutex_destroy(&(interface->shared_list_blocked_ready.mutex));

    sem_destroy(&(interface->sem_scheduler));
    sem_destroy(&(interface->sem_concurrency));

	free(interface);
}

bool interface_name_matches(t_Interface *interface, char *name) {
	return (strcmp(interface->name, name) == 0);
}