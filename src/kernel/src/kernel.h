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
#include "utils/module.h"
#include "utils/serialize/pcb.h"
#include "utils/socket.h"
#include "console.h"
#include "socket.h"

extern t_Server COORDINATOR_IO;
extern t_Connection CONNECTION_MEMORY;
extern t_Connection CONNECTION_CPU_DISPATCH;
extern t_Connection CONNECTION_CPU_INTERRUPT;

int module(int, char*[]);
void read_module_config(t_config *module_config);
void switch_process_state(t_PCB* pcb, int new_state);
t_PCB *create_pcb();
void initialize_long_term_scheduler(void);
void initialize_short_term_scheduler(void);
void initialize_cpu_command_line_interface(void);
void *long_term_scheduler(void*);
void *short_term_scheduler(void*);
t_PCB *FIFO_scheduling_algorithm(void);
t_PCB *RR_scheduling_algorithm(void);
//t_PCB *VRR_scheduling_algorithm(void* arg);
void *receptor_mensajes_cpu(void*);
int current_time(void);
int asignar_PID();
void* start_quantum(void* arg);

#endif /* KERNEL_H */