/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "payload.h"

// Crea un payload vacío de tamaño size y offset 0
Payload *payload_create(void) {
  Payload *payload = malloc(sizeof(Payload));
  payload->size = 0;
  payload->stream = NULL;
  return payload;
}

// Libera la memoria asociada al payload
void payload_destroy(Payload *payload) {
  if(payload == NULL) return;
  if(payload->size != 0) {
    // TODO: Verificar si es necesario liberar el stream caso por caso (creo que no)
    free(payload->stream);
  }
  free(payload);
}

// Agrega un stream al payload en la posición actual y avanza el offset
void payload_add(Payload *payload, void *data, size_t dataSize) {
  payload->stream = realloc(payload->stream, (size_t) (payload->size + dataSize));
  memcpy((void*)(((uint8_t*) payload->stream) + payload->size), data, dataSize);
  payload->size += (PayloadSize) dataSize;
}

size_t memcpy_destination_offset(void *destination, size_t offset, void *source, size_t bytes) {
  memcpy((void*)(((uint8_t*) destination) + offset), source, bytes);
  offset += bytes;
  return offset;
}

size_t memcpy_source_offset(void *destination, void *source, size_t offset, size_t bytes) {
  memcpy(destination, (void*)(((uint8_t*) source) + offset), bytes);
  offset += bytes;
  return offset;
}