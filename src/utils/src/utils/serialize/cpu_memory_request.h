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
 * @brief Serializacion de un e_CPU_Memory_Request para ser enviado.
 * @param payload Payload a encolar.
 * @param source e_CPU_Memory_Request fuente a serializar
 */
void cpu_memory_request_serialize(t_Payload *payload, e_CPU_Memory_Request source) ;


/**
 * @brief Deserializacion de un e_CPU_Memory_Request para ser leido.
 * @param payload Payload a desencolar.
 * @param destination Destino del e_CPU_Memory_Request deserializado
 */
void cpu_memory_request_deserialize(t_Payload *payload, e_CPU_Memory_Request *destination);


/**
 * @brief Loguea un e_CPU_Memory_Request.
 * @param memory_request e_CPU_Memory_Request a loguear.
 */
void cpu_memory_request_log(e_CPU_Memory_Request memory_request);

#endif // UTILS_SERIALIZE_CPU_MEMORY_REQUEST_H