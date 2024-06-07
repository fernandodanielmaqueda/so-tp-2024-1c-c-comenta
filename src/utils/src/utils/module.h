/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef MODULE_H
#define MODULE_H

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

extern char *MODULE_NAME;
extern char *MODULE_LOG_PATHNAME;
extern char *SOCKET_LOG_PATHNAME;
extern char *SERIALIZE_LOG_PATHNAME;
extern char *MODULE_CONFIG_PATHNAME;

extern t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
extern t_log *SERIALIZE_LOGGER;
extern t_config *MODULE_CONFIG;

void initialize_loggers(void);
void finish_loggers(void);
void initialize_configs(void);
void finish_configs(void);
extern void read_module_config(t_config*);

#endif // MODULES_H