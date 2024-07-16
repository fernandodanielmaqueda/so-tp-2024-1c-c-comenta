/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/text.h"

void text_serialize(t_Payload *payload, char *source) {
  if(payload == NULL)
    return;
  
  t_StringLength textSize;

  if(source == NULL) {
    textSize = 0;
    payload_append(payload, (void *) &(textSize), sizeof(textSize));
  } else {
    textSize = (t_StringLength) strlen(source) + 1;
    payload_append(payload, (void *) &(textSize), sizeof(textSize));
    payload_append(payload, (void *) source, textSize);
  }

  text_log(source);
}

void text_deserialize(t_Payload *payload, char **destination) {
  if(payload == NULL || destination == NULL)
    return;

  t_StringLength textSize;
  payload_shift(payload, (void *) &(textSize), sizeof(textSize));

  if(!textSize) {
    *destination = NULL;
  } else {
    *destination = malloc((size_t) textSize);
    if(*destination == NULL) {
      log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena de destino");
      exit(EXIT_FAILURE);
    }
    
    payload_shift(payload, (void *) *destination, (size_t) textSize);
  }

  text_log(*destination);
}

void text_log(char *text) {

  log_info(SERIALIZE_LOGGER,
    "text: %s"
    , (text != NULL) ? text : "(nil)"
    );
}