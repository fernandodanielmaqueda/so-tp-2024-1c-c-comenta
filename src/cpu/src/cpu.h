/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/memory.h"
#include "commons/bitarray.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"
#include "utils/modules.h"
#include "utils/serialize.h"
#include "utils/socket.h"

int module(int, char*[]);
void read_module_config(t_config *module_config);
void initialize_sockets(void);
void finish_sockets(void);
void *cpu_dispatch_start_server_for_kernel(void *server_parameter);
void *cpu_interrupt_start_server_for_kernel(void *server_parameter);
void instruction_cycle(void);
void execute(t_instruction_use *instruction, t_pcb *pcb);
t_pcb *string_to_register(const char *string);

#endif /* CPU_H */