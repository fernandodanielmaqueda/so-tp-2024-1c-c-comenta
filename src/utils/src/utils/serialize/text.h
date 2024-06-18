/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_TEXT_H
#define UTILS_SERIALIZE_TEXT_H

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

typedef uint32_t t_StringLength;

/**
 * @brief Serializacion del texto para ser enviada.
 * @param package Package a rellenar.
 * @param string Texto a serializar
 */
void text_serialize(t_Payload *payload, char *text);


/**
 * @brief Deserializacion del texto para ser enviada.
 * @param Payload Payload.
 */
char *text_deserialize(t_Payload *payload);

void text_free(char *text);


void text_log(char *text);

#endif // UTILS_SERIALIZE_TEXT_H