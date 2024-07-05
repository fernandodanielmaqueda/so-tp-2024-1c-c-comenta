
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Server COORDINATOR_MEMORY;
int FD_CLIENT_KERNEL;
int FD_CLIENT_CPU;

sem_t sem_coordinator_kernel_client_connected;
sem_t sem_coordinator_cpu_client_connected;

void initialize_sockets(void) {

	pthread_t thread_memory_start_server;

    sem_init(&sem_coordinator_kernel_client_connected, 0, 0);
    sem_init(&sem_coordinator_cpu_client_connected, 0, 0);

	// [Server] Memory <- [Cliente(s)] Entrada/Salida + Kernel + CPU
	pthread_create(&thread_memory_start_server, NULL, memory_start_server, (void*) &COORDINATOR_MEMORY);

	// Se bloquea hasta que se realicen todas las conexiones
    sem_wait(&sem_coordinator_kernel_client_connected);
    sem_destroy(&sem_coordinator_kernel_client_connected);

    sem_wait(&sem_coordinator_cpu_client_connected);    
    sem_destroy(&sem_coordinator_cpu_client_connected);
}

void finish_sockets(void) {
	close(COORDINATOR_MEMORY.fd_listen);
	close(FD_CLIENT_KERNEL);
	close(FD_CLIENT_CPU);
}

void *memory_start_server(void *server_parameter) {
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
		pthread_create(&thread_new_client, NULL, memory_client_handler, (void*) fd_new_client);
		pthread_detach(thread_new_client);
	}

	return NULL;
}

void *memory_client_handler(void *fd_new_client_parameter) {
	int *fd_new_client = (int *) fd_new_client_parameter;

    e_Port_Type port_type;

    receive_port_type(&port_type, *fd_new_client);

    switch(port_type) {
        case KERNEL_PORT_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UN KERNEL MAS DE UNA VEZ
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Kernel");
            send_port_type(MEMORY_PORT_TYPE, *fd_new_client);

            FD_CLIENT_KERNEL = *fd_new_client;
            sem_post(&sem_coordinator_kernel_client_connected);
            //listen_kernel(*fd_new_client);
            return NULL;
        case CPU_PORT_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UNA CPU MAS DE UNA VEZ
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] CPU");
            send_port_type(MEMORY_PORT_TYPE, *fd_new_client);

            FD_CLIENT_CPU = *fd_new_client;
            sem_post(&sem_coordinator_cpu_client_connected);
            listen_cpu(*fd_new_client);
            return NULL;
        case IO_PORT_TYPE:
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
            send_port_type(MEMORY_PORT_TYPE, *fd_new_client);

            listen_io(*fd_new_client);
            close(*fd_new_client);
            free(fd_new_client);
            return NULL;
        default:
            log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] No reconocido");
            send_port_type(TO_BE_IDENTIFIED_PORT_TYPE, *fd_new_client);

            close(*fd_new_client);
            free(fd_new_client);
            return NULL;
    }

    return NULL;
}