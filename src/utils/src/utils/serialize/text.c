/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/text.h"

void text_serialize(t_Payload *payload, char *text) {
  t_StringLength textLength = (t_StringLength) strlen(text) + 1;
  payload_enqueue(payload, (void *) &(textLength), sizeof(t_StringLength));
  payload_enqueue(payload, (void *) text, textLength);

  text_log(text);
}

char *text_deserialize(t_Payload *payload) {
  t_StringLength textLength;
  payload_dequeue(payload, (void *) &(textLength), sizeof(t_StringLength));

  char *text = malloc((size_t) textLength);
  if(text == NULL) {
    log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena de destino");
    exit(EXIT_FAILURE);
  }
  
  payload_dequeue(payload, (void *) text, (size_t) textLength);

  text_log(text);
  return text;
}

void text_free(char *text) {
  free(text);
}

void text_log(char *text) {
  log_info(SERIALIZE_LOGGER,
    "text[%p]: %s"
    , (void *) text
    , text
    );
}