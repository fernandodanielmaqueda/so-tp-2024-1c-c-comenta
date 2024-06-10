#include "console.h"

t_log *CONSOLE_LOGGER;
char *CONSOLE_LOG_PATHNAME = "console.log";

t_Command CONSOLE_COMMANDS[] = {
  { "EJECUTAR_SCRIPT", kernel_command_run_script, "Change to directory DIR" },
  { "INICIAR_PROCESO", kernel_command_start_process, "Delete FILE" },
  { "FINALIZAR_PROCESO", kernel_command_kill_process, "Display this text" },
  { "DETENER_PLANIFICACION", kernel_command_stop_scheduling, "Synonym for `help'" },
  { "INICIAR_PLANIFICACION", kernel_command_start_scheduling, "List files in DIR" },
  { "MULTIPROGRAMACION", kernel_command_multiprogramming, "Synonym for `list'" },
  { "PROCESO_ESTADO", kernel_command_process_states, "Print the current working directory" },
  { NULL, NULL, NULL }
};

int EXIT = 0;

void *initialize_kernel_console(void *argument) {

    CONSOLE_LOGGER = log_create(CONSOLE_LOG_PATHNAME, "Console", true, LOG_LEVEL_TRACE);

    char *line, *subline;

    while(!EXIT) {
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
int execute_line(char *line) {
    register int i = 0;
    char *arg;
    int argc = 0;
    char *argv[MAX_CONSOLE_ARGS] = {NULL};
    t_Command *command;

    while(line[i]) {
        while(line[i] && whitespace(line[i]))
            i++;

        if(!line[i])
            break;

        arg = line + i;
        argv[argc++] = arg;

        while(line[i] && !whitespace(line[i]))
            i++;

        if(line[i])
            line[i++] = '\0';
    }

    command = find_command(argv[0]);

    if (command == NULL) {
        fprintf(stderr, "%s: No such command.\n", argv[0]);
        log_error(CONSOLE_LOGGER, "%s: No existe el comando especificado.", argv[0]);
        return 0;
    }

    /* Call the function. */
    ((*(command->function)) (argc, argv));

    return 0;
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
    //char* path_script = strdup(command_console[1]);
    //log_trace(CONSOLE_LOGGER, "Se va a ejecutar el script %s", path_script);
    //TODO: ejecutar_script(path);==========================//

    log_trace(CONSOLE_LOGGER, "Ejecutar script");
    return 0;    
}

int kernel_command_start_process(int argc, char* argv[]) {

    // strdup: duplica la cadena de caracteres y le asigna memoria sufuente
    //char* path = strdup(command_console[1]);
    //log_trace(CONSOLE_LOGGER, "Se va a iniciar el proceso %s", path);

    //TODO: iniciar_proceso(path);==========================//
    return 0;
}
int kernel_command_kill_process(int argc, char* argv[]) {
    //int pid = atoi(command_console[1]);
    //NECESITO EL PCB PARA SEGUIR ACA =-=======================//
    //log_trace(CONSOLE_LOGGER, "Se fianlizo el proceso %d", pid);
    return 0;
}

int kernel_command_stop_scheduling(int argc, char* argv[]) {
    //log_trace(CONSOLE_LOGGER, "Se  detuvo la planificacion");
    //stop_planificacion();
    return 0;
}

int kernel_command_start_scheduling(int argc, char* argv[]) {
    //log_trace(CONSOLE_LOGGER, "Se  inicio la planificacion");
    //init_planificacion();
    return 0;
}

int kernel_command_multiprogramming(int argc, char* argv[]) {
    //int nuevo_grado = atoi(command_console[1]);
    //log_trace(CONSOLE_LOGGER, " Grado Actual: %d",  nuevo_grado);   
    //TODO:crear_nuevo_semaforo_multiprog==========================//
    return 0;
}

int kernel_command_process_states(int argc, char* argv[]) {
    // listState();
    return 0;
}