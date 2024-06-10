/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

t_Single_Client_Server SERVER_CPU_DISPATCH;
t_Single_Client_Server SERVER_CPU_INTERRUPT;
t_Connection CONNECTION_MEMORY;

void initialize_sockets(void)
{
    pthread_t thread_cpu_dispatch_start_server_for_kernel;
    pthread_t thread_cpu_interrupt_start_server_for_kernel;
    pthread_t thread_cpu_connect_to_memory;

    // [Server] CPU (Dispatch) <- [Cliente] Kernel
    pthread_create(&thread_cpu_dispatch_start_server_for_kernel, NULL, cpu_start_server_for_kernel, (void *)&SERVER_CPU_DISPATCH);
    // [Server] CPU (Interrupt) <- [Cliente] Kernel
    pthread_create(&thread_cpu_interrupt_start_server_for_kernel, NULL, cpu_start_server_for_kernel, (void *)&SERVER_CPU_INTERRUPT);
    // [Client] CPU -> [Server] Memoria
    pthread_create(&thread_cpu_connect_to_memory, NULL, client_thread_connect_to_server, (void *)&CONNECTION_MEMORY);

    // Se bloquea hasta que se realicen todas las conexiones
    pthread_join(thread_cpu_dispatch_start_server_for_kernel, NULL);
    pthread_join(thread_cpu_interrupt_start_server_for_kernel, NULL);
    pthread_join(thread_cpu_connect_to_memory, NULL);
}

void finish_sockets(void)
{
    close(SERVER_CPU_DISPATCH.server.fd_listen);
    close(SERVER_CPU_DISPATCH.client.fd_client);

    close(SERVER_CPU_INTERRUPT.server.fd_listen);
    close(SERVER_CPU_INTERRUPT.client.fd_client);

    close(CONNECTION_MEMORY.fd_connection);
}

void *cpu_start_server_for_kernel(void *single_client_server_parameter)
{
    t_Single_Client_Server *single_client_server = (t_Single_Client_Server *) single_client_server_parameter;
    t_Server *server = &(single_client_server->server);
    t_Client *client = &(single_client_server->client);

    ssize_t bytes;

    t_Handshake handshake;

    server_start(server);

    while (1) {
        while (1) {
            log_trace(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            client->fd_client = server_accept(server->fd_listen);

            if (client->fd_client != -1)
                break;
            else
            {
                log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
                continue;
            }
        }

        log_trace(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(client->fd_client, &handshake, sizeof(t_Handshake), MSG_WAITALL);

        if (bytes == 0) {
            log_error(SOCKET_LOGGER, "Desconectado [Cliente] %s en Puerto: %s\n", PORT_NAMES[server->clients_type], server->port);
            close(client->fd_client);
            continue;
        }
        if (bytes == -1) {
            log_error(SOCKET_LOGGER, "Funcion recv: %s\n", strerror(errno));
            close(client->fd_client);
            continue;
        }
        if (bytes != sizeof(t_Handshake)) {
            log_error(SOCKET_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zd) con los que se esperaban recibir (%zd)\n", sizeof(t_Handshake), bytes);
            close(client->fd_client);
            continue;
        }

        if ((e_PortType)handshake == server->clients_type)
            break;
        else
        {
            log_warning(SOCKET_LOGGER, "Error de Handshake con [Cliente] No reconocido");
            handshake = -1;
            bytes = send(client->fd_client, &handshake, sizeof(t_Handshake), 0);
            close(client->fd_client);

            if (bytes == -1) {
                log_error(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
                continue;
            }
            if (bytes != sizeof(t_Handshake)) {
                log_error(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", sizeof(t_Handshake), bytes);
                continue;
            }
        }
    }

    log_debug(SOCKET_LOGGER, "OK Handshake con [Cliente] Kernel");
    handshake = 0;
    bytes = send(client->fd_client, &handshake, sizeof(t_Handshake), 0);

    return NULL;
}