/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_PAYLOAD_H
#define UTILS_PAYLOAD_H

#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include "commons/log.h"

typedef uint32_t t_PayloadSize;
typedef uint8_t t_EnumValue;

typedef struct t_Payload {
    t_PayloadSize size; // Tamaño del payload
    void* stream; // Payload
} t_Payload;

extern t_log *SERIALIZE_LOGGER;

t_Payload *payload_create(void);
void payload_destroy(t_Payload *payload);


/**
 * Enqueues data into the payload.
 * @param payload A pointer to the payload structure.
 * @param source A pointer to the source buffer.
 * @param sourceSize The size of the source buffer.
 */
void payload_enqueue(t_Payload *payload, void *source, size_t sourceSize);


/**
 * Dequeues data from the payload.
 * @param payload A pointer to the payload structure.
 * @param destination A pointer to the destination buffer.
 * @param destinationSize The size of the destination buffer.
 * @return An integer status code (0 for success, non-zero for failure).
 */
void payload_dequeue(t_Payload *payload, void *destination, size_t destinationSize);


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