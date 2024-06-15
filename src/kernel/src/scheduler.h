
#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

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
#include "utils/module.h"
#include "utils/socket.h"
#include "kernel.h"

typedef struct t_Scheduling_Algorithm {
    char *name;
    t_PCB *(*function_fetcher) (void);
    t_PCB *(*function_reprogrammer) (t_PCB *);
} t_Scheduling_Algorithm;

extern t_Scheduling_Algorithm *SCHEDULING_ALGORITHM;

extern t_list *START_PROCESS;
extern pthread_mutex_t MUTEX_LIST_START_PROCESS;

// Listas globales de estados
extern t_list *LIST_NEW;
extern t_list *LIST_READY;
extern t_list *LIST_READY_PRIORITARY;
extern t_list *LIST_EXECUTING;
extern t_list *LIST_BLOCKED;
extern t_list *LIST_EXIT;

extern pthread_mutex_t mutex_PID;
extern pthread_mutex_t mutex_LIST_NEW;
extern pthread_mutex_t mutex_LIST_READY;
extern pthread_mutex_t MUTEX_LIST_READY_PRIORITARY;
extern pthread_mutex_t mutex_LIST_BLOCKED;
extern pthread_mutex_t mutex_LIST_EXECUTING;
extern pthread_mutex_t mutex_LIST_EXIT;

extern sem_t sem_detener_execute;
extern sem_t sem_detener_new_ready;
extern sem_t sem_detener_block_ready;
extern sem_t sem_detener_block;
extern sem_t sem_detener_planificacion;

extern pthread_t THREAD_LONG_TERM_SCHEDULER;
extern pthread_t THREAD_SHORT_TERM_SCHEDULER;
extern pthread_t hilo_mensajes_cpu;
extern pthread_t THREAD_INTERRUPT;

extern sem_t SEM_LONG_TERM_SCHEDULER;
extern sem_t SEM_SHORT_TERM_SCHEDULER;
extern sem_t SEM_MULTIPROGRAMMING_LEVEL;
extern sem_t SEM_PROCESS_READY;

extern int QUANTUM;
extern int MULTIPROGRAMMING_LEVEL;

//consola interactiva
extern pthread_mutex_t MUTEX_PID_DETECTED;
extern int IDENTIFIER_PID;
//

extern int PID_COUNTER;

t_Scheduling_Algorithm *find_scheduling_algorithm(char *name);
void initialize_long_term_scheduler(void);
void initialize_short_term_scheduler(void);
void *long_term_scheduler(void*);
void *short_term_scheduler(void*);
t_PCB *FIFO_scheduling_algorithm(void);
t_PCB *RR_scheduling_algorithm(void);
t_PCB *VRR_scheduling_algorithm(void);
t_PCB *FIFO_scheduling_reprogrammer(t_PCB *pcb);
t_PCB *RR_scheduling_reprogrammer(t_PCB *pcb);
t_PCB *VRR_scheduling_reprogrammer(t_PCB *pcb);
t_PCB *kernel_get_normal_list(void);
t_PCB *kernel_get_priority_list(void);
void switch_process_state(t_PCB* pcb, int new_state);
t_PCB *pcb_create();
int current_time(void);
int asignar_PID();
void* start_quantum_VRR(t_PCB *pcb);
void* start_quantum();
void stop_planificacion(void);
void init_planificacion(void);
void free_strv(char** array);

#endif // KERNEL_SCHEDULER_H