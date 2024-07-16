/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subpayload.h"

void subpayload_serialize(t_Payload *payload, t_Payload source) {
  if(payload == NULL)
    return;

  payload_append(payload, (void *) &(source.size), sizeof(source.size));
  payload_append(payload, (void *) source.stream, (size_t) source.size);

  subpayload_log(source);
}

void subpayload_deserialize(t_Payload *payload, t_Payload *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_shift(payload, (void *) &(destination->size), sizeof(destination->size));

  destination->stream = malloc((size_t) destination->size);
  if(destination->stream == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para el stream de destino");
    exit(EXIT_FAILURE);
  }

  payload_shift(payload, (void *) destination->stream, (size_t) destination->size);

  subpayload_log(*destination);
}

void subpayload_log(t_Payload source) {

  log_info(SERIALIZE_LOGGER,
    "t_Payload:\n"
    "* size: %" PRIu32 "\n"
    "* stream: %p"
    , source.size
    , source.stream
  );
}