/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "payload.h"

// Crea un payload vacío de tamaño size y offset 0
t_Payload *payload_create(void) {

  t_Payload *payload = malloc(sizeof(t_Payload));
  if(payload == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo crear el payload con malloc");
    exit(EXIT_FAILURE);
  }
  
  payload->size = 0;
  payload->stream = NULL;
  return payload;
}

// Libera la memoria asociada al payload
void payload_destroy(t_Payload *payload) {
  if(payload == NULL)
    return;
  free(payload->stream);
  free(payload);
}

// Agrega un stream al payload en la posición actual y avanza el offset
void payload_enqueue(t_Payload *payload, void *source, size_t sourceSize) {
  // Check for invalid input
  if (payload == NULL || source == NULL || sourceSize == 0) {
    return;
  }

  void *newStream = realloc(payload->stream, ((size_t) payload->size) + sourceSize);
  if(newStream == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo agregar el stream al payload con realloc");
    exit(EXIT_FAILURE);
  }

  payload->stream = newStream;

  memcpy((void*)(((uint8_t *) payload->stream) + payload->size), source, sourceSize);
  payload->size += (t_PayloadSize) sourceSize;
}

void payload_dequeue(t_Payload *payload, void *destination, size_t destinationSize) {
  // Check for invalid input or not enough data in the payload
  if (payload == NULL || payload->stream == NULL || destinationSize == 0 || ((size_t) payload->size) < destinationSize) {
    return;
  }

  // Copy the requested data from the payload's stream
  if(destination != NULL)
    memcpy(destination, payload->stream, destinationSize);

  // Calculate the new size of the payload after removing the data
  size_t newSize = ((size_t) (payload->size)) - destinationSize;

  if(newSize > 0) {
    memmove(payload->stream, (void*)(((uint8_t*) payload->stream) + destinationSize), newSize);

    void *newStream = realloc(payload->stream, newSize);
    if(newStream == NULL) {
      log_error(SERIALIZE_LOGGER, "No se pudo quitar el stream al payload con realloc");
      exit(EXIT_FAILURE);
    }

    payload->stream = newStream;
    payload->size = (t_PayloadSize) newSize;
  } else {
    free(payload->stream);
    payload->stream = NULL;
    payload->size = 0;
  }
}

size_t memcpy_serialize(void *destination, size_t offset_destination, void *source, size_t bytes) {
  memcpy((void*)(((uint8_t*) destination) + offset_destination), source, bytes);
  offset_destination += bytes;
  return offset_destination;
}

size_t memcpy_deserialize(void *destination, void *source, size_t offset_source, size_t bytes) {
  memcpy(destination, (void*)(((uint8_t*) source) + offset_source), bytes);
  offset_source += bytes;
  return offset_source;
}