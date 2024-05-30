/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "modules.h"

t_log *CONNECTIONS_LOGGER;
char *CONNECTIONS_LOG_PATHNAME = "connections.log";

void initialize_loggers(void) {
	MODULE_LOGGER = log_create(MODULE_LOG_PATHNAME, MODULE_NAME, true, LOG_LEVEL_INFO);
	CONNECTIONS_LOGGER = log_create(CONNECTIONS_LOG_PATHNAME, MODULE_NAME, true, LOG_LEVEL_INFO);
}

void finish_loggers(void) {
	log_destroy(MODULE_LOGGER);
	log_destroy(CONNECTIONS_LOGGER);
}

void initialize_configs(void) {
	MODULE_CONFIG = config_create(MODULE_CONFIG_PATHNAME);

	if(MODULE_CONFIG == NULL) {
		log_error(MODULE_LOGGER, "No se pudo abrir el archivo config del modulo %s: %s", MODULE_NAME, MODULE_CONFIG_PATHNAME);
        exit(EXIT_FAILURE);
	}

	read_module_config(MODULE_CONFIG);
}

void finish_configs(void) {
	config_destroy(MODULE_CONFIG);
}