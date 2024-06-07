/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_interrupt_H
#define SERIALIZE_interrupt_H

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

typedef enum t_Interrupt {
    TYPE_INTERRUPT_SIN_INT,
    TYPE_INTERRUPT_FIN_PROCESO,
    TYPE_INTERRUPT_FIN_QUANTUM
} t_Interrupt;


/**
 * @brief Enviar interrupt (incluye el serializado)
 * @param interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el interrupt.
 */
void interrupt_send(enum t_Interrupt *interrupt, int fd_socket);


/**
 * @brief Serializacion del t_Interrupt para ser enviada.
 * @param package Package a rellenar.
 * @param interrupt interrupt a serializar
 */
void interrupt_serialize(Payload *payload, enum t_Interrupt *interrupt);


/**
 * @brief Deserializacion del t_Interrupt para ser enviada.
 * @param Payload Payload.
 */
enum t_Interrupt *interrupt_deserialize(Payload *payload);


void interrupt_print(enum t_Interrupt *interrupt);

#endif // SERIALIZE_interrupt_H