/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_SUBPAYLOAD_H
#define UTILS_SERIALIZE_SUBPAYLOAD_H

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


/**
 * @brief Serializacion del t_Interrupt para ser enviada.
 * @param package Package a rellenar.
 * @param interrupt interrupt a serializar
 */
void subpayload_serialize(t_Payload *payload, t_Payload *subpayload);


/**
 * @brief Deserializacion del t_Interrupt para ser enviada.
 * @param Payload Payload.
 */
t_Payload *subpayload_deserialize(t_Payload *payload);


void subpayload_free(t_Payload *subpayload);
void subpayload_log(t_Payload *subpayload);

#endif // UTILS_SERIALIZE_SUBPAYLOAD_H