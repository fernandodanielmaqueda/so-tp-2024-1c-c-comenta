/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_EVICTION_REASON_H
#define UTILS_SERIALIZE_EVICTION_REASON_H

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

typedef enum e_Eviction_Reason {
    ERROR_EVICTION_REASON, // por ejemplo decode
    INTERRUPTION_EVICTION_REASON, //Incluye el quantum
    SYSCALL_EVICTION_REASON // Incluye a EXIT
} e_Eviction_Reason;


/**
 * @brief Serializacion del e_Eviction_Reason para ser enviada.
 * @param package Package a rellenar.
 * @param eviction_reason eviction_reason a serializar
 */
void eviction_reason_serialize(t_Payload *payload, e_Eviction_Reason *eviction_reason);


/**
 * @brief Deserializacion del e_Eviction_Reason para ser enviada.
 * @param Payload Payload.
 */
e_Eviction_Reason *eviction_reason_deserialize(t_Payload *payload);

void eviction_reason_free(e_Eviction_Reason *eviction_reason);
void eviction_reason_log(e_Eviction_Reason *eviction_reason);

#endif // UTILS_SERIALIZE_EVICTION_REASON_H