/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef ENTRADASALIDA_H
#define ENTRADASALIDA_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/socket.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "utils/module.h"
#include "utils/socket.h"
#include "utils/package.h"
#include "utils/serialize/eviction_reason.h"
#include "utils/serialize/cpu_opcode.h"

typedef enum e_IO_Type {
    GENERIC_IO_TYPE,
    STDIN_IO_TYPE,
    STDOUT_IO_TYPE,
    DIALFS_IO_TYPE
} e_IO_Type;

typedef struct t_IO_Type {
    char *name;
    enum e_IO_Type type;
    void (*function) (void);
} t_IO_Type;

extern t_IO_Type IO_TYPES[];
extern t_IO_Type *IO_TYPE;

extern int (*IO_OPERATION) (t_CPU_Instruction *);

int module(int, char*[]);
void read_module_config(t_config *module_config);
t_IO_Type *io_type_find(char *name);
int (*io_operation_find (enum e_CPU_OpCode opcode)) (t_CPU_Instruction *);
void generic_function(void);
void initialize_sockets(void);
void finish_sockets(void);
void stdin_function();
void stdout_function();
int receive_from_memory(void* direccionMemoria);
int io_gen_sleep_io_operation(t_CPU_Instruction *instruction);
int io_stdin_read_io_operation(t_CPU_Instruction *instruction);
int io_stdout_write_io_operation(t_CPU_Instruction *instruction);
int io_fs_create_io_operation(t_CPU_Instruction *instruction);
int io_fs_delete_io_operation(t_CPU_Instruction *instruction);
int io_fs_truncate_io_operation(t_CPU_Instruction *instruction);
int io_fs_write_io_operation(t_CPU_Instruction *instruction);
int io_fs_read_io_operation(t_CPU_Instruction *instruction);

#endif /* ENTRADASALIDA_H */