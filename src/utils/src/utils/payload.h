/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include "commons/log.h"

typedef uint32_t t_PayloadSize;

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
 * @param data A pointer to the data to be enqueued.
 * @param dataSize The size of the data to be enqueued.
 * @return An integer status code (0 for success, non-zero for failure).
 */
void payload_enqueue(t_Payload *payload, void *source, size_t sourceSize);
void payload_enqueue_uint8_t(t_Payload *payload, uint8_t *source);
void payload_enqueue_uint32_t(t_Payload *payload, uint32_t *source);

/**
 * Dequeues data from the payload.
 * @param payload A pointer to the payload structure.
 * @param dataSize The size of the buffer to be dequeued.
 * @return A pointer to the dequeued data. NULL if the operation fails.
 *         The caller is responsible for freeing this memory.
 */
void payload_dequeue(t_Payload *payload, void *destination, size_t destinationSize);
void payload_dequeue_uint8_t(t_Payload *payload, uint8_t *destination);
void payload_dequeue_uint32_t(t_Payload *payload, uint32_t *destination);

size_t memcpy_destination_offset(void *destination, size_t offset_destination, void *source, size_t bytes);
size_t memcpy_destination_offset_uint8_t(uint8_t *destination, size_t offset_destination, uint8_t *source);
size_t memcpy_destination_offset_uint32_t(uint32_t *destination, size_t offset_destination, uint32_t *source);

size_t memcpy_source_offset(void *destination, void *source, size_t offset_source, size_t bytes);
size_t memcpy_source_offset_uint8_t(uint8_t *destination, uint8_t *source, size_t offset_source);
size_t memcpy_source_offset_uint32_t(uint32_t *destination, uint32_t *source, size_t offset_source);

#endif // PAYLOAD_H