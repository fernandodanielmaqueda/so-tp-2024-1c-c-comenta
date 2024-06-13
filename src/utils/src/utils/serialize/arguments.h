/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_ARGUMENTS_H
#define SERIALIZE_ARGUMENTS_H

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

typedef struct t_Arguments {
    uint8_t argc;
    char *argv[];
} t_Arguments;


/**
 * @brief Enviar arguments (incluye el serializado)
 * @param arguments t_Arguments a enviar.
 * @param fd_socket Socket desde donde se va a recibir arguments.
 */
void arguments_send(t_Arguments *arguments, int fd_socket);


/**
 * @brief Serializacion del t_Arguments para ser enviada.
 * @param payload t_Payload a rellenar.
 * @param arguments t_Arguments a serializar
 */
void arguments_serialize(t_Payload *payload, t_Arguments *arguments);


/**
 * @brief Deserializacion del t_Arguments para ser enviada.
 * @param payload t_Payload.
 */
t_Arguments *arguments_deserialize(t_Payload *payload);


void arguments_log(t_Arguments *arguments);

#endif // SERIALIZE_ARGUMENTS_H