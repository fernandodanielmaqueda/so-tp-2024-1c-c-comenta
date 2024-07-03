/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Single_Client_Server SERVER_CPU_DISPATCH;
t_Single_Client_Server SERVER_CPU_INTERRUPT;
t_Connection CONNECTION_MEMORY;

sem_t CONNECTED_KERNEL_CPU_INTERRUPT;

void initialize_sockets(void) {
    sem_init(&CONNECTED_KERNEL_CPU_INTERRUPT, 0, 0);

    pthread_t thread_cpu_dispatch_start_server_for_kernel;
    pthread_t thread_cpu_interrupt_start_server_for_kernel;
    pthread_t thread_cpu_connect_to_memory;

    // [Server] CPU (Dispatch) <- [Cliente] Kernel
    pthread_create(&thread_cpu_dispatch_start_server_for_kernel, NULL, cpu_start_server_for_kernel, (void *)&SERVER_CPU_DISPATCH);
    // [Server] CPU (Interrupt) <- [Cliente] Kernel
    pthread_create(&thread_cpu_interrupt_start_server_for_kernel, NULL, kernel_cpu_interrupt_handler, NULL);
    // [Client] CPU -> [Server] Memoria
    pthread_create(&thread_cpu_connect_to_memory, NULL, client_thread_connect_to_server, (void *)&CONNECTION_MEMORY);

    // Se bloquea hasta que se realicen todas las conexiones
    pthread_join(thread_cpu_dispatch_start_server_for_kernel, NULL);
    pthread_detach(thread_cpu_interrupt_start_server_for_kernel);
    pthread_join(thread_cpu_connect_to_memory, NULL);

    sem_wait(&CONNECTED_KERNEL_CPU_INTERRUPT);
    sem_destroy(&CONNECTED_KERNEL_CPU_INTERRUPT);
}

void finish_sockets(void) {
    close(SERVER_CPU_DISPATCH.server.fd_listen);
    close(SERVER_CPU_DISPATCH.client.fd_client);

    close(SERVER_CPU_INTERRUPT.server.fd_listen);
    close(SERVER_CPU_INTERRUPT.client.fd_client);

    close(CONNECTION_MEMORY.fd_connection);
}

void *cpu_start_server_for_kernel(void *single_client_server_parameter) {
    t_Single_Client_Server *single_client_server = (t_Single_Client_Server *) single_client_server_parameter;
    t_Server *server = &(single_client_server->server);
    t_Client *client = &(single_client_server->client);

    e_PortType port_type;

    server_start(server);

    while (1) {
        while (1) {
            log_trace(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            client->fd_client = server_accept(server->fd_listen);

            if (client->fd_client != -1)
                break;
            
            log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
        }

        log_trace(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        receive_port_type(&port_type, client->fd_client);

        if (port_type == server->clients_type)
            break;

        log_warning(SOCKET_LOGGER, "Error de Handshake con [Cliente] No reconocido");
        send_port_type(TO_BE_IDENTIFIED_PORT_TYPE, client->fd_client);
        close(client->fd_client);
    }

    log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Kernel");
    send_port_type(server->server_type, client->fd_client);

    return NULL;
}