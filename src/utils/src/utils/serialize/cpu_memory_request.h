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
typedef enum t_CPU_Memory_Request {
    INSTRUCTION_REQUEST,
    READ_REQUEST, //utilizado en MEMORIA-IO
    WRITE_REQUEST, //utilizado en MEMORIA-IO
    RESIZE_REQUEST,
    FRAME_ACCESS,    //PARA MEMORIA Y REVISAR LA TLB
    FRAME_REQUEST,
    PAGE_SIZE_REQUEST
} t_CPU_Memory_Request;

/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcb t_PCB a enviar.
 * @param fd_socket Socket desde donde se va a recibir el pcb.
 */
void cpu_memory_request_send(enum t_CPU_Memory_Request *instruction, int socket) ;


/**
 * @brief Serializacion del t_PCB para ser enviada.
 * @param package Package a rellenar.
 * @param pcb Pcb a serializar
 */
void cpu_memory_request_serialize(Payload *payload, enum t_CPU_Memory_Request *instruction) ;


/**
 * @brief Deserializacion del t_PCB para ser enviada.
 * @param Payload Payload.
 */
enum t_CPU_Memory_Request *cpu_memory_request_deserialize(Payload *payload);

void cpu_memory_request_print(enum t_CPU_Memory_Request *instruction);

#endif // SERIALIZE_CPU_MEMORY_REQUEST_H