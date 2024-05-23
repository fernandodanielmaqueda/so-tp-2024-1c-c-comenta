/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef MODULES_H
#define MODULES_H

#include <stdlib.h>
#include "commons/log.h"
#include "commons/config.h"

extern char *module_name;
extern char *module_log_pathname;
extern char *module_config_pathname;

extern t_log *module_logger;

extern t_config *module_config;

void initialize_module_logger(void);
void initialize_module_config(void);
extern void read_module_config(t_config*);

#endif // MODULES_H