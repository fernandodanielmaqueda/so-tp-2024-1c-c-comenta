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
#include "utils/module.h"
#include "utils/serialize/interrupt.h"
#include "utils/serialize/cpu_instruction.h"
#include "utils/serialize/cpu_memory_request.h"
#include "utils/serialize/pcb.h"
#include "utils/serialize/arguments.h"
#include "utils/socket.h"
#include "socket.h"
#include "opcodes.h"

#include "utils/estructuras.h"

//Para el SET en el ciclo de instruccion verificar si es in o out
typedef enum {
	IN,
	OUT
} t_in_out;

extern char *MODULE_NAME;
extern char *MODULE_LOG_PATHNAME;
extern char *MODULE_CONFIG_PATHNAME;

extern t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
extern t_config *MODULE_CONFIG;

// Tipos de interrupciones para el ciclo
extern int interruption_io;

extern int CANTIDAD_ENTRADAS_TLB;
extern char *ALGORITMO_TLB;

extern int size_pag;
extern long timestamp;
extern int direccion_logica; // momentaneo hasta ver de donde la saco
extern t_list *tlb;          // tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

// Variables para trabajar con las instrucciones
extern int nro_page;
extern uint32_t value;

extern t_PCB *PCB;
extern e_Register register_origin;
extern e_Register register_destination;

extern e_Interrupt INTERRUPT;

extern int dir_logica_origin;
extern int dir_logica_destination;

extern int dir_fisica_origin;
extern int dir_fisica_destination;

extern uint32_t unit_work;
extern char *interfaz;

extern pthread_mutex_t sem_mutex_tlb;

extern const char *t_register_string[];
extern const char *t_interrupt_type_string[];

int module(int, char*[]);
void read_module_config(t_config *module_config);
void initialize_sockets(void);
void finish_sockets(void);
void *cpu_dispatch_start_server_for_kernel(void *server_parameter);
void *cpu_interrupt_start_server_for_kernel(void *server_parameter);
void instruction_cycle(void);
void decode_execute(t_CPU_Instruction *instruction, t_PCB *pcb);
int string_to_register(const char *string);
int mmu(uint32_t dir_logica, t_PCB *pcb, int tamanio_pagina, int register_otrigin , int register_destination, int in_out);
int check_tlb(int process_id, int nro_page);
void tlb_access(t_PCB *pcb, int nro_page, int nro_frame_required, int direc ,int register_origin, int register_destination,int in_out);
void request_data_in_memory(int nro_frame_required, int pid, int nro_page, int direc, int register_origin, int register_destination);
void request_data_out_memory(int nro_frame_required, int pid, int nro_page, int direc, int register_origin, int register_destination);
void request_frame_memory(int page, int pid);
t_PCB *cpu_receive_pcb(void);
t_Arguments *cpu_receive_cpu_instruction(void);
e_Interrupt *cpu_receive_interrupt_type(void);

#endif /* CPU_H */