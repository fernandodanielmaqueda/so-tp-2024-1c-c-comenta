/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_EXIT_H
#define SERIALIZE_EXIT_H

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

typedef enum e_Exit {
    SUCCESS_EXIT,
    FAILURE_EXIT
} e_Exit;

/**
 * @brief Enviar exit (incluye el serializado)
 * @param exit t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el exit.
 */
void exit_send(e_Exit *exit, int fd_socket);


/**
 * @brief Serializacion del t_Interrupt para ser enviada.
 * @param package Package a rellenar.
 * @param exit exit a serializar
 */
void exit_serialize(t_Payload *payload, e_Exit *exit);


/**
 * @brief Deserializacion del t_Interrupt para ser enviada.
 * @param Payload Payload.
 */
e_Exit *exit_deserialize(t_Payload *payload);


void exit_log(e_Exit *exit);

#endif // SERIALIZE_EXIT_H