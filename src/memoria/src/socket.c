
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Server COORDINATOR_MEMORY;
t_Client *CLIENT_KERNEL;
t_Client *CLIENT_CPU;

sem_t sem_coordinator_kernel_client_connected;
sem_t sem_coordinator_cpu_client_connected;

void initialize_sockets(void) {

    sem_init(&sem_coordinator_kernel_client_connected, 0, 0);
    sem_init(&sem_coordinator_cpu_client_connected, 0, 0);

	// [Server] Memory <- [Cliente(s)] Entrada/Salida + Kernel + CPU
	pthread_create(&COORDINATOR_MEMORY.thread_server, NULL, memory_start_server, (void *) &COORDINATOR_MEMORY);

	// Se bloquea hasta que se realicen todas las conexiones
    sem_wait(&sem_coordinator_kernel_client_connected);
    sem_destroy(&sem_coordinator_kernel_client_connected);

    sem_wait(&sem_coordinator_cpu_client_connected);    
    sem_destroy(&sem_coordinator_cpu_client_connected);
}

void finish_sockets(void) {
	close(COORDINATOR_MEMORY.fd_listen);
    for(int i = 0; i < list_size(COORDINATOR_MEMORY.shared_list_clients.list); i++)
        close(((t_Client *) list_get(COORDINATOR_MEMORY.shared_list_clients.list, i))->fd_client);
}

void *memory_start_server(void *server_parameter) {
	t_Server *server = (t_Server *) server_parameter;

	int fd_new_client;
	t_Client *new_client;

	server_start(server);

	while(1) {
		log_trace(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		fd_new_client = server_accept(server->fd_listen);

		if(fd_new_client == -1) {
			log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			continue;
		}

        new_client = malloc(sizeof(t_Client));
		if(new_client == NULL) {
			log_error(SOCKET_LOGGER, "Error al reservar memoria para [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			exit(1);
		}

		log_trace(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		new_client->fd_client = fd_new_client;
        new_client->client_type = server->clients_type;
        new_client->server = server;
		pthread_create(&(new_client->thread_client_handler), NULL, memory_client_handler, (void *) new_client);
		pthread_detach(new_client->thread_client_handler);
	}

	return NULL;
}

void *memory_client_handler(void *new_client_parameter) {
	t_Client *new_client = (t_Client *) new_client_parameter;

    e_Port_Type port_type;

    receive_port_type(&port_type, new_client->fd_client);

    switch(port_type) {
        case KERNEL_PORT_TYPE:
            new_client->client_type = KERNEL_PORT_TYPE;
            // REVISAR QUE NO SE PUEDA CONECTAR UN KERNEL MAS DE UNA VEZ
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Kernel");
            send_port_type(MEMORY_PORT_TYPE, new_client->fd_client);

            pthread_mutex_lock(&(new_client->server->shared_list_clients.mutex));
                list_add(new_client->server->shared_list_clients.list, new_client);
            pthread_mutex_unlock(&(new_client->server->shared_list_clients.mutex));

            CLIENT_KERNEL = new_client;

            sem_post(&sem_coordinator_kernel_client_connected);
            return NULL;
        case CPU_PORT_TYPE:
            new_client->client_type = CPU_PORT_TYPE;
            // REVISAR QUE NO SE PUEDA CONECTAR UNA CPU MAS DE UNA VEZ
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] CPU");
            send_port_type(MEMORY_PORT_TYPE, new_client->fd_client);

            pthread_mutex_lock(&(new_client->server->shared_list_clients.mutex));
                list_add(new_client->server->shared_list_clients.list, new_client);
            pthread_mutex_unlock(&(new_client->server->shared_list_clients.mutex));

            CLIENT_CPU = new_client;

            sem_post(&sem_coordinator_cpu_client_connected);
            listen_cpu(new_client->fd_client);
            return NULL;
        case IO_PORT_TYPE:
            new_client->client_type = IO_PORT_TYPE;
            log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Entrada/Salida");
            send_port_type(MEMORY_PORT_TYPE, new_client->fd_client);

            pthread_mutex_lock(&(new_client->server->shared_list_clients.mutex));
                list_add(new_client->server->shared_list_clients.list, new_client);
            pthread_mutex_unlock(&(new_client->server->shared_list_clients.mutex));

            listen_io(new_client->fd_client);

            close(new_client->fd_client);
            free(new_client);
            return NULL;
        default:
            log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] No reconocido");
            send_port_type(TO_BE_IDENTIFIED_PORT_TYPE, new_client->fd_client);

            close(new_client->fd_client);
            free(new_client);
            return NULL;
    }

    return NULL;
}