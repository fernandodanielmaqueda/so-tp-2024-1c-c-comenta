/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_KERNEL_INTERRUPT_H
#define UTILS_SERIALIZE_KERNEL_INTERRUPT_H

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

typedef enum e_Kernel_Interrupt {
    NONE_KERNEL_INTERRUPT,
    QUANTUM_KERNEL_INTERRUPT,
    KILL_KERNEL_INTERRUPT
} e_Kernel_Interrupt;


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

void kernel_interrupt_free(e_Kernel_Interrupt *kernel_interrupt);
void kernel_interrupt_log(e_Kernel_Interrupt *kernel_interrupt);

#endif // UTILS_SERIALIZE_KERNEL_INTERRUPT_H