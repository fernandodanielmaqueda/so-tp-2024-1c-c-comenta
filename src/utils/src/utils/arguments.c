/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/arguments.h"

t_Arguments *arguments_create(int max_argc, bool duplicate_lines) {
  t_Arguments *arguments = malloc(sizeof(t_Arguments));
  if(arguments == NULL) {
      log_error(MODULE_LOGGER, "No se pudo reservar memoria para los argumentos.");
      exit(EXIT_FAILURE);
  }

  arguments->argc = 0;
  arguments->argv = NULL;
  *(int *)&(arguments->max_argc) = max_argc;
  *(bool *)&(arguments->duplicate_lines) = duplicate_lines;

  return arguments;
}

void arguments_add(t_Arguments *arguments, char *line) {

  char *subline = strip_whitespaces(line);

  char *work_line = subline;
  if(arguments->duplicate_lines) {
    if((work_line = strdup(subline)) == NULL) {
        log_error(MODULE_LOGGER, "No se pudo duplicar la línea de argumentos.");
        free(arguments);
        exit(EXIT_FAILURE);
    }
  }

  register int i = 0;
  char **new_argv;

  while(work_line[i]) {
    while(work_line[i] && isspace(work_line[i]))
      i++;

    if(!work_line[i])
      break;
    
    if(arguments->argc == arguments->max_argc) {
      log_error(MODULE_LOGGER, "Se excedió la cantidad máxima de argumentos permitidos.");
      if (arguments->duplicate_lines)
        free(work_line);
      free(arguments->argv);
      free(arguments);
      exit(EXIT_FAILURE);
    }

    new_argv = realloc(arguments->argv, ++arguments->argc * sizeof(char *));
    if(new_argv == NULL) {
      log_error(MODULE_LOGGER, "No se pudo reservar memoria para los argumentos.");
      if (arguments->duplicate_lines)
        free(work_line);
      free(arguments->argv);
      free(arguments);
      exit(EXIT_FAILURE);
    }
    arguments->argv = new_argv;
    arguments->argv[arguments->argc - 1] = work_line + i;

    while(work_line[i] && !isspace(work_line[i]))
        i++;

    if(work_line[i])
        work_line[i++] = '\0';
  }
}

void arguments_remove(t_Arguments *arguments) {
  if(arguments == NULL)
      return;

  if(arguments->duplicate_lines)
    if(arguments->argv != NULL)
      free(*(arguments->argv));

  free(arguments->argv);
  arguments->argc = 0;
  arguments->argv = NULL;
}

void arguments_destroy(t_Arguments *arguments) {
  arguments_remove(arguments);
  free(arguments);
}

/* Strip whitespace from the start and end of STRING.  Return a pointer into STRING. */
char *strip_whitespaces(char *string) {
  register char *start, *end;

  // Recorre el inicio de cadena hasta encontrar un caracter que no sea un espacio
  for (start = string; isspace(*start); start++);

  if (*start == '\0')
    return (start);

  // Busca el fin de la cadena arrancando desde s para mayor optimización por menor recorrido
  end = start + strlen(start) - 1;

  while (end > start && isspace(*end))
    end--;

  *++end = '\0';

  return start;
}