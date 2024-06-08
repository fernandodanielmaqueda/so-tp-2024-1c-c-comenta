
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
    sem_wait(&sem_coordinator_cpu_client_connected);
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
		log_info(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		*fd_new_client = server_accept(server->fd_listen);

		if(*fd_new_client == -1) {
			log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			free(fd_new_client);
			continue;
		}

		log_info(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		pthread_create(&thread_new_client, NULL, memory_client_handler, (void*) fd_new_client);
		pthread_detach(thread_new_client);
	}

	return NULL;
}

void *memory_client_handler(void *fd_new_client_parameter) {
	int *fd_new_client = (int *) fd_new_client_parameter;

    ssize_t bytes;
    t_Handshake handshake;

    bytes = recv(*fd_new_client, &handshake, sizeof(t_Handshake), MSG_WAITALL);

    if (bytes == 0) {
        log_warning(SOCKET_LOGGER, "Desconectado [Cliente] No reconocido\n");
        close(*fd_new_client);
        free(fd_new_client);
        return NULL;
    }
    if (bytes == -1) {
        log_warning(SOCKET_LOGGER, "Funcion recv: %s\n", strerror(errno));
        close(*fd_new_client);
        free(fd_new_client);
        return NULL;
    }
    if (bytes != sizeof(t_Handshake)) {
        log_warning(SOCKET_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zd) con los que se esperaban recibir (%zd)\n", sizeof(t_Handshake), bytes);
        close(*fd_new_client);
        free(fd_new_client);
        return NULL;
    }

    switch((e_PortType) handshake) {
        case KERNEL_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UN KERNEL MAS DE UNA VEZ
            log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] Kernel");
            handshake = 0;
            bytes = send(*fd_new_client, &handshake, sizeof(t_Handshake), 0);

            if (bytes == -1) {
                log_warning(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }
            if (bytes != sizeof(t_Handshake)) {
                log_warning(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }

            FD_CLIENT_KERNEL = *fd_new_client;
            sem_post(&sem_coordinator_kernel_client_connected);
            // Lógica de manejo de cliente Kernel (crear un hilo para menejo de cliente Kernel)
            return NULL;
        case CPU_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UNA CPU MAS DE UNA VEZ
            log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] CPU");
            handshake = 0;
            bytes = send(*fd_new_client, &handshake, sizeof(t_Handshake), 0);

            if (bytes == -1) {
                log_warning(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }
            if (bytes != sizeof(t_Handshake)) {
                log_warning(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }

            FD_CLIENT_CPU = *fd_new_client;
            sem_post(&sem_coordinator_cpu_client_connected);
            // Lógica de manejo de cliente CPU (crear un hilo para menejo de cliente CPU)
            return NULL;
        case IO_TYPE:
            log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
            handshake = 0;
            bytes = send(*fd_new_client, &handshake, sizeof(t_Handshake), 0);

            if (bytes == -1) {
                log_warning(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }
            if (bytes != sizeof(t_Handshake)) {
                log_warning(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }

            // Lógica de manejo de cliente Entrada/Salida (crear un hilo para menejo de cliente Entrada/Salida)
            close(*fd_new_client);
            free(fd_new_client);
            return NULL;
        default:
            log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] No reconocido");
            handshake = -1;
            bytes = send(*fd_new_client, &handshake, sizeof(t_Handshake), 0);

            if (bytes == -1) {
                log_warning(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }
            if (bytes != sizeof(t_Handshake)) {
                log_warning(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
                close(*fd_new_client);
                free(fd_new_client);
                return NULL;
            }

            close(*fd_new_client);
            free(fd_new_client);
            return NULL;
    }

    return NULL;
}