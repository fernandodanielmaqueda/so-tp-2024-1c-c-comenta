/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_SUBHEADER_H
#define UTILS_SERIALIZE_SUBHEADER_H

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
 * @brief Serializacion del e_Header para ser enviada.
 * @param package Package a rellenar.
 * @param subheader Subheader a serializar
 */
void subheader_serialize(t_Payload *payload, e_Header *subheader);


/**
 * @brief Deserializacion del e_Header para ser enviada.
 * @param Payload Payload.
 */
e_Header *subheader_deserialize(t_Payload *payload);


void subheader_free(e_Header *subheader);
void subheader_log(e_Header *subheader);

#endif // UTILS_SERIALIZE_SUBHEADER_H