/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Server COORDINATOR_IO;
t_Connection CONNECTION_MEMORY;
t_Connection CONNECTION_CPU_DISPATCH;
t_Connection CONNECTION_CPU_INTERRUPT;

void initialize_sockets(void) {

	pthread_t thread_kernel_start_server_for_io;
	pthread_t thread_kernel_connect_to_memory;
	pthread_t thread_kernel_connect_to_cpu_dispatch;
	pthread_t thread_kernel_connect_to_cpu_interrupt;

	// [Server] Kernel <- [Cliente(s)] Entrada/Salida
	pthread_create(&thread_kernel_start_server_for_io, NULL, kernel_start_server_for_io, (void*) &COORDINATOR_IO);
	// [Client] Kernel -> [Server] Memoria
	pthread_create(&thread_kernel_connect_to_memory, NULL, client_thread_connect_to_server, (void*) &CONNECTION_MEMORY);
	// [Client] Kernel -> [Server] CPU (Dispatch Port)
	pthread_create(&thread_kernel_connect_to_cpu_dispatch, NULL, client_thread_connect_to_server, (void*) &CONNECTION_CPU_DISPATCH);
	// [Client] Kernel -> [Server] CPU (Interrupt Port)
	pthread_create(&thread_kernel_connect_to_cpu_interrupt, NULL, client_thread_connect_to_server, (void*) &CONNECTION_CPU_INTERRUPT);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_kernel_connect_to_memory, NULL);
	pthread_join(thread_kernel_connect_to_cpu_dispatch, NULL);
	pthread_join(thread_kernel_connect_to_cpu_interrupt, NULL);
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

	e_Port_Type port_type;

	receive_port_type(&port_type, *fd_new_client);

	if(port_type == IO_PORT_TYPE) {
		log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
		send_port_type(KERNEL_PORT_TYPE, *fd_new_client);

		// Lógica de manejo de cliente Entrada/Salida
	} else {
		log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] No reconocido");
		send_port_type(TO_BE_IDENTIFIED_PORT_TYPE, *fd_new_client);
	}

	close(*fd_new_client);
	return NULL;
}