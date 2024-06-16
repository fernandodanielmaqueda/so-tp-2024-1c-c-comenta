/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/arguments.h"

t_Arguments *arguments_create(char *line, t_log *logger) {
    char *subline;

    t_Arguments *arguments = malloc(sizeof(t_Arguments));
    if(arguments == NULL) {
        log_error(logger, "No se pudo reservar memoria para los argumentos.");
        exit(EXIT_FAILURE);
    }

    arguments->argc = 0;
    arguments->argv = malloc(MAX_ARGC * sizeof(char *));

    for(int i = 0; i < MAX_ARGC; i++) {
        arguments->argv[i] = NULL;
    }

    subline = strip_whitespaces(line);
    char *duplicate = strdup(subline);

    register int i = 0;

    while(duplicate[i]) {
        while(duplicate[i] && whitespace(duplicate[i]))
            i++;

        if(!duplicate[i])
            break;

        if(arguments->argc == MAX_ARGC) {
            log_warning(logger, "Demasiados argumentos.");
            exit(EXIT_FAILURE);
        }

        arguments->argv[arguments->argc++] = duplicate + i;

        while(duplicate[i] && !whitespace(duplicate[i]))
            i++;

        if(duplicate[i])
            duplicate[i++] = '\0';
    }
}

/* Strip whitespace from the start and end of STRING.  Return a pointer into STRING. */
char *strip_whitespaces(char *string) {
  register char *start, *end;

  // Recorre el inicio de cadena hasta encontrar un caracter que no sea un espacio
  for (start = string; whitespace(*start); start++);

  if (*start == '\0')
    return (start);

  // Busca el fin de la cadena arrancando desde s para mayor optimización por menor recorrido
  end = start + strlen(start) - 1;

  while (end > start && whitespace(*end))
    end--;

  *++end = '\0';

  return start;
}

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