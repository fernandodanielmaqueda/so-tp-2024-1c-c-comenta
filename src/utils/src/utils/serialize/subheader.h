/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_HEADER_H
#define SERIALIZE_HEADER_H

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

/**
 * @brief Enviar interrupt (incluye el serializado)
 * @param interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el interrupt.
 */
void header_send(e_Header *header, int fd_socket);


/**
 * @brief Serializacion del t_Interrupt para ser enviada.
 * @param package Package a rellenar.
 * @param interrupt interrupt a serializar
 */
void header_serialize(t_Payload *payload, e_Header *header);


/**
 * @brief Deserializacion del t_Interrupt para ser enviada.
 * @param Payload Payload.
 */
e_Header *header_deserialize(t_Payload *payload);


void header_log(e_Header *header);

#endif // SERIALIZE_HEADER_H