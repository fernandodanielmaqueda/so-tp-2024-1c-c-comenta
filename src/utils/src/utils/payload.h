/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdlib.h>
#include <netdb.h>
#include <string.h>

typedef uint32_t t_PayloadSize;

typedef struct t_Payload {
    t_PayloadSize size; // Tamaño del payload
    void* stream; // Payload
} t_Payload;

t_Payload *payload_create(void);
void payload_destroy(t_Payload *payload);

/**
 * @brief Agregar dato a paquete
 * @param payload t_Payload a rellenar.
 * @param data Datos a agregar
 * @param dataSize Tamaño de los datos a agregar.
 */
void payload_enqueue(t_Payload *payload, void *data, size_t dataSize);

size_t memcpy_destination_offset(void *destination, size_t offset, void *source, size_t bytes);
size_t memcpy_source_offset(void *destination, void *source, size_t offset, size_t bytes);

#endif // PAYLOAD_H