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
#include <readline/readline.h>
#include "commons/config.h"
#include "commons/string.h"
#include "commons/log.h"
#include "commons/log.h"
#include "estructuras.h"
#include "utils/package.h"
#include "utils/module.h"

#define MAX_ARGC 6 // 1 para el nombre del comando + 5 para los argumentos

typedef struct t_Arguments {
    uint8_t argc;
    char **argv;
} t_Arguments;


t_Arguments *arguments_create(char *line, t_log *logger);
char *strip_whitespaces(char *string);

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