/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef KERNEL_H_
#define KERNEL_H_

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
#include "utils/modules.h"
#include "utils/socket.h"

int module(int, char*[]);
void read_module_config(t_config*);
void initialize_sockets(void);
void switch_process_state(t_pcb *pcb, int estado_nuevo);
t_pcb *create_pcb(char *instrucciones);
void initialize_long_term_scheduler();
void initialize_short_term_scheduler();
void initialize_cpu_command_line_interface();
void long_term_scheduler();
void short_term_scheduler();
t_pcb *FIFO_scheduling_algorithm();
void receptor_mensajes_cpu();
int current_time();
int asignar_PID();

#endif /* KERNEL_H_ */