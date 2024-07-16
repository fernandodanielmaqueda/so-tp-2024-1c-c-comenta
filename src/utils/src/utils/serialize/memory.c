/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/memory.h"

void physical_address_serialize_element(t_Payload *payload, void *source) {
    if(payload == NULL || source == NULL)
        return;

    physical_address_serialize(payload, *(t_Physical_Address *) source);
}

void physical_address_deserialize_element(t_Payload *payload, void *destination) {
  if(payload == NULL || destination == NULL)
    return;

  physical_address_deserialize(payload, (t_Physical_Address *) destination);
}

void physical_address_serialize(t_Payload *payload, t_Physical_Address source) {
  if(payload == NULL)
    return;

  payload_append(payload, &source, sizeof(source));

  physical_address_log(source);
}

void physical_address_deserialize(t_Payload *payload, t_Physical_Address *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_shift(payload, destination, sizeof(*destination));

  physical_address_log(*destination);
}

void physical_address_log(t_Physical_Address source) {
  log_info(SERIALIZE_LOGGER,
    "t_Physical_Address: %" PRIu32
    , source
  );
}