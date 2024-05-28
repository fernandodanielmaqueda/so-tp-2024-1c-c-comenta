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
#include "modules.h"

#define RETRY_CONNECTION_IN_SECONDS 10
#define MAX_CONNECTION_ATTEMPS 10

#ifndef DEBUG_SERIALIZATION
#define DEBUG_SERIALIZATION 0
#endif

#define DEBUGGING_SERIALIZATION if(DEBUG_SERIALIZATION)

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

typedef struct {
    uint32_t size;
    void* stream;
} t_buffer;

typedef struct t_package {
    uint8_t codigo_operacion;
    t_buffer* buffer;
} t_package;

/*
int start_server_module(char* module, char * pathconfig);
int start_client_module(char* module, char* pathconfig);
void get_ip_port_from_module(const char* module, char* path_config,char* ip, char* port);
*/

void *client_thread_connect_to_server(void *connection_parameter);
int client_start_try(char* ip, char* port);
void server_start(Server *server);
int server_start_try(char* port);
int server_accept(int socket_servidor);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param socket t_package donde se creara el buffer
 */
t_opcode get_opCode(int fd_socket);


/**
 * @brief Crear paquete.
 * @param opCode Codigo de operacion que tendra el paquete.
 */
t_package *package_create(uint8_t opCode);


/**
 * @brief Agregar dato a paquete
 * @param package t_package a rellenar.
 * @param value Dato a agregar
 * @param size Tamaño del dato a agregar.
 */
void package_add(t_package *package, void *value, int size);


/**
 * @brief Eliminar paquete
 * @param paquete t_package a eliminar.
 */
void package_kill(t_package *package);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param size Tamaño del buffer.
 * @param fd_client Socket desde donde se va a recibir el paquete.
 */
void *buffer_get(int *size, int fd_client);


/**
 * @brief Crear Buffer
 * @param package t_package donde se creara el buffer
 */
void buffer_create(t_package *package);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param fd_client Socket desde donde se va a recibir el paquete.
 */
t_list* get_package_like_list(int fd_client);

/**
 * @brief Eliminar pcb.
 * @param pcb_target t_pcb a eliminar.
 */
//void kill_pcb(t_pcb *pcb_target);


/**
 * @brief Eliminar instruccion.
 * @param lineInstruccion t_instruccion a eliminar.
 */
//void instruction_delete(t_instruccion *lineInstruccion);


/**
 * @brief Serializacion del t_pcb para ser enviada.
 * @param package t_package a rellenar.
 * @param pcb Pcb a serializar
 */
void serialize_pcb(t_package *package, t_pcb *pcb);


/**
 * @brief Deserializacion del t_pcb para ser enviada.
 * @param socketClient Socket desde donde se va a recibir el pcb.
 */
t_pcb* deserialize_pcb(int socketClient);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param codigoOperacion Codigo de operacion a enviar
 * @param mensaje Mensaje a enviar
 * @param socket Socket destino
 */
void send_message(t_opcode codigoOperacion, char* mensaje, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a enviar
 * @param socket Socket destino
 */
void package_send(t_package* package, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a serializar
 * @param bytes Tamanio del paquete
 */
void* package_serialize(t_package* package, int bytes);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param instruccion Instruccion a enviar
 * @param socket Socket destino
 */
void send_instruccion(t_instruction_use* instruccion, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param socket Socket a recibir
 */
t_instruction_use* receive_instruccion(int socket);

void serialize_pcb_2(t_package* paquete, t_pcb* pcb);
void free_package(t_package* paquete);
void send_pcb(int socket, t_pcb* pcb);
void deserialize_pcb_2(t_pcb* pcb, void* stream);
void receive_pcb(int socket, t_pcb *pcb);

#endif // SOCKET_H