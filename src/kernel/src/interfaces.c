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

	t_Interface *new_interface = malloc(sizeof(t_Interface));
	if(new_interface == NULL) {
		log_error(SOCKET_LOGGER, "malloc: Error al reservar memoria para [Interfaz] Entrada/Salida");
		exit(1);
	}

    new_interface->client = new_client;

	send_header(INTERFACE_DATA_REQUEST_HEADER, new_client->fd_client);
	receive_interface_data(&(new_interface->name), &(new_interface->io_type), new_client->fd_client);

    new_interface->shared_list_blocked.list = list_create();
    pthread_mutex_init(&(new_interface->shared_list_blocked.mutex), NULL);

    sem_init(&(new_interface->sem_concurrency), 0, 1);

    wait_draining_requests(&SCHEDULING_SYNC);
        wait_draining_requests(&INTERFACES_SYNC);
            // FALTA CHEQUEAR QUE EL NOMBRE DE LA INTERFAZ NO EXISTA DE ANTEMANO
            list_add(LIST_INTERFACES, new_interface);
        signal_draining_requests(&INTERFACES_SYNC);
    signal_draining_requests(&SCHEDULING_SYNC);

    t_PID pid;
    t_Return_Value return_value;

    t_PCB *pcb;

	while(1) {

        if(receive_io_operation_finished(&pid, &return_value, new_interface->client->fd_client)) {
            wait_draining_requests(&SCHEDULING_SYNC);
                wait_draining_requests(&INTERFACES_SYNC);
                    //list_remove() POR NOMBRE
                signal_draining_requests(&INTERFACES_SYNC);
            signal_draining_requests(&SCHEDULING_SYNC);

            // MANEJAR CASO DE DESCONEXIÓN/ERROR
            pthread_mutex_lock(&(new_interface->shared_list_blocked.mutex));
                for(int i = 0; i < (new_interface->shared_list_blocked.list)->elements_count; i++) {
                    pcb = (t_PCB *) list_get(new_interface->shared_list_blocked.list, i);

                    if(pcb->exec_context.PID == pid) {
                        list_remove(new_interface->shared_list_blocked.list, i);
                        break;
                    }
                }
            pthread_mutex_unlock(&(new_interface->shared_list_blocked.mutex));
        }

        //pcb = find_pcb_by_pid(pid);

        // VER SI HAY UN NUEVO PCB EN LA LISTA DE BLOQUEADOS PARA DESPACHAR
	}

	return NULL;
}

void io_operation_dispatcher(t_PCB *pcb, t_Interface interface) {
    sem_trywait(&(interface.sem_concurrency));
    //sem_trywait(&(interface.sem_scheduler));

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