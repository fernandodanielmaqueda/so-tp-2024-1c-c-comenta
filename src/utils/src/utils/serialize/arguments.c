/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/arguments.h"

void arguments_send(t_Arguments *arguments, int fd_socket) {
  t_Package *package = package_create_with_header(ARGUMENTS_HEADER);
  arguments_serialize(package->payload, arguments);
  package_send(package, fd_socket);
  package_destroy(package);
}

void arguments_serialize(t_Payload *payload, t_Arguments *arguments) {

  payload_enqueue(payload, &(arguments->argc), sizeof(arguments->argc));

  for(int i = 0; i < arguments->argc; i++) {
    payload_enqueue_string(payload, arguments->argv[i]);
  }

  arguments_log(arguments);
}

t_Arguments *arguments_deserialize(t_Payload *payload) {
  t_Arguments *arguments = malloc(sizeof(t_Arguments));

  payload_dequeue(payload, &(arguments->argc), sizeof(arguments->argc));

  for(int i = 0; i < arguments->argc; i++) {
    payload_dequeue_string(payload, &(arguments->argv[i]));
  }

  arguments_log(arguments);
  return arguments;
}

void arguments_free(t_Arguments *arguments) {
  // @
}

void arguments_log(t_Arguments *arguments) {
  
  /*
  char *newBuffer;
  char *buffer;
  size_t bufferSize = 0;

  for(int i = 0; i < arguments->argc; i++) {

    while(*(arguments->argv[i])) {

      bufferSize += strlen(arguments->argv[i]) + 1;

      newBuffer = (char *) realloc(buffer, bufferSize);
      if(newBuffer == NULL) {
        log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena concatenada de argumentos");
        exit(EXIT_FAILURE);
      }
      buffer = newBuffer;

      buffer[bufferSize - 1] = *(arguments->argv[i]);
    }

    bufferSize += 1;

    newBuffer = realloc(bufferSize);
    if(newBuffer == NULL) {
      log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena concatenada de argumentos")
      exit(EXIT_FAILURE);
    }
    buffer = newBuffer;

    buffer[bufferSize - 1] = ' ';
    

  }

  if((int i = arguments->argc) )



  if(int i = (arguments->argc - 1)) {
    bufferSize += 1;

    newBuffer = realloc(bufferSize);
    if(newBuffer == NULL) {
      log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena concatenada de argumentos")
      exit(EXIT_FAILURE);
    }
    buffer = newBuffer;

    while(*(arguments->argv[i])) 
      buffer[bufferSize - 1] = *(arguments->argv[i]);

  }
  bufferSize += 1;


  log_info(SERIALIZE_LOGGER,
    "t_Arguments[%p]:\n"
    "* argc: %" PRIu8 "\n"
    "* argv: %s"
    , (void *) arguments,
    , arguments->argc
    , buffer
    );

  free(buffer);

  */

}