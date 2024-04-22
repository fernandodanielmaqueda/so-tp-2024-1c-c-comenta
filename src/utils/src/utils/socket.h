#ifndef SOCKET_H
#define SOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <estructuras.h>

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
int start_server(char* ip, char* port);
int start_client(char* ip, char* port);
int esperar_cliente(int socket_servidor);
void liberar_conexion(int socket_cliente);

/**
 * @brief Agregar dato a paquete
 * @param paquete t_paquete a rellenar.
 * @param valor Dato a agregar
 * @param tamanio Tamaño del dato a agregar.
 */
void add_to_package(t_paquete *paquete, void *valor, int tamanio);

/**
 * @brief Serializacion del t_pcb para ser enviada.
 * @param paquete t_paquete a rellenar.
 * @param pcb Pcb a serializar
 */
void serialize_pcb(t_paquete *paquete, t_pcb *pcb);




#endif // SOCKET_H