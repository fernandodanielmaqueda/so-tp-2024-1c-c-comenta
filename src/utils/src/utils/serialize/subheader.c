/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subheader.h"

void subheader_serialize(t_Payload *payload, e_Header source) {
  if(payload == NULL)
    return;

  t_EnumValue aux;
  
    aux = (t_EnumValue) source;
  payload_enqueue(payload, &aux, sizeof(t_EnumValue));

  subheader_log(source);
}

void subheader_deserialize(t_Payload *payload, e_Header *destination) {
  if(payload == NULL || destination == NULL)
    return;

  t_EnumValue aux;

  payload_dequeue(payload, &aux, sizeof(t_EnumValue));
    *destination = (e_Header) aux;

  subheader_log(*destination);
}

void subheader_log(e_Header subheader) {
  log_info(SERIALIZE_LOGGER,
    "e_Header: %d"
    , subheader
  );
}