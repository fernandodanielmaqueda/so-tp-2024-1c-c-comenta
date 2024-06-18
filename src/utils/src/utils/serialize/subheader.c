/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subheader.h"

void subheader_serialize(t_Payload *payload, e_Header *subheader) {
  payload_enqueue(payload, subheader, sizeof(t_EnumValue));

  subheader_log(subheader);
}

e_Header *subheader_deserialize(t_Payload *payload) {
  e_Header *subheader = malloc(sizeof(e_Header));

  payload_dequeue(payload, subheader, sizeof(t_EnumValue));

  subheader_log(subheader);
  return subheader;
}

void subheader_free(e_Header *subheader) {
  free(subheader);
}

void subheader_log(e_Header *subheader) {
  log_info(SERIALIZE_LOGGER,
    "e_Header[%p]: %d"
    , (void *) subheader
    , *subheader
  );
}