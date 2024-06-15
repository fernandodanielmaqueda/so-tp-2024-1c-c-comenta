/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Server COORDINATOR_IO;
t_Connection CONNECTION_MEMORY;
t_Connection CONNECTION_CPU_DISPATCH;
t_Connection CONNECTION_CPU_INTERRUPT;

sem_t CONNECTED_CPU_DISPATCH;

void initialize_sockets(void) {
	sem_init(&CONNECTED_CPU_DISPATCH, 0, 0);

	pthread_t thread_kernel_start_server_for_io;
	pthread_t thread_kernel_connect_to_memory;
	pthread_t thread_kernel_connect_to_cpu_dispatch;
	pthread_t thread_kernel_connect_to_cpu_interrupt;

	// [Server] Kernel <- [Cliente(s)] Entrada/Salida
	pthread_create(&thread_kernel_start_server_for_io, NULL, kernel_start_server_for_io, (void*) &COORDINATOR_IO);
	// [Client] Kernel -> [Server] Memoria
	pthread_create(&thread_kernel_connect_to_memory, NULL, client_thread_connect_to_server, (void*) &CONNECTION_MEMORY);
	// [Client] Kernel -> [Server] CPU (Dispatch Port)
	pthread_create(&thread_kernel_connect_to_cpu_dispatch, NULL, cpu_dispatch_handler, (void*) &CONNECTION_CPU_DISPATCH);
	// [Client] Kernel -> [Server] CPU (Interrupt Port)
	pthread_create(&thread_kernel_connect_to_cpu_interrupt, NULL, client_thread_connect_to_server, (void*) &CONNECTION_CPU_INTERRUPT);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_kernel_connect_to_memory, NULL);
	pthread_detach(thread_kernel_connect_to_cpu_dispatch);
	pthread_join(thread_kernel_connect_to_cpu_interrupt, NULL);

	sem_wait(&CONNECTED_CPU_DISPATCH);
}

void finish_sockets(void) {
	close(COORDINATOR_IO.fd_listen);
	close(CONNECTION_MEMORY.fd_connection);
	close(CONNECTION_CPU_DISPATCH.fd_connection);
	close(CONNECTION_CPU_INTERRUPT.fd_connection);
}

void *kernel_start_server_for_io(void *server_parameter) {
	t_Server *server = (t_Server *) server_parameter;

	int *fd_new_client;
	pthread_t thread_new_client;

	server_start(server);

	while(1) {
		fd_new_client = malloc(sizeof(int));
		log_trace(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		*fd_new_client = server_accept(server->fd_listen);

		if(*fd_new_client == -1) {
			log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			free(fd_new_client);
			continue;
		}

		log_trace(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		pthread_create(&thread_new_client, NULL, kernel_client_handler_for_io, (void*) fd_new_client);
		pthread_detach(thread_new_client);
	}

	return NULL;
}

void *kernel_client_handler_for_io(void *fd_new_client_parameter) {
	int *fd_new_client = (int *) fd_new_client_parameter;

	ssize_t bytes;
    t_Handshake handshake;

    bytes = recv(*fd_new_client, &handshake, sizeof(t_Handshake), MSG_WAITALL);

	if (bytes == 0) {
		log_warning(SOCKET_LOGGER, "Desconectado [Cliente] Entrada/Salida [%d]\n", *fd_new_client);
		close(*fd_new_client);
		return NULL;
	}
	if (bytes == -1) {
		log_warning(SOCKET_LOGGER, "Funcion recv: %s\n", strerror(errno));
		close(*fd_new_client);
		return NULL;
	}
	if (bytes != sizeof(t_Handshake)) {
		log_warning(SOCKET_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zd) con los que se esperaban recibir (%zd)\n", sizeof(t_Handshake), bytes);
		close(*fd_new_client);
		return NULL;
	}

    switch((e_PortType) handshake) {
        case IO_TYPE:
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
			handshake = 0;
            bytes = send(*fd_new_client, &handshake, sizeof(t_Handshake), 0);

			if (bytes == -1) {
				log_warning(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
				close(*fd_new_client);
				return NULL;
			}
			if (bytes != sizeof(t_Handshake)) {
				log_warning(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
				close(*fd_new_client);
				return NULL;
			}

            // Lógica de manejo de cliente Entrada/Salida
            close(*fd_new_client);
        	break;
        default:
            log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] No reconocido");
			handshake = -1;
            bytes = send(*fd_new_client, &handshake, sizeof(t_Handshake), 0);

			if (bytes == -1) {
				log_warning(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
			}
			if (bytes != sizeof(t_Handshake)) {
				log_warning(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
			}

            close(*fd_new_client);
			return NULL;
    }

	return NULL;
}