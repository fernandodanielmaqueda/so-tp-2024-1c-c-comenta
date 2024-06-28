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
        return 1;
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
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "EJECUTAR_SCRIPT %s", argv[1]);

    FILE *script = fopen(argv[1], "r");
    if(script == NULL) {
        log_warning(CONSOLE_LOGGER, "%s: No se pudo abrir el <PATH>: %s", argv[1], strerror(errno));
        return 1;
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
                return 1;
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

    return 0;
}

int kernel_command_start_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PROCESO <PATH (EN MEMORIA)>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "INICIAR_PROCESO %s", argv[1]);

    t_PCB *pcb = pcb_create();

    send_process_create(argv[1], pcb->PID, CONNECTION_MEMORY.fd_connection);

    t_Return_Value return_value;
    receive_return_value_with_header(PROCESS_CREATE_HEADER, &return_value, CONNECTION_MEMORY.fd_connection);
    if(return_value) {
        log_warning(MODULE_LOGGER, "[Memoria]: No se pudo INICIAR_PROCESO %s", argv[1]);
        return 1;
    }

    wait_list_process_states();
        pthread_mutex_lock(&MUTEX_LIST_NEW);
            list_add(LIST_NEW, pcb);
        pthread_mutex_unlock(&MUTEX_LIST_NEW);
    signal_list_process_states();

    log_info(MINIMAL_LOGGER, "Se crea el proceso <%d> en NEW", pcb->PID);

    sem_post(&SEM_LONG_TERM_SCHEDULER_NEW);

    return 0;
}

int kernel_command_kill_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: FINALIZAR_PROCESO <PID>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "FINALIZAR_PROCESO %s", argv[1]);

    // TODO: Implementación

    return 0;
}

int kernel_command_stop_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: DETENER_PLANIFICACION");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "DETENER_PLANIFICACION");

    // TODO: Implementación

    return 0;
}

int kernel_command_start_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PLANIFICACION");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "INICIAR_PLANIFICACION");

    // TODO: Implementación

    return 0;
}

int kernel_command_multiprogramming(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: MULTIPROGRAMACION <VALOR>");
        return 1;
    }

    int value = atoi(argv[1]);
    if(value < 0) {
        log_warning(CONSOLE_LOGGER, "MULTIPROGRAMACION: El valor debe ser mayor o igual a 0.");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "MULTIPROGRAMACION %s", argv[1]);

    if (value >= MULTIPROGRAMMING_LEVEL) {
        for(register int i = value - MULTIPROGRAMMING_LEVEL; i > 0; i--)
            sem_post(&SEM_MULTIPROGRAMMING_LEVEL);
        
    } else
        for(register int i = MULTIPROGRAMMING_LEVEL - value; i > 0; i--)
            if(sem_trywait(&SEM_MULTIPROGRAMMING_LEVEL)) {
                if(errno == EAGAIN) {
                    pthread_mutex_lock(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
                        MULTIPROGRAMMING_DIFFERENCE+= i;
                    pthread_mutex_unlock(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
                    sem_post(&SEM_MULTIPROGRAMMING_POSTER);
                } else {
                    log_warning(CONSOLE_LOGGER, "sem_trywait: %s", strerror(errno));
                    return 1;
                }
            }

    MULTIPROGRAMMING_LEVEL = value;

    return 0;
}

int kernel_command_process_states(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: PROCESO_ESTADO");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "PROCESO_ESTADO");

    pthread_mutex_lock(&MUTEX_LIST_PROCESS_STATES);
        LIST_PROCESS_STATES = 1;
    pthread_mutex_unlock(&MUTEX_LIST_PROCESS_STATES);

    int sem_value;
    pthread_mutex_lock(&MUTEX_LIST_PROCESS_STATES);
    while(1) {
        sem_getvalue(&SEM_SWITCHING_STATES_COUNT, &sem_value);
        if(!sem_value)
            break;
        pthread_cond_wait(&COND_SWITCHING_STATES, &MUTEX_LIST_PROCESS_STATES);
    }
    pthread_mutex_unlock(&MUTEX_LIST_PROCESS_STATES);

        char *pid_string_new = pcb_list_to_pid_string(LIST_NEW);
        char *pid_string_ready = pcb_list_to_pid_string(LIST_READY);
        char *pid_string_executing = pcb_list_to_pid_string(LIST_EXECUTING);
        char *pid_string_blocked = pcb_list_to_pid_string(LIST_BLOCKED);
        char *pid_string_exit = pcb_list_to_pid_string(LIST_EXIT);

    pthread_mutex_lock(&MUTEX_LIST_PROCESS_STATES);
        LIST_PROCESS_STATES = 0;
        pthread_cond_broadcast(&COND_LIST_PROCESS_STATES);
    pthread_mutex_unlock(&MUTEX_LIST_PROCESS_STATES);

    log_info(CONSOLE_LOGGER,
        "Lista de proceso por estado:\n"
        "* NEW: %s\n"
        "* READY: %s\n"
        "* EXEC: %s\n"
        "* BLOCKED: %s\n"
        "* EXIT: %s"
        , pid_string_new
        , pid_string_ready
        , pid_string_executing
        , pid_string_blocked
        , pid_string_exit
    );

    free(pid_string_new);
    free(pid_string_ready);
    free(pid_string_executing);
    free(pid_string_blocked);
    free(pid_string_exit);

    return 0;
}

char *pcb_list_to_pid_string(t_list *pcb_list) {
    char *string = string_new();
    char *pid_as_string;
    string_append(&string, "[");
    for(register int i = 0; i < list_size(pcb_list); i++) {
        pid_as_string = string_from_format("%" PRIu32, ((t_PCB *) list_get(pcb_list, i))->PID);
        string_append(&string, pid_as_string);
        free(pid_as_string);
        if(i < list_size(pcb_list) - 1)
            string_append(&string, " , ");
    }
    string_append(&string, "]");
    return string;
}