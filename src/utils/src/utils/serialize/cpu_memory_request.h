/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_CPU_MEMORY_REQUEST_H
#define SERIALIZE_CPU_MEMORY_REQUEST_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "estructuras.h"
#include "utils/package.h"

#include "commons/log.h"
#include "utils/module.h"

 //CPU - Memoria//
typedef enum e_CPU_Memory_Request {
    INSTRUCTION_REQUEST,
    READ_REQUEST, //utilizado en MEMORIA-IO
    WRITE_REQUEST, //utilizado en MEMORIA-IO
    RESIZE_REQUEST,
    FRAME_ACCESS,    //PARA MEMORIA Y REVISAR LA TLB
    FRAME_REQUEST,
    PAGE_SIZE_REQUEST
} e_CPU_Memory_Request;

/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcb t_PCB a enviar.
 * @param fd_socket Socket desde donde se va a recibir el pcb.
 */
void cpu_memory_request_send(e_CPU_Memory_Request *memory_request, int socket) ;


/**
 * @brief Serializacion del t_PCB para ser enviada.
 * @param package t_Package a rellenar.
 * @param pcb Pcb a serializar
 */
void cpu_memory_request_serialize(t_Payload *payload, e_CPU_Memory_Request *memory_request) ;


/**
 * @brief Deserializacion del t_PCB para ser enviada.
 * @param payload t_Payload.
 */
e_CPU_Memory_Request *cpu_memory_request_deserialize(t_Payload *payload);

void cpu_memory_request_print(e_CPU_Memory_Request *memory_request);
void send_string(char* mensaje, int socket, int opcod);
void send_int(int nro, int socket, int opcod);
void send_2int(int nro, int nro2, int socket, int opcod);
void receive_2int(int* nro1, int* nro2, t_Payload* payload);
void send_String_1int(int nro, char* mensaje, int socket, int opcod);
void receive_2int_1uint32(int* nro1, int* nro2,uint32_t* contenido, t_Payload* payload);
void send_2int_1uint32(int nro1, int nro2,uint32_t contenido, int socket, int opcod);


#endif // SERIALIZE_CPU_MEMORY_REQUEST_H