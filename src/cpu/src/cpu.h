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
#include "utils/estructuras.h"
#include "utils/modules.h"
#include "utils/estructuras.h"
#include "utils/serialize.h"
#include "utils/socket.h"

int tamanio_pagina; //momentaneo hasta que me llegue de memoria
long timestamp;
int direccion_logica; //momentaneo hasta ver de donde la saco
t_list  *tlb; //tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb


//Para el SET en el ciclo de instruccion verificar si es in o out
typedef enum {
	IN,
	OUT
} t_in_out;


int module(int, char*[]);
void read_module_config(t_config *module_config);
void initialize_sockets(void);
void finish_sockets(void);
void *cpu_dispatch_start_server_for_kernel(void *server_parameter);
void *cpu_interrupt_start_server_for_kernel(void *server_parameter);
void instruction_cycle(void);
void execute(t_instruction_use *instruction, t_pcb *pcb);
int string_to_register(const char *string);
int mmu(uint32_t dir_logica, t_pcb *pcb, int tamanio_pagina, int register_otrigin , int register_destination, int in_out);
int check_tlb(int process_id, int nro_page);
void tlb_access(t_pcb *pcb, int nro_page, int nro_frame_required, int direc ,int register_origin, int register_destination,int in_out);
void request_data_in_memory(int nro_frame_required, int pid, int nro_page, int direc, int register_origin, int register_destination);
void request_data_out_memory(int nro_frame_required, int pid, int nro_page, int direc, int register_origin, int register_destination);



#endif /* CPU_H */