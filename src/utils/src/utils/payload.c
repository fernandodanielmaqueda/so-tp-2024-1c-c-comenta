/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "payload.h"

// Crea un payload vacío de tamaño size y offset 0
t_Payload *payload_create(void) {

  t_Payload *payload = malloc(sizeof(t_Payload));
  if(payload == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo crear el payload con malloc");
    exit(1);
  }
  
  payload->size = 0;
  payload->stream = NULL;
  return payload;
}

// Libera la memoria asociada al payload
void payload_destroy(t_Payload *payload) {
  if(payload == NULL)
    return;
  if(payload->size != 0) {
    // TODO: Verificar si es necesario liberar el stream caso por caso (creo que no)
    free(payload->stream);
  }
  free(payload);
}

// Agrega un stream al payload en la posición actual y avanza el offset
void payload_enqueue(t_Payload *payload, void *source, size_t sourceSize) {
  // Check for invalid input
  if (payload == NULL || source == NULL || sourceSize == 0) {
    return;
  }

  void *newStream = realloc(payload->stream, (size_t) ((payload->size) + sourceSize));
  if(newStream == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo agregar el stream al payload con realloc");
    exit(1);
  }

  payload->stream = newStream;

  memcpy((void*)(((uint8_t*) payload->stream) + payload->size), source, sourceSize);
  payload->size += (t_PayloadSize) sourceSize;
}

void payload_enqueue_uint8_t(t_Payload *payload, uint8_t *source) {
  payload_enqueue(payload, (void *) source, sizeof(uint8_t));
}

void payload_enqueue_uint32_t(t_Payload *payload, uint32_t *source) {
  payload_enqueue(payload, (void *) source, sizeof(uint32_t));
}

void payload_dequeue(t_Payload *payload, void *destination, size_t destinationSize) {
  // Check for invalid input or not enough data in the payload
  if (payload == NULL || payload->stream == NULL || destination == NULL || destinationSize == 0 || ((size_t) payload->size) < destinationSize) {
    return;
  }

  // Copy the requested data from the payload's stream
  memcpy(destination, payload->stream, destinationSize);

  // Calculate the new size of the payload after removing the data
  size_t newSize = ((size_t) (payload->size)) - destinationSize;

  if(newSize > 0) {
    memmove(payload->stream, (void*)(((uint8_t*) payload->stream) + destinationSize), newSize);

    void *newStream = realloc(payload->stream, newSize);
    if(newStream == NULL) {
      log_error(SERIALIZE_LOGGER, "No se pudo quitar el stream al payload con realloc");
      exit(1);
    }

    payload->stream = newStream;
    payload->size = (t_PayloadSize) newSize;
  } else {
    free(payload->stream);
    payload->stream = NULL;
    payload->size = 0;
  }
}

void payload_dequeue_uint8_t(t_Payload *payload, uint8_t *destination) {
  payload_dequeue(payload, (void *) destination, sizeof(uint8_t));
}

void payload_dequeue_uint32_t(t_Payload *payload, uint32_t *destination) {
  payload_dequeue(payload, (void *) destination, sizeof(uint32_t));
}

size_t memcpy_destination_offset(void *destination, size_t offset_destination, void *source, size_t bytes) {
  memcpy((void*)(((uint8_t*) destination) + offset_destination), source, bytes);
  offset_destination += bytes;
  return offset_destination;
}

size_t memcpy_destination_offset_uint8_t(uint8_t *destination, size_t offset_destination, uint8_t *source) {
  return memcpy_destination_offset((void *) destination, offset_destination, (void *) source, sizeof(uint8_t));
}

size_t memcpy_destination_offset_uint32_t(uint32_t *destination, size_t offset_destination, uint32_t *source) {
  return memcpy_destination_offset(destination, offset_destination, (void *) source, sizeof(uint32_t));
}

size_t memcpy_source_offset(void *destination, void *source, size_t offset_source, size_t bytes) {
  memcpy(destination, (void*)(((uint8_t*) source) + offset_source), bytes);
  offset_source += bytes;
  return offset_source;
}

size_t memcpy_source_offset_uint8_t(uint8_t *destination, uint8_t *source, size_t offset_source) {
  return memcpy_source_offset(destination, (void *) source, offset_source, sizeof(uint8_t));
}

size_t memcpy_source_offset_uint32_t(uint32_t *destination, uint32_t *source, size_t offset_source) {
  return memcpy_source_offset(destination, (void *) source, offset_source, sizeof(uint32_t));
}