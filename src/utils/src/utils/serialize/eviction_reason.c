/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/eviction_reason.h"

void eviction_reason_serialize(t_Payload *payload, e_Eviction_Reason source) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, &source, sizeof(t_EnumValue));

  eviction_reason_log(source);
}

void eviction_reason_deserialize(t_Payload *payload, e_Eviction_Reason *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_dequeue(payload, destination, sizeof(t_EnumValue));

  eviction_reason_log(*destination);
}

void eviction_reason_log(e_Eviction_Reason eviction_reason) {
  log_info(SERIALIZE_LOGGER,
    "e_Eviction_Reason: %d"
    , eviction_reason
  );
}