/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subheader.h"

void subheader_serialize(t_Payload *payload, e_Header source) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, &source, sizeof(t_EnumValue));

  subheader_log(source);
}

void subheader_deserialize(t_Payload *payload, e_Header *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_dequeue(payload, destination, sizeof(t_EnumValue));

  subheader_log(*destination);
}

void subheader_log(e_Header subheader) {
  log_info(SERIALIZE_LOGGER,
    "e_Header: %d"
    , subheader
  );
}