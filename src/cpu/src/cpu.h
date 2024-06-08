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
#include "utils/serialize/cpu_instruction.h"
#include "utils/serialize/cpu_memory_request.h"
#include "utils/serialize/pcb.h"
#include "utils/socket.h"


//Para el SET en el ciclo de instruccion verificar si es in o out
typedef enum {
	IN,
	OUT
} t_in_out;



const char * const t_instruction_type_string[] = {
    [SET] = "SET",
    [MOV_IN] = "MOV_IN",
    [MOV_OUT] = "MOV_OUT",
    [SUM] = "SUM",
    [SUB] = "SUB",
    [JNZ] = "JNZ",
    [RESIZE] = "RESIZE",
    [COPY_STRING] = "COPY_STRING",
    [WAIT] = "WAIT",
    [SIGNAL] = "SIGNAL",
    [IO_GEN_SLEEP] = "IO_GEN_SLEEP",
    [EXIT]= "EXIT",
    [IO_STDIN_READ] = "IO_STDIN_READ",
    [IO_STDOUT_WRITE] = "IO_STDOUT_WRITE",
    [IO_FS_CREATE] = "IO_FS_CREATE",`
    [IO_FS_DELETE] = "IO_FS_DELETE",
    [IO_FS_TRUNCATE] = "IO_FS_TRUNCATE",
    [IO_FS_WRITE] = "IO_FS_WRITE",
    [IO_FS_READ] = "IO_FS_READ"
       
   
};
const char * const t_register_string[] = {
    [AX] = "AX",
    [BX] = "BX",
    [CX] = "CX",
    [DX] = "DX"
    [EAX] = "EAX",
    [EBX] = "EBX",
    [ECX] = "ECX",
    [EDX] = "EDX",
    [RAX] = "RAX",
    [RBX] = "RBX",
    [RCX] = "RCX",
    [RDX] = "RDX",
    [SI] = "SI",
    [DI] = "DI"
};

const char * const t_interrupt_type_string[] = {
    [TYPE_INTERRUPT_SIN_INT] = "SIN_INTERRUPCION",
    [TYPE_INTERRUPT_FIN_PROCESO] = "INT_FIN_PROCESO",
    [TYPE_INTERRUPT_FIN_QUANTUM] = "INT_FIN_QUANTUM",
    
};



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
int request_frame_memory(int page, int pid);
t_PCB *cpu_receive_pcb(void);
t_CPU_Instruction *cpu_receive_cpu_instruction(void);
e_Interrupt cpu_recive_interrupt_type(void);


#endif /* CPU_H */