/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef MODULE_H
#define MODULE_H

#include <stdlib.h>
#include "commons/log.h"
#include "commons/config.h"

typedef enum e_PortType {
    KERNEL_PORT_TYPE,
    CPU_PORT_TYPE,
    CPU_DISPATCH_PORT_TYPE,
    CPU_INTERRUPT_PORT_TYPE,
    MEMORY_PORT_TYPE,
    IO_PORT_TYPE,
    TO_BE_DEFINED_PORT_TYPE
} e_PortType;
#define PortType_Count 7

extern char *MODULE_NAME;

extern t_log *MINIMAL_LOGGER;
extern char *MINIMAL_LOG_PATHNAME;

extern t_log *MODULE_LOGGER;
extern char *MODULE_LOG_PATHNAME;

extern t_log *SOCKET_LOGGER;
extern char *SOCKET_LOG_PATHNAME;

extern t_log *SERIALIZE_LOGGER;
extern char *SERIALIZE_LOG_PATHNAME;

extern t_config *MODULE_CONFIG;
extern char *MODULE_CONFIG_PATHNAME;

void initialize_loggers(void);
void finish_loggers(void);
void initialize_configs(char *pathname);
void finish_configs(void);
extern void read_module_config(t_config*);

#endif // MODULES_H