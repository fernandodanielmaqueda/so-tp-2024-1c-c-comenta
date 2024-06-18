/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_CPU_MEMORY_REQUEST_H
#define UTILS_SERIALIZE_CPU_MEMORY_REQUEST_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "utils/package.h"

#include "commons/log.h"
#include "utils/module.h"

 //CPU - Memoria//
typedef enum e_CPU_Memory_Request {
    INSTRUCTION_REQUEST,
    READ_REQUEST, //utilizado en MEMORIA-IO
    WRITE_REQUEST, //utilizado en MEMORIA-IO
    RESIZE_REQUEST,
    OUT_OF_MEMORY,
    FRAME_ACCESS,    //PARA MEMORIA Y REVISAR LA TLB
    FRAME_REQUEST,
    PAGE_SIZE_REQUEST
} e_CPU_Memory_Request;


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

void cpu_memory_request_free(e_CPU_Memory_Request *memory_request);
void cpu_memory_request_log(e_CPU_Memory_Request *memory_request);

#endif // UTILS_SERIALIZE_CPU_MEMORY_REQUEST_H