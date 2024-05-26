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

typedef struct {
    uint8_t codigo_operacion;
    t_buffer* buffer;
} t_paquete;

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
 * @brief Crear paquete.
 * @param codigoOperacion Codigo de operacion que tendra el paquete.
 */
t_paquete *create_package(uint8_t codigoOperacion);


/**
 * @brief Agregar dato a paquete
 * @param paquete t_paquete a rellenar.
 * @param valor Dato a agregar
 * @param tamanio Tamaño del dato a agregar.
 */
void add_to_package(t_paquete *paquete, void *valor, int tamanio);


/**
 * @brief Eliminar paquete
 * @param paquete t_paquete a eliminar.
 */
void kill_package(t_paquete *paquete);


/**
 * @brief Eliminar pcb.
 * @param pcbObjetivo t_pcb a eliminar.
 */
void kill_pcb(t_pcb *pcbObjetivo);



/**
 * @brief Eliminar instruccion.
 * @param lineaInstruccion t_instruccion a eliminar.
 */
void delete_instruction(t_instruccion *lineaInstruccion);


/**
 * @brief Serializacion del t_pcb para ser enviada.
 * @param paquete t_paquete a rellenar.
 * @param pcb Pcb a serializar
 */
void serialize_pcb(t_paquete *paquete, t_pcb *pcb);


/**
 * @brief Deserializacion del t_pcb para ser enviada.
 * @param socketClient Socket desde donde se va a recibir el pcb.
 */
t_pcb* deserialize_pcb(int socketClient);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param socketClient Socket desde donde se va a recibir el paquete.
 */
t_list* get_package_like_list(int socketClient);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param size Tamaño del buffer.
 * @param socketClient Socket desde donde se va a recibir el paquete.
 */
void *get_buffer(int *size, int socketCliente);


/**
 * @brief Crear Buffer
 * @param paquete t_paquete donde se creara el buffer
 */
void create_buffer(t_paquete *paquete);

#endif // SOCKET_H