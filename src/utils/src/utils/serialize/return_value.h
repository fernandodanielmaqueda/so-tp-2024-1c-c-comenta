/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_RETURN_VALUE_H
#define UTILS_SERIALIZE_RETURN_VALUE_H

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

typedef int8_t t_Return_Value;


/**
 * @brief Serializacion del t_Return_Value para ser enviada.
 * @param package Package a rellenar.
 * @param return_value return_value a serializar
 */
void return_value_serialize(t_Payload *payload, t_Return_Value *return_value);


/**
 * @brief Deserializacion del t_Return_Value para ser enviada.
 * @param Payload Payload.
 */
t_Return_Value *return_value_deserialize(t_Payload *payload);

void return_value_free(t_Return_Value *return_value);
void return_value_log(t_Return_Value *return_value);

#endif // UTILS_SERIALIZE_RETURN_VALUE_H