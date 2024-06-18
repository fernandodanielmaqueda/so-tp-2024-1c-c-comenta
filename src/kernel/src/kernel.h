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
#include "utils/serialize/pcb.h"
#include "utils/serialize/eviction_reason.h"
#include "utils/serialize/cpu_opcode.h"
#include "utils/socket.h"
#include "console.h"
#include "socket.h"
#include "scheduler.h"
#include "resources.h"
#include "syscalls.h"

extern char *MODULE_NAME;

extern t_log *MODULE_LOGGER;
extern char *MINIMAL_LOG_PATHNAME;

extern t_config *MODULE_CONFIG;
extern char *MODULE_CONFIG_PATHNAME;

int module(int, char*[]);
void read_module_config(t_config *module_config);
void initialize_cpu_command_line_interface(void);
void *receptor_mensajes_cpu(void*);
void listen_cpu(int fd_cpu) ;

#endif /* KERNEL_H */