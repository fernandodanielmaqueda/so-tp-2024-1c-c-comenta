/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Server SERVER_CPU_DISPATCH;
t_Server SERVER_CPU_INTERRUPT;
t_Connection CONNECTION_MEMORY;

sem_t CONNECTED_KERNEL_CPU_INTERRUPT;

void initialize_sockets(void) {
    sem_init(&CONNECTED_KERNEL_CPU_INTERRUPT, 0, 0);

    pthread_t thread_cpu_connect_to_memory;

    // [Server] CPU (Dispatch) <- [Cliente] Kernel
    pthread_create(&(SERVER_CPU_DISPATCH.thread_server), NULL, cpu_start_server_for_kernel, (void *) &SERVER_CPU_DISPATCH);
    // [Server] CPU (Interrupt) <- [Cliente] Kernel
    pthread_create(&(SERVER_CPU_INTERRUPT.thread_server), NULL, kernel_cpu_interrupt_handler, NULL);
    // [Client] CPU -> [Server] Memoria
    pthread_create(&thread_cpu_connect_to_memory, NULL, client_thread_connect_to_server, (void *) &CONNECTION_MEMORY);

    // Se bloquea hasta que se realicen todas las conexiones
    pthread_join(SERVER_CPU_DISPATCH.thread_server, NULL);
    pthread_detach(SERVER_CPU_INTERRUPT.thread_server);
    pthread_join(thread_cpu_connect_to_memory, NULL);

    sem_wait(&CONNECTED_KERNEL_CPU_INTERRUPT);
    sem_destroy(&CONNECTED_KERNEL_CPU_INTERRUPT);
}

void finish_sockets(void) {
    close(SERVER_CPU_DISPATCH.fd_listen);
    close(((t_Client *) list_get(SERVER_CPU_DISPATCH.clients, 0))->fd_client);

    close(SERVER_CPU_INTERRUPT.fd_listen);
    close(((t_Client *) list_get(SERVER_CPU_INTERRUPT.clients, 0))->fd_client);

    close(CONNECTION_MEMORY.fd_connection);
}

void *cpu_start_server_for_kernel(void *server_parameter) {
    t_Server *server = (t_Server *) server_parameter;

    e_Port_Type port_type;
    int fd_new_client;

    server_start(server);

    while(1) {
        while(1) {
            log_trace(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            fd_new_client = server_accept(server->fd_listen);

            if (fd_new_client != -1)
                break;

            log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
        }

        log_trace(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        receive_port_type(&port_type, fd_new_client);

        if (port_type == server->clients_type)
            break;

        log_warning(SOCKET_LOGGER, "Error de Handshake con [Cliente] No reconocido");
        send_port_type(TO_BE_IDENTIFIED_PORT_TYPE, fd_new_client);
        close(fd_new_client);
    }

    log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Kernel");
    send_port_type(server->server_type, fd_new_client);

    t_Client *new_client = malloc(sizeof(t_Client));
    if(new_client == NULL) {
        log_error(SOCKET_LOGGER, "Error de memoria al intentar aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
        exit(1);
    }

    new_client->fd_client = fd_new_client;
    new_client->client_type = server->clients_type;
    new_client->server = server;

    pthread_mutex_lock(&server->mutex_clients);
        list_add(server->clients, new_client);
    pthread_mutex_unlock(&server->mutex_clients);

    return NULL;
}