/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subpayload.h"

void subpayload_serialize(t_Payload *payload, t_Payload source) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, (void *) &(source.size), sizeof(t_PayloadSize));
  payload_enqueue(payload, (void *) source.stream, (size_t) source.size);

  subpayload_log(source);
}

void subpayload_deserialize(t_Payload *payload, t_Payload *subpayload) {
  if(payload == NULL || subpayload == NULL)
    return;

  payload_dequeue(payload, (void *) &(subpayload->size), sizeof(t_PayloadSize)); 
  payload_dequeue(payload, (void *) subpayload->stream, (size_t) subpayload->size);

  subpayload_log(*subpayload);
}

void subpayload_log(t_Payload subpayload) {

  log_info(SERIALIZE_LOGGER,
    "t_Payload:\n"
    "* size: %" PRIu32 "\n"
    "* stream: %p"
    , subpayload.size
    , subpayload.stream
  );
}