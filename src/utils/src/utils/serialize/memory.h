/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_MEMORY_H
#define UTILS_SERIALIZE_MEMORY_H

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

typedef uint32_t t_MemorySize;
typedef uint32_t t_Logical_Address;
typedef uint32_t t_Physical_Address;
typedef uint32_t t_Offset;
typedef uint32_t t_Page_Number;
typedef t_Page_Number t_Frame_Number;
typedef uint32_t t_Page_Quantity;


void physical_address_serialize_element(t_Payload *payload, void *source);


void physical_address_deserialize_element(t_Payload *payload, void *destination);


/**
 * @brief Serializacion de un t_Physical_Address para ser enviado.
 * @param payload Payload a encolar.
 * @param source t_Physical_Address fuente a serializar
 */
void physical_address_serialize(t_Payload *payload, t_Physical_Address source);


/**
 * @brief Deserializacion de un t_Physical_Address para ser leido.
 * @param payload Payload a desencolar.
 * @param destination Destino del t_Physical_Address deserializado
 */
void physical_address_deserialize(t_Payload *payload, t_Physical_Address *destination);


/**
 * @brief Loguea un t_Physical_Address.
 * @param source t_Physical_Address a loguear.
 */
void physical_address_log(t_Physical_Address source);

#endif // UTILS_SERIALIZE_MEMORY_H