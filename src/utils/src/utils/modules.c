/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "modules.h"

void initialize_module_logger() {
	module_logger = log_create(module_log_pathname, module_name, true, LOG_LEVEL_INFO);
	
}

void initialize_module_config() {
	t_config *module_config = config_create(module_config_pathname);

	if(module_config == NULL) {
		log_error(module_logger, "No se pudo abrir el archivo config del modulo %s: %s", module_name, module_config_pathname);
        exit(EXIT_FAILURE);
	}

	read_module_config(module_config);
}