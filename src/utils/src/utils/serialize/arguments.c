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

void arguments_log(t_Arguments *arguments) {

  /*
  char *buffer;
  size_t bufferSize = 0;

      // Calcular el tamaño total necesario
      for(int i = 0; i < arguments->argc; i++) {
          bufferSize += strlen(argv[i]) + 1; // + 1 para el espacio ó para el \0
      }

      char *concatenated_args = (char *) malloc(bufferSize * sizeof(char));
      if(concatenated_args == NULL) {
        log_error(SERIALIZE_LOGGER, "No se pudo reservar memoria para la cadena concatenada de argumentos")
        exit(EXIT_FAILURE);
      }

      concatenated_args[0] = '\0';

      for(int i = 0; i < arguments->argc; i++) {
        strcat(concatenated_args, argv[i]);
        strcat(concatenated_args, " ");
      }

  log_info(SERIALIZE_LOGGER,
    "t_Arguments[%p]:\n"
    "* argc: %" PRIu8 "\n"
    "* argv: %s"
    ,(void *) arguments,
    arguments->argc,
    //pcb->arrival_RUNNING
    );

  free(concatenated_args);
  */
}