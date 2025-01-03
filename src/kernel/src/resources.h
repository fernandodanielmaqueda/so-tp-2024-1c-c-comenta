/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef KERNEL_RESOURCES_H
#define KERNEL_RESOURCES_H

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
#include "utils/serialize/exec_context.h"
#include "utils/socket.h"
#include "console.h"
#include "socket.h"
#include "scheduler.h"

typedef struct t_Resource {
    long instances;
    char *name;
    pthread_mutex_t mutex_instances;
    t_Shared_List shared_list_blocked;
} t_Resource;

extern int RESOURCE_QUANTITY;
extern t_Resource *RESOURCES;

void resources_read_module_config(t_config *module_config);
t_Resource *resource_find(char *name);
void resource_log(t_Resource *resource);
void resources_free(void);

#endif // KERNEL_RESOURCES_H