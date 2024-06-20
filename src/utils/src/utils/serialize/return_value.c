/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/return_value.h"

void return_value_serialize(t_Payload *payload, t_Return_Value return_value) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, &return_value, sizeof(t_Return_Value));

  return_value_log(return_value);
}

void return_value_deserialize(t_Payload *payload, t_Return_Value *return_value) {
  if(payload == NULL || return_value == NULL)
    return;

  payload_dequeue(payload, return_value, sizeof(t_Return_Value));

  return_value_log(*return_value);
}

void return_value_log(t_Return_Value return_value) {
  log_info(SERIALIZE_LOGGER,
    "t_Return_Value: %d"
    , return_value
  );
}