#include "console.h"

t_log *CONSOLE_LOGGER;
char *CONSOLE_LOG_PATHNAME = "console.log";

t_Command CONSOLE_COMMANDS[] = {
  { "EJECUTAR_SCRIPT", kernel_command_run_script },
  { "INICIAR_PROCESO", kernel_command_start_process },
  { "FINALIZAR_PROCESO", kernel_command_kill_process },
  { "DETENER_PLANIFICACION", kernel_command_stop_scheduling },
  { "INICIAR_PLANIFICACION", kernel_command_start_scheduling },
  { "MULTIPROGRAMACION", kernel_command_multiprogramming },
  { "PROCESO_ESTADO", kernel_command_process_states },
  { NULL, NULL }
};

int EXIT_CONSOLE = 0;

void *initialize_kernel_console(void *argument) {

    CONSOLE_LOGGER = log_create(CONSOLE_LOG_PATHNAME, "Console", true, LOG_LEVEL_TRACE);

    char *line, *subline;

    initialize_readline();	/* Bind our completer. */

    while(!EXIT_CONSOLE) {
        line = readline("> ");

        if(line == NULL)
            continue;

        subline = strip_whitespaces(line);

        if(*subline) {
            add_history(subline);
            execute_line(subline);
        }

        free(line);
    }

    clear_history();
    return NULL;
}

/* Tell the GNU Readline library how to complete.  We want to try to complete on command names if this is the first word in the line, or on filenames if not. */
void initialize_readline(void) {
  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = console_completion;
}

/* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or NULL if there aren't any. */
char **console_completion(const char *text, int start, int end) {
    char **matches = NULL;

    /* If this word is at the start of the line, then it is a command
        to complete.  Otherwise it is the name of a file in the current
        directory. */
    if(start == 0)
        matches = rl_completion_matches(text, command_generator);

    return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char *command_generator(const char *text, int state) {
    static int list_index, length;
    char *name;

    /* If this is a new word to complete, initialize now.  This includes
        saving the length of TEXT for efficiency, and initializing the index
        variable to 0. */
    if(!state) {
        list_index = 0;
        length = strlen(text);
    }

    /* Return the next name which partially matches from the command list. */
    while ((name = CONSOLE_COMMANDS[list_index].name) != NULL) {
        list_index++;

        if(strncmp(name, text, length) == 0)
            return(strdup(name));
    }

    /* If no names matched, then return NULL. */
    return NULL;
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

/* Execute a command line. */
void execute_line(char *line) {
    register int i = 0;
    char *arg;
    int argc = 0;
    char *argv[MAX_CONSOLE_ARGC] = {NULL};
    t_Command *command;

    while(line[i]) {
        while(line[i] && whitespace(line[i]))
            i++;

        if(!line[i])
            break;

        if(argc == MAX_CONSOLE_ARGC) {
            log_warning(CONSOLE_LOGGER, "Demasiados argumentos.");
            return;
        }

        arg = line + i;
        argv[argc++] = arg;

        while(line[i] && !whitespace(line[i]))
            i++;

        if(line[i])
            line[i++] = '\0';
    }

    command = find_command(argv[0]);

    if (command == NULL) {
        log_warning(CONSOLE_LOGGER, "%s: No existe el comando especificado.", argv[0]);
        return;
    }

    /* Call the function. */
    ((*(command->function)) (argc, argv));

    return;
}

/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name. */
t_Command *find_command (char *name) {
    register int i;

    for(i = 0; CONSOLE_COMMANDS[i].name != NULL; i++)
        if(strcmp(name, CONSOLE_COMMANDS[i].name) == 0)
            return (&CONSOLE_COMMANDS[i]);

    return NULL;
}

//initialize_readline

int kernel_command_run_script(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: EJECUTAR_SCRIPT <PATH>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "EJECUTAR_SCRIPT %s", argv[1]);

    return 0;    
}

int kernel_command_start_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PROCESO <PATH>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "INICIAR_PROCESO %s", argv[1]);

    return 0;
}

int kernel_command_kill_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: FINALIZAR_PROCESO <PID>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "FINALIZAR_PROCESO %s", argv[1]);

    return 0;
}

int kernel_command_stop_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: DETENER_PLANIFICACION");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "DETENER_PLANIFICACION");

    return 0;
}

int kernel_command_start_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PLANIFICACION");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "INICIAR_PLANIFICACION");

    return 0;
}

int kernel_command_multiprogramming(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: MULTIPROGRAMACION <VALOR>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "MULTIPROGRAMACION %s", argv[1]);

    return 0;
}

int kernel_command_process_states(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: PROCESO_ESTADO");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "PROCESO_ESTADO");

    return 0;
}