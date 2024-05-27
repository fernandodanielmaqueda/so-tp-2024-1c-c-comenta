/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef MODULES_H
#define MODULES_H

#include <stdlib.h>
#include "commons/log.h"
#include "commons/config.h"

typedef enum PortType {
    KERNEL_TYPE,
    CPU_TYPE,
    CPU_DISPATCH_TYPE,
    CPU_INTERRUPT_TYPE,
    MEMORY_TYPE,
    IO_TYPE,
    TO_BE_DEFINED_TYPE
} PortType;
#define PortType_Count 7

extern char *module_name;
extern char *module_log_pathname;
extern char *connections_log_pathname;
extern char *module_config_pathname;

extern t_log *module_logger;
extern t_log *connections_logger;
extern t_config *module_config;

void initialize_loggers(void);
void finish_loggers(void);
void initialize_configs(void);
void finish_configs(void);
extern void read_module_config(t_config*);

#endif // MODULES_H