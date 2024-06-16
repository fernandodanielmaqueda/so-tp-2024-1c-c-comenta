/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef CPU_OPCODES_H
#define CPU_OPCODES_H

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
#include "cpu.h"

#include "utils/estructuras.h"

typedef struct t_CPU_OpCode {
    char *name;
    int (*function) (void);
} t_CPU_OpCode;

extern t_CPU_OpCode CPU_OPCODES[];

t_CPU_OpCode *decode_instruction(char *name);
int set_cpu_opcode(void);
int mov_in_cpu_opcode(void);
int mov_out_cpu_opcode(void);
int sum_cpu_opcode(void);
int sub_cpu_opcode(void);
int jnz_cpu_opcode(void);
int resize_cpu_opcode(void);
int copy_string_cpu_opcode(void);
int wait_cpu_opcode(void);
int signal_cpu_opcode(void);
int io_gen_sleep_cpu_opcode(void);
int io_stdin_read_cpu_opcode(void);
int io_stdout_write_cpu_opcode(void);
int io_fs_create_cpu_opcode(void);
int io_fs_delete_cpu_opcode(void);
int io_fs_truncate_cpu_opcode(void);
int io_fs_write_cpu_opcode(void);
int io_fs_read_cpu_opcode(void);
int exit_cpu_opcode(void);

#endif // CPU_OPCODES_H