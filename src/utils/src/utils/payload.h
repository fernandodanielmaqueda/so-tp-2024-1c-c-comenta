/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include "commons/log.h"

typedef uint32_t t_PayloadSize;
typedef uint32_t t_StringLength;

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
void payload_enqueue_uint8_t(t_Payload *payload, uint8_t *source);
void payload_enqueue_uint32_t(t_Payload *payload, uint32_t *source);
void payload_enqueue_string(t_Payload *payload, char *source);


/**
 * Dequeues data from the payload.
 * @param payload A pointer to the payload structure.
 * @param destination A pointer to the destination buffer.
 * @param destinationSize The size of the destination buffer.
 * @return An integer status code (0 for success, non-zero for failure).
 */
void payload_dequeue(t_Payload *payload, void *destination, size_t destinationSize);
void payload_dequeue_uint8_t(t_Payload *payload, uint8_t *destination);
void payload_dequeue_uint32_t(t_Payload *payload, uint32_t *destination);
void payload_dequeue_string(t_Payload *payload, char **destination);


/**
 * Copies data from source to destination.
 * @param destination A pointer to the destination buffer.
 * @param offset_destination The offset in the destination buffer.
 * @param source A pointer to the source buffer.
 * @param bytes The number of bytes to copy.
 * @return The new offset in the destination buffer.
 */
size_t memcpy_serialize(void *destination, size_t offset_destination, void *source, size_t bytes);
size_t memcpy_serialize_uint8_t(void *destination, size_t offset_destination, uint8_t *source);
size_t memcpy_serialize_uint32_t(void *destination, size_t offset_destination, uint32_t *source);
size_t memcpy_serialize_string(void *destination, size_t offset_destination, char *source);


/**
 * Copies data from source to destination.
 * @param destination A pointer to the destination buffer.
 * @param source A pointer to the source buffer.
 * @param offset_source The offset in the source buffer.
 * @param bytes The number of bytes to copy.
 * @return The new offset in the source buffer.
 */
size_t memcpy_deserialize(void *destination, void *source, size_t offset_source, size_t bytes);
size_t memcpy_deserialize_uint8_t(uint8_t *destination, void *source, size_t offset_source);
size_t memcpy_deserialize_uint32_t(uint32_t *destination, void *source, size_t offset_source);
size_t memcpy_deserialize_string(char **destination, void *source, size_t offset_source);

#endif // PAYLOAD_H