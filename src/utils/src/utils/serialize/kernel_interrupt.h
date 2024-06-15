/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_KERNEL_INTERRUPT_H
#define SERIALIZE_KERNEL_INTERRUPT_H

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

typedef enum e_Kernel_Interrupt { 
    QUANTUM_INTERRUPT,
    KILL_INTERRUPT
} e_Kernel_Interrupt;


/**
 * @brief Enviar kernel_interrupt (incluye el serializado)
 * @param kernel_interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el kernel_interrupt.
 */
void kernel_interrupt_send(e_Kernel_Interrupt *kernel_interrupt, int fd_socket);


/**
 * @brief Serializacion del t_Interrupt para ser enviada.
 * @param package Package a rellenar.
 * @param kernel_interrupt kernel_interrupt a serializar
 */
void kernel_interrupt_serialize(t_Payload *payload, e_Kernel_Interrupt *kernel_interrupt);


/**
 * @brief Deserializacion del t_Interrupt para ser enviada.
 * @param Payload Payload.
 */
e_Kernel_Interrupt *kernel_interrupt_deserialize(t_Payload *payload);


void kernel_interrupt_log(e_Kernel_Interrupt *kernel_interrupt);

#endif // SERIALIZE_KERNEL_INTERRUPT_H