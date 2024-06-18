/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/eviction_reason.h"

void eviction_reason_serialize(t_Payload *payload, e_Eviction_Reason *eviction_reason) {
  payload_enqueue(payload, eviction_reason, sizeof(uint8_t));

  eviction_reason_log(eviction_reason);
}

e_Eviction_Reason *eviction_reason_deserialize(t_Payload *payload) {
  e_Eviction_Reason *eviction_reason = malloc(sizeof(e_Eviction_Reason));

  payload_dequeue(payload, eviction_reason, sizeof(uint8_t));

  eviction_reason_log(eviction_reason);
  return eviction_reason;
}

void eviction_reason_free(e_Eviction_Reason *eviction_reason) {
  free(eviction_reason);
}

void eviction_reason_log(e_Eviction_Reason *eviction_reason) {
  log_info(SERIALIZE_LOGGER,
    "e_Eviction_Reason[%p]: %d"
    , (void *) eviction_reason
    , *eviction_reason
  );
}