/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subpayload.h"

void subpayload_serialize(t_Payload *payload, t_Payload *subpayload) {
  payload_enqueue(payload, (void *) &(subpayload->size), sizeof(t_PayloadSize));
  payload_enqueue(payload, (void *) subpayload->stream, (size_t) subpayload->size);

  subpayload_log(subpayload);
}

t_Payload *subpayload_deserialize(t_Payload *payload) {
  t_Payload *subpayload = payload_create();

  payload_dequeue(payload, (void *) &(subpayload->size), sizeof(t_PayloadSize)); 
  payload_dequeue(payload, (void *) subpayload->stream, (size_t) subpayload->size);

  subpayload_log(subpayload);
  return subpayload;
}

void subpayload_free(t_Payload *subpayload) {
  payload_destroy(subpayload);
}

void subpayload_log(t_Payload *subpayload) {
  log_info(SERIALIZE_LOGGER,
    "t_Payload[%p]:\n"
    "* size: %" PRIu32 "\n"
    "* stream: %p"
    , (void *) subpayload
    , subpayload->size
    , subpayload->stream
  );
}