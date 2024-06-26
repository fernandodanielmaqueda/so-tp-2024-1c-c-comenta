#include "console.h"


t_log *CONSOLE_LOGGER;
char *CONSOLE_LOG_PATHNAME = "console.log";

t_Command CONSOLE_COMMANDS[] = {
  { .name = "EJECUTAR_SCRIPT", .function = kernel_command_run_script },
  { .name = "INICIAR_PROCESO", .function = kernel_command_start_process },
  { .name = "FINALIZAR_PROCESO", .function = kernel_command_kill_process },
  { .name = "DETENER_PLANIFICACION", .function = kernel_command_stop_scheduling },
  { .name = "INICIAR_PLANIFICACION", .function = kernel_command_start_scheduling },
  { .name = "MULTIPROGRAMACION", .function = kernel_command_multiprogramming },
  { .name = "PROCESO_ESTADO", .function = kernel_command_process_states },
  { .name = NULL, .function = NULL }
};

int EXIT_CONSOLE = 0;

void *initialize_kernel_console(void *argument) {

    CONSOLE_LOGGER = log_create(CONSOLE_LOG_PATHNAME, "Console", true, LOG_LEVEL_TRACE);

    char *line, *subline;

    initialize_readline();

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

/* Execute a command line. */
int execute_line(char *line) {

    t_Arguments *arguments = arguments_create(MAX_CONSOLE_ARGC, false);
    arguments_add(arguments, line);

    t_Command *command = find_command(arguments->argv[0]);
    if (command == NULL) {
        log_warning(CONSOLE_LOGGER, "%s: No existe el comando especificado.", arguments->argv[0]);
        return EXIT_FAILURE;
    }

    // Call the function
    return ((*(command->function)) (arguments->argc, arguments->argv));
}

t_Command *find_command (char *name) {
    for(register int i = 0; CONSOLE_COMMANDS[i].name != NULL; i++)
        if(!strcmp(CONSOLE_COMMANDS[i].name, name))
            return (&CONSOLE_COMMANDS[i]);

    return NULL;
}

int kernel_command_run_script(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: EJECUTAR_SCRIPT <PATH (EN KERNEL)>");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "EJECUTAR_SCRIPT %s", argv[1]);

    FILE *script = fopen(argv[1], "r");
    if(script == NULL) {
        log_warning(CONSOLE_LOGGER, "%s: No se pudo abrir el <PATH>: %s", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    char *line = NULL, *subline;
    size_t length;
    ssize_t nread;
    
    while(1) {
        nread = getline(&line, &length, script);

        if(nread == -1) {
            if(errno) {
                log_warning(CONSOLE_LOGGER, "Funcion getline: %s", strerror(errno));
                free(line);
                return EXIT_FAILURE;
            }

            break;
        }

        subline = strip_whitespaces(line);

        if(*subline)
            if(execute_line(subline))
                break;
        
    }

    free(line);
    fclose(script);

    return EXIT_SUCCESS;
}

int kernel_command_start_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PROCESO <PATH (EN MEMORIA)>");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "INICIAR_PROCESO %s", argv[1]);

    pthread_mutex_lock(&MUTEX_LIST_START_PROCESS);
        list_add(START_PROCESS, strdup(argv[1]));
    pthread_mutex_unlock(&MUTEX_LIST_START_PROCESS);

    sem_post(&SEM_LONG_TERM_SCHEDULER_NEW);

    return EXIT_SUCCESS;
}

int kernel_command_kill_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: FINALIZAR_PROCESO <PID>");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "FINALIZAR_PROCESO %s", argv[1]);

    // TODO: Implementación
    switch_process_state(atoi(argv[1]), EXIT_STATE);

    

    

    return EXIT_SUCCESS;
}

int kernel_command_stop_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: DETENER_PLANIFICACION");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "DETENER_PLANIFICACION");

    // TODO: Implementación

    return EXIT_SUCCESS;
}

int kernel_command_start_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PLANIFICACION");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "INICIAR_PLANIFICACION");

    // TODO: Implementación

    return EXIT_SUCCESS;
}

int kernel_command_multiprogramming(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: MULTIPROGRAMACION <VALOR>");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "MULTIPROGRAMACION %s", argv[1]);

    // TODO: Implementación

    return EXIT_SUCCESS;
}

int kernel_command_process_states(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: PROCESO_ESTADO");
        return EXIT_FAILURE;
    }

    log_trace(CONSOLE_LOGGER, "PROCESO_ESTADO");

    // TODO: Implementación

    return EXIT_SUCCESS;
}