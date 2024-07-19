
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

typedef enum e_Scheduling_Algorithm {
    FIFO_SCHEDULING_ALGORITHM,
    RR_SCHEDULING_ALGORITHM,
    VRR_SCHEDULING_ALGORITHM
} e_Scheduling_Algorithm;

typedef struct t_Scheduling_Algorithm {
    char *name;
    t_PCB *(*function_fetcher) (void);
} t_Scheduling_Algorithm;

extern t_PID PID_COUNTER;
extern pthread_mutex_t MUTEX_PID_COUNTER;
extern t_PCB **PCB_ARRAY;
extern pthread_mutex_t MUTEX_PCB_ARRAY;
extern t_list *LIST_RELEASED_PIDS; // LIFO
extern pthread_mutex_t MUTEX_LIST_RELEASED_PIDS;
extern pthread_cond_t COND_LIST_RELEASED_PIDS;

extern const char *STATE_NAMES[];

extern t_Shared_List SHARED_LIST_NEW;
extern t_Shared_List SHARED_LIST_READY;
extern t_Shared_List SHARED_LIST_READY_PRIORITARY;
extern t_Shared_List SHARED_LIST_EXEC;
extern t_Shared_List SHARED_LIST_EXIT;

extern pthread_t THREAD_LONG_TERM_SCHEDULER_NEW;
extern sem_t SEM_LONG_TERM_SCHEDULER_NEW;
extern pthread_t THREAD_LONG_TERM_SCHEDULER_EXIT;
extern sem_t SEM_LONG_TERM_SCHEDULER_EXIT;
extern pthread_t THREAD_SHORT_TERM_SCHEDULER;
extern sem_t SEM_SHORT_TERM_SCHEDULER;

extern int EXEC_PCB;

extern t_Scheduling_Algorithm SCHEDULING_ALGORITHMS[];

extern e_Scheduling_Algorithm SCHEDULING_ALGORITHM;

extern t_Quantum QUANTUM;
extern pthread_t THREAD_QUANTUM_INTERRUPT;
extern pthread_mutex_t MUTEX_QUANTUM_INTERRUPT;
extern int QUANTUM_INTERRUPT;

extern t_temporal *TEMPORAL_DISPATCHED;

extern const char *EXIT_REASONS[];

extern int KILL_EXEC_PROCESS;
extern pthread_mutex_t MUTEX_KILL_EXEC_PROCESS;

extern unsigned int MULTIPROGRAMMING_LEVEL;
extern sem_t SEM_MULTIPROGRAMMING_LEVEL;
extern unsigned int MULTIPROGRAMMING_DIFFERENCE;
extern pthread_mutex_t MUTEX_MULTIPROGRAMMING_DIFFERENCE;
extern sem_t SEM_MULTIPROGRAMMING_POSTER;
extern pthread_t THREAD_MULTIPROGRAMMING_POSTER;

extern int SCHEDULING_PAUSED;
extern pthread_mutex_t MUTEX_SCHEDULING_PAUSED;

extern t_Drain_Ongoing_Resource_Sync SCHEDULING_SYNC;

int find_scheduling_algorithm(char *name, e_Scheduling_Algorithm *destination);
void initialize_scheduling(void);
void pause_scheduling(void);
void resume_scheduling(void);
void finish_scheduling(void);
void initialize_long_term_scheduler(void);
void initialize_short_term_scheduler(void);
void *long_term_scheduler_new(void *NULL_parameter);
void *long_term_scheduler_exit(void *NULL_parameter);
void *short_term_scheduler(void *NULL_parameter);
void *multiprogramming_poster(void *NULL_argument);
t_PCB *FIFO_scheduling_algorithm(void);
t_PCB *RR_scheduling_algorithm(void);
t_PCB *VRR_scheduling_algorithm(void);
void switch_process_state(t_PCB* pcb, e_Process_State NEW_STATE);

bool pcb_matches_pid(t_PCB *pcb, t_PID *pid);

void log_state_list(t_log *logger, const char *state_name, t_list *pcb_list);
void pcb_list_to_pid_string(t_list *pcb_list, char **destination);
t_PCB *pcb_create(void);
t_PID pid_assign(t_PCB *pcb);
void pid_release(t_PID pid);
void* start_quantum(t_PCB *pcb);

void init_resource_sync(t_Drain_Ongoing_Resource_Sync *resource_sync);
void destroy_resource_sync(t_Drain_Ongoing_Resource_Sync *resource_sync);
void wait_ongoing(t_Drain_Ongoing_Resource_Sync *resource_sync);
void signal_ongoing(t_Drain_Ongoing_Resource_Sync *resource_sync);
void wait_ongoing_locking(t_Drain_Ongoing_Resource_Sync *resource_sync);
void signal_ongoing_unlocking(t_Drain_Ongoing_Resource_Sync *resource_sync);
void wait_draining_requests(t_Drain_Ongoing_Resource_Sync *resource_sync);
void signal_draining_requests(t_Drain_Ongoing_Resource_Sync *resource_sync);

#endif // KERNEL_SCHEDULER_H