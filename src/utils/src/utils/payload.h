/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_PAYLOAD_H
#define UTILS_PAYLOAD_H

#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include "commons/log.h"

typedef uint32_t t_PayloadSize;
typedef int8_t t_EnumValue;

typedef struct t_Payload {
    t_PayloadSize size;
    void *stream;
} t_Payload;

extern t_log *SERIALIZE_LOGGER;

void payload_init(t_Payload *payload);
void payload_destroy(t_Payload *payload);


/**
 * Agrega datos al final del payload.
 * @param payload Puntero al payload.
 * @param source Puntero a los datos fuente.
 * @param sourceSize Tamaño en bytes de los datos fuente.
 */
void payload_append(t_Payload *payload, void *source, size_t sourceSize);


/**
 * Agrega datos al inicio del payload.
 * @param payload Puntero al payload.
 * @param source Puntero a los datos fuente.
 * @param sourceSize El tamaño en bytes de los datos fuente.
 */
void payload_prepend(t_Payload *payload, void *source, size_t sourceSize);


/**
 * Remueve datos del payload.
 * @param payload Puntero al payload.
 * @param destination Puntero al buffer de destino.
 * @param destinationSize Tamaño en bytes del buffer de destino.
 */
void payload_shift(t_Payload *payload, void *destination, size_t destinationSize);


/**
 * Remueve datos del payload.
 * @param payload Puntero al payload.
 * @param destination Puntero al buffer de destino.
 * @param destinationSize Tamaño en bytes del buffer de destino.
 */
void payload_truncate(t_Payload *payload, void *destination, size_t destinationSize);


/**
 * Copies data from source to destination.
 * @param destination A pointer to the destination buffer.
 * @param offset_destination The offset in the destination buffer.
 * @param source A pointer to the source buffer.
 * @param bytes The number of bytes to copy.
 * @return The new offset in the destination buffer.
 */
size_t memcpy_serialize(void *destination, size_t offset_destination, void *source, size_t bytes);


/**
 * Copies data from source to destination.
 * @param destination A pointer to the destination buffer.
 * @param source A pointer to the source buffer.
 * @param offset_source The offset in the source buffer.
 * @param bytes The number of bytes to copy.
 * @return The new offset in the source buffer.
 */
size_t memcpy_deserialize(void *destination, void *source, size_t offset_source, size_t bytes);

#endif // UTILS_PAYLOAD_H