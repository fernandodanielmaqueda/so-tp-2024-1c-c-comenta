/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "kernel.h"

char *MODULE_NAME = "kernel";

t_log *MODULE_LOGGER;
char *MODULE_LOG_PATHNAME = "kernel.log";

t_config *MODULE_CONFIG;
char *MODULE_CONFIG_PATHNAME = "kernel.config";

int module(int argc, char *argv[]) {

	initialize_loggers();
	initialize_configs(MODULE_CONFIG_PATHNAME);
	initialize_sockets();
	
	PID_COUNTER = 0;
	
	sem_init(&SEM_LONG_TERM_SCHEDULER_NEW, 0, 0);
	sem_init(&SEM_LONG_TERM_SCHEDULER_EXIT, 0, 0);
	sem_init(&SEM_SHORT_TERM_SCHEDULER, 0, 0);
	sem_init(&SEM_MULTIPROGRAMMING_LEVEL, 0, MULTIPROGRAMMING_LEVEL);
	sem_init(&SEM_MULTIPROGRAMMING_POSTER, 0, 0);

	LIST_NEW = list_create();
	LIST_READY = list_create();
	LIST_READY_PRIORITARY = list_create();
	LIST_EXECUTING = list_create();
	LIST_BLOCKED = list_create();
	LIST_EXIT = list_create();

	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	//UN HILO PARA CADA PROCESO
	initialize_long_term_scheduler();
	initialize_short_term_scheduler();

	initialize_kernel_console(NULL);

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();

    return EXIT_SUCCESS;
}

void read_module_config(t_config *module_config) {
	COORDINATOR_IO = (t_Server) {.server_type = KERNEL_PORT_TYPE, .clients_type = IO_PORT_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA")};
	CONNECTION_MEMORY = (t_Connection) {.client_type = KERNEL_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(module_config, "IP_MEMORIA"), .port = config_get_string_value(module_config, "PUERTO_MEMORIA")};
	CONNECTION_CPU_DISPATCH = (t_Connection) {.client_type = KERNEL_PORT_TYPE, .server_type = CPU_DISPATCH_PORT_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_DISPATCH")};
	CONNECTION_CPU_INTERRUPT = (t_Connection) {.client_type = KERNEL_PORT_TYPE, .server_type = CPU_INTERRUPT_PORT_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_INTERRUPT")};
	SCHEDULING_ALGORITHM = find_scheduling_algorithm(config_get_string_value(module_config, "ALGORITMO_PLANIFICACION"));
	QUANTUM = config_get_int_value(module_config, "QUANTUM");
	resources_read_module_config(module_config);
	MULTIPROGRAMMING_LEVEL = config_get_int_value(module_config, "GRADO_MULTIPROGRAMACION");
}