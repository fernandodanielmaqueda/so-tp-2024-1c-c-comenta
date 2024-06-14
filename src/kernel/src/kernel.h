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
#include "utils/socket.h"
#include "console.h"
#include "socket.h"

typedef struct t_Scheduling_Algorithm {
    char *name;
    t_PCB *(*function) (void);
} t_Scheduling_Algorithm;

extern char *MODULE_NAME;

extern t_log *MODULE_LOGGER;
extern char *MODULE_LOG_PATHNAME;

extern t_config *MODULE_CONFIG;
extern char *MODULE_CONFIG_PATHNAME;

// Listas globales de estados
extern t_list *LIST_NEW;
extern t_list *LIST_READY;
extern t_list *LIST_READY_PRIORITARY;
extern t_list *LIST_EXECUTING;
extern t_list *LIST_BLOCKED;
extern t_list *LIST_EXIT;

extern t_list *START_PROCESS;
extern pthread_mutex_t MUTEX_LIST_START_PROCESS;

extern pthread_mutex_t mutex_PID;
extern pthread_mutex_t mutex_LIST_NEW;
extern pthread_mutex_t mutex_LIST_READY;
extern pthread_mutex_t mutex_LIST_BLOCKED;
extern pthread_mutex_t mutex_LIST_EXECUTING;
extern pthread_mutex_t mutex_LIST_EXIT;

//consola interactiva
extern pthread_mutex_t mutex_pid_detected;
extern int identifier_pid;
//

extern pthread_t hilo_largo_plazo;
extern pthread_t hilo_corto_plazo;
extern pthread_t hilo_mensajes_cpu;
extern pthread_t thread_interrupt;

extern sem_t SEM_LONG_TERM_SCHEDULER;
extern sem_t sem_short_term_scheduler;
extern sem_t sem_multiprogramming_level;
extern sem_t process_ready;

extern t_Scheduling_Algorithm *SCHEDULING_ALGORITHM;
extern int QUANTUM;
extern char **RESOURCES;
extern char **RESOURCE_INSTANCES;
extern int MULTIPROGRAMMING_LEVEL;
extern int pidContador;

int module(int, char*[]);
void read_module_config(t_config *module_config);
t_Scheduling_Algorithm *find_scheduling_algorithm(char *name);
void switch_process_state(t_PCB* pcb, int new_state) ;
t_PCB *create_pcb();
void initialize_long_term_scheduler(void);
void initialize_short_term_scheduler(void);
void initialize_cpu_command_line_interface(void);
void *long_term_scheduler(void*);
void *short_term_scheduler(void*);
t_PCB *FIFO_scheduling_algorithm(void);
t_PCB *RR_scheduling_algorithm(void);
t_PCB *VRR_scheduling_algorithm(void);
t_PCB *kernel_get_normal_list(void);
t_PCB *kernel_get_priority_list(void);
void *receptor_mensajes_cpu(void*);
int current_time(void);
int asignar_PID();
void* start_quantum_VRR(t_PCB *pcb);
void* start_quantum();
void listen_cpu(int fd_cpu) ;

#endif /* KERNEL_H */