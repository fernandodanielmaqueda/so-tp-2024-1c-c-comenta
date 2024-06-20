/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/text.h"

void text_serialize(t_Payload *payload, char *source) {
  if(payload == NULL || source == NULL)
    return;

  t_StringLength textLength = (t_StringLength) strlen(source) + 1;
  payload_enqueue(payload, (void *) &(textLength), sizeof(t_StringLength));
  payload_enqueue(payload, (void *) source, textLength);

  text_log(source);
}

void text_deserialize(t_Payload *payload, char **destination) {
  if(payload == NULL || destination == NULL)
    return;

  t_StringLength textLength;
  payload_dequeue(payload, (void *) &(textLength), sizeof(t_StringLength));

  *destination = malloc((size_t) textLength);
  if(*destination == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena de destino");
    exit(EXIT_FAILURE);
  }
  
  payload_dequeue(payload, (void *) destination, (size_t) textLength);

  text_log(*destination);
}

void text_log(char *text) {
  if(text == NULL)
    return;

  log_info(SERIALIZE_LOGGER,
    "text: %s"
    , text
    );
}