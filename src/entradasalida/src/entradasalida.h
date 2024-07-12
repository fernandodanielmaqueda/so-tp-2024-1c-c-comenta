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
#include "utils/send.h"
#include "utils/socket.h"

typedef struct t_IO_Type {
    char *name;
    void (*function) (void);
} t_IO_Type;

typedef struct t_IO_Operation {
    char *name;
    int (*function) (t_Payload *);
} t_IO_Operation;

extern char *INTERFACE_NAME;

extern int WORK_UNIT_TIME;

extern t_Connection CONNECTION_KERNEL;
extern t_Connection CONNECTION_MEMORY;

extern char *PATH_BASE_DIALFS;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;
extern int COMPRESSION_DELAY;

extern t_IO_Type IO_TYPES[];

extern e_IO_Type IO_TYPE;

extern t_IO_Operation IO_OPERATIONS[];

int module(int, char*[]);
void read_module_config(t_config *module_config);
int io_type_find(char *name, e_IO_Type *destination);
void initialize_sockets(void);
void finish_sockets(void);
void generic_interface_function(void);
void stdin_interface_function(void);
void stdout_interface_function(void);
void dialfs_interface_function(void);
int io_operation_execute(t_Payload *operation);
int io_gen_sleep_io_operation(t_Payload *instruction);
int io_stdin_read_io_operation(t_Payload *instruction);
int io_stdout_write_io_operation(t_Payload *instruction);
int io_fs_create_io_operation(t_Payload *instruction);
int io_fs_delete_io_operation(t_Payload *instruction);
int io_fs_truncate_io_operation(t_Payload *instruction);
int io_fs_write_io_operation(t_Payload *instruction);
int io_fs_read_io_operation(t_Payload *instruction);

#endif /* ENTRADASALIDA_H */