/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "commons/collections/dictionary.h"
#include <commons/temporal.h>
#include "utils/module.h"
#include "utils/send.h"
#include "utils/socket.h"
#include "console.h"
#include "socket.h"

typedef struct t_Drain_Ongoing_Resource_Sync {
	pthread_mutex_t mutex_resource;
	sem_t sem_drain_requests_count;
	pthread_cond_t cond_drain_requests;
	sem_t sem_ongoing_count;
	pthread_cond_t cond_ongoing;
} t_Drain_Ongoing_Resource_Sync;

typedef enum e_Process_State {
    NEW_STATE,
    READY_STATE,
    EXEC_STATE,
    BLOCKED_STATE,
	EXIT_STATE
} e_Process_State;

typedef struct t_PCB {
    t_Exec_Context exec_context;

    e_Process_State current_state;
    t_Shared_List *shared_list_state;

    t_Payload *instruction;

    e_Exit_Reason exit_reason;
} t_PCB;

#include "scheduler.h"
#include "resources.h"
#include "interfaces.h"
#include "syscalls.h"

extern char *MODULE_NAME;

extern t_log *MODULE_LOGGER;
extern char *MINIMAL_LOG_PATHNAME;

extern t_config *MODULE_CONFIG;
extern char *MODULE_CONFIG_PATHNAME;

int module(int, char*[]);
void initialize_mutexes(void);
void finish_mutexes(void);
void initialize_semaphores(void);
void finish_semaphores(void);
void read_module_config(t_config *module_config);
void initialize_cpu_command_line_interface(void);
void *receptor_mensajes_cpu(void*);
void listen_cpu(int fd_cpu) ;

#endif /* KERNEL_H */