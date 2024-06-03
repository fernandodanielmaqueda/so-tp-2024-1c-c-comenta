/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "estructuras.h"
#include "module.h"

#define RETRY_CONNECTION_IN_SECONDS 10
#define MAX_CONNECTION_ATTEMPS 10

extern const char *PORT_NAMES[PortType_Count];
//extern const int32_t HANDSHAKES[PortType_Count];

typedef struct Connection {
    int fd_connection;
    enum PortType client_type;
    enum PortType server_type;
    char *ip;
    char *port;
} Connection;

typedef struct Server {
    int fd_listen;
    enum PortType server_type;
    enum PortType clients_type;
    char *port;
} Server;

void *client_thread_connect_to_server(void *connection_parameter);
int client_start_try(char* ip, char* port);
void server_start(Server *server);
int server_start_try(char* port);
int server_accept(int socket_servidor);

#endif // SOCKET_H