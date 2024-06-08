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

typedef enum e_Interrupt { // CONTEXT_SWITCH_CAUSE
    ERROR_CAUSE, // por ejemplo decode
    SYSCALL_CAUSE, //incluye el EXIT
    INTERRUPTION_CAUSE //Incluye el quantum
} e_Interrupt;

/**
 * @brief Enviar interrupt (incluye el serializado)
 * @param interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el interrupt.
 */
void interrupt_send(e_Interrupt *interrupt, int fd_socket);


/**
 * @brief Serializacion del t_Interrupt para ser enviada.
 * @param package Package a rellenar.
 * @param interrupt interrupt a serializar
 */
void interrupt_serialize(t_Payload *payload, e_Interrupt *interrupt);


/**
 * @brief Deserializacion del t_Interrupt para ser enviada.
 * @param Payload Payload.
 */
e_Interrupt *interrupt_deserialize(t_Payload *payload);


void interrupt_print(e_Interrupt *interrupt);

#endif // SERIALIZE_interrupt_H