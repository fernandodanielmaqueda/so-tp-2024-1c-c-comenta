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
#include "utils/serialize/arguments.h"
#include "utils/serialize/interrupt.h"
typedef enum e_IO_Type {
    GENERIC_IO_TYPE,
    STDIN_IO_TYPE,
    STDOUT_IO_TYPE,
    DIALFS_IO_TYPE
} e_IO_Type;

int module(int, char*[]);
void read_module_config(t_config *module_config);
void initialize_sockets(void);
void finish_sockets(void);
void* generic();
void gen_sleep(int work_units, int work_unit_time);
void* stdin_function();
void IO_STDIN_READ(void* registroDireccion, void* direccionTamanio);
void* stdout_function();
void IO_STDOUT_WRITE(int direccionMemoria);
int receive_from_memory(int direccionMemoria);

#endif /* ENTRADASALIDA_H */