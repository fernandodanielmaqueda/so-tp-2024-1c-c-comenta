#include "console.h"


t_log *CONSOLE_LOGGER;
char *CONSOLE_LOG_PATHNAME = "console.log";

t_Command CONSOLE_COMMANDS[] = {
  { .name = "EJECUTAR_SCRIPT", .function = kernel_command_run_script },
  { .name = "INICIAR_PROCESO", .function = kernel_command_start_process },
  { .name = "FINALIZAR_PROCESO", .function = kernel_command_kill_process },
  { .name = "DETENER_PLANIFICACION", .function = kernel_command_pause_scheduling },
  { .name = "INICIAR_PLANIFICACION", .function = kernel_command_resume_scheduling },
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

    t_Arguments *arguments = arguments_create(MAX_CONSOLE_ARGC);
    if(arguments == NULL) {
        log_error(CONSOLE_LOGGER, "arguments_create: Error al reservar memoria para los argumentos");
        exit(EXIT_FAILURE);
    }

    if(arguments_use(arguments, line)) {
        switch(errno) {
            case E2BIG:
                log_error(CONSOLE_LOGGER, "%s: Demasiados argumentos en el comando", line);
                break;
            case ENOMEM:
                log_error(CONSOLE_LOGGER, "arguments_use: Error al reservar memoria para los argumentos");
                exit(EXIT_FAILURE);
            default:
                log_error(CONSOLE_LOGGER, "arguments_use: %s", strerror(errno));
                break;
        }
        arguments_destroy(arguments);
        return 1;
    }

    t_Command *command = find_command(arguments->argv[0]);
    if (command == NULL) {
        log_warning(CONSOLE_LOGGER, "%s: No existe el comando especificado.", arguments->argv[0]);
        arguments_destroy(arguments);
        return 1;
    }

    // Call the function
    int exit_status = ((*(command->function)) (arguments->argc, arguments->argv));
    arguments_destroy(arguments);
    return exit_status;
}

t_Command *find_command (char *name) {
    for(register int i = 0; CONSOLE_COMMANDS[i].name != NULL; i++)
        if(strcmp(CONSOLE_COMMANDS[i].name, name) == 0)
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

    send_process_create(argv[1], pcb->exec_context.PID, CONNECTION_MEMORY.fd_connection);

    t_Return_Value return_value;
    receive_return_value_with_expected_header(PROCESS_CREATE_HEADER, &return_value, CONNECTION_MEMORY.fd_connection);
    if(return_value) {
        log_warning(MODULE_LOGGER, "[Memoria]: No se pudo INICIAR_PROCESO %s", argv[1]);
        return 1;
    }

    wait_draining_requests(&SCHEDULING_SYNC);
        list_add(SHARED_LIST_NEW.list, pcb);
    signal_draining_requests(&SCHEDULING_SYNC);

    log_debug(MINIMAL_LOGGER, "Se crea el proceso <%d> en NEW", pcb->exec_context.PID);

    sem_post(&SEM_LONG_TERM_SCHEDULER_NEW);

    return 0;
}

int kernel_command_kill_process(int argc, char* argv[]) {

    if(argc != 2) {
        log_warning(CONSOLE_LOGGER, "Uso: FINALIZAR_PROCESO <PID>");
        return 1;
    }

    log_trace(CONSOLE_LOGGER, "FINALIZAR_PROCESO %s", argv[1]);

    char *end;
    t_PID pid = (t_PID) strtoul(argv[1], &end, 10);
    if(!*(argv[1]) || *end) {
        log_error(MODULE_LOGGER, "%s: No es un PID valido", argv[1]);
        return 1;
    }

    if(pid >= PID_COUNTER) {
        log_error(MODULE_LOGGER, "No existe un proceso con PID <%d>", (int) pid);
        return 1;
    }

    wait_ongoing(&SCHEDULING_SYNC);

        t_PCB *pcb = PCB_ARRAY[pid];
        if(pcb == NULL) {
            log_error(MODULE_LOGGER, "No existe un proceso con PID <%d>", (int) pid);
            signal_ongoing(&SCHEDULING_SYNC);
            return 1;
        }

        switch(pcb->current_state) {
            case NEW_STATE:
                pcb->exec_context.exit_reason = INTERRUPTED_BY_USER_EXIT_REASON;
                switch_process_state(pcb, EXIT_STATE);
                break;

            case READY_STATE:
                pcb->exec_context.exit_reason = INTERRUPTED_BY_USER_EXIT_REASON;
                switch_process_state(pcb, EXIT_STATE);
                break;

            case EXEC_STATE:
                pthread_mutex_lock(&MUTEX_KILL_EXEC_PROCESS);
                    KILL_EXEC_PROCESS = 1;
                pthread_mutex_unlock(&MUTEX_KILL_EXEC_PROCESS);
                send_kernel_interrupt(KILL_KERNEL_INTERRUPT, pcb->exec_context.PID, CONNECTION_CPU_INTERRUPT.fd_connection);
                break;

            case BLOCKED_STATE:
                pcb->exec_context.exit_reason = INTERRUPTED_BY_USER_EXIT_REASON;
                switch_process_state(pcb, EXIT_STATE);
                break;

            case EXIT_STATE:
                break;
        }

    signal_ongoing(&SCHEDULING_SYNC);

    return 0;
}

int kernel_command_pause_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: DETENER_PLANIFICACION");
        return 1;
    }

    pthread_mutex_lock(&MUTEX_SCHEDULING_PAUSED);
        if(SCHEDULING_PAUSED) {
            pthread_mutex_unlock(&MUTEX_SCHEDULING_PAUSED);
            log_trace(CONSOLE_LOGGER, "DETENER_PLANIFICACION sin efecto.");
            return 0;
        }

        SCHEDULING_PAUSED = 1;
    pthread_mutex_unlock(&MUTEX_SCHEDULING_PAUSED);

    log_trace(CONSOLE_LOGGER, "DETENER_PLANIFICACION");

    wait_ongoing(&SCHEDULING_SYNC);
    return 0;
}

int kernel_command_resume_scheduling(int argc, char* argv[]) {

    if(argc != 1) {
        log_warning(CONSOLE_LOGGER, "Uso: INICIAR_PLANIFICACION");
        return 1;
    }

    pthread_mutex_lock(&MUTEX_SCHEDULING_PAUSED);
        if(!SCHEDULING_PAUSED) {
            pthread_mutex_unlock(&MUTEX_SCHEDULING_PAUSED);
            log_trace(CONSOLE_LOGGER, "INICIAR_PLANIFICACION sin efecto.");
            return 0;
        }

        SCHEDULING_PAUSED = 0;
    pthread_mutex_unlock(&MUTEX_SCHEDULING_PAUSED);

    log_trace(CONSOLE_LOGGER, "INICIAR_PLANIFICACION");

    signal_ongoing(&SCHEDULING_SYNC);
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
                if(errno != EAGAIN) {
                    log_warning(CONSOLE_LOGGER, "sem_trywait: %s", strerror(errno));
                    return 1;
                }

                pthread_mutex_lock(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
                MULTIPROGRAMMING_DIFFERENCE+= i;
                pthread_mutex_unlock(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
                sem_post(&SEM_MULTIPROGRAMMING_POSTER);
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

    char *pid_string_new = string_new();
    char *pid_string_ready = string_new();
    char *pid_string_exec = string_new();
    char *pid_string_blocked = string_new();
    char *pid_string_exit = string_new();

    wait_ongoing(&SCHEDULING_SYNC);
        pcb_list_to_pid_string(SHARED_LIST_NEW.list, &pid_string_new);

        pcb_list_to_pid_string(SHARED_LIST_READY.list, &pid_string_ready);
        pcb_list_to_pid_string(SHARED_LIST_READY_PRIORITARY.list, &pid_string_ready);

        pcb_list_to_pid_string(SHARED_LIST_EXEC.list, &pid_string_exec);

        pcb_list_to_pid_string(SHARED_LIST_BLOCKED.list, &pid_string_blocked);

        pcb_list_to_pid_string(SHARED_LIST_EXIT.list, &pid_string_exit);
    signal_ongoing(&SCHEDULING_SYNC);

    log_info(CONSOLE_LOGGER,
        "Lista de proceso por estado:\n"
        "* NEW: [%s]\n"
        "* READY: [%s]\n"
        "* EXEC: [%s]\n"
        "* BLOCKED: [%s]\n"
        "* EXIT: [%s]"
        , pid_string_new
        , pid_string_ready
        , pid_string_exec
        , pid_string_blocked
        , pid_string_exit
    );

    free(pid_string_new);
    free(pid_string_ready);
    free(pid_string_exec);
    free(pid_string_blocked);
    free(pid_string_exit);

    return 0;
}