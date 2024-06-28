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
	initialize_mutexes();
	initialize_semaphores();
	initialize_sockets();

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
	finish_semaphores();
	finish_mutexes();

    return EXIT_SUCCESS;
}

void initialize_mutexes(void) {
	pthread_mutex_init(&MUTEX_QUANTUM_INTERRUPT, NULL);
	pthread_mutex_init(&MUTEX_LIST_PROCESS_STATES, NULL);
	pthread_cond_init(&COND_LIST_PROCESS_STATES, NULL);
	pthread_cond_init(&COND_SWITCHING_STATES, NULL);
	pthread_mutex_init(&MUTEX_LIST_NEW, NULL);
	pthread_mutex_init(&MUTEX_LIST_READY, NULL);
	pthread_mutex_init(&MUTEX_LIST_READY_PRIORITARY, NULL);
	pthread_mutex_init(&MUTEX_LIST_EXECUTING, NULL);
	pthread_mutex_init(&MUTEX_LIST_BLOCKED, NULL);
	pthread_mutex_init(&MUTEX_LIST_EXIT, NULL);
	pthread_mutex_init(&MUTEX_MULTIPROGRAMMING_DIFFERENCE, NULL);
}

void finish_mutexes(void) {
	pthread_mutex_destroy(&MUTEX_QUANTUM_INTERRUPT);
	pthread_mutex_destroy(&MUTEX_LIST_PROCESS_STATES);
	pthread_cond_destroy(&COND_LIST_PROCESS_STATES);
	pthread_cond_destroy(&COND_SWITCHING_STATES);
	pthread_mutex_destroy(&MUTEX_LIST_NEW);
	pthread_mutex_destroy(&MUTEX_LIST_READY);
	pthread_mutex_destroy(&MUTEX_LIST_READY_PRIORITARY);
	pthread_mutex_destroy(&MUTEX_LIST_EXECUTING);
	pthread_mutex_destroy(&MUTEX_LIST_BLOCKED);
	pthread_mutex_destroy(&MUTEX_LIST_EXIT);
	pthread_mutex_destroy(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
}

void initialize_semaphores(void) {

	sem_init(&SEM_LONG_TERM_SCHEDULER_NEW, 0, 0);
	sem_init(&SEM_LONG_TERM_SCHEDULER_EXIT, 0, 0);
	sem_init(&SEM_SHORT_TERM_SCHEDULER, 0, 0);
	sem_init(&SEM_MULTIPROGRAMMING_LEVEL, 0, MULTIPROGRAMMING_LEVEL);
	sem_init(&SEM_MULTIPROGRAMMING_POSTER, 0, 0);
	sem_init(&SEM_SWITCHING_STATES_COUNT, 0, 0);
}

void finish_semaphores(void) {

	sem_destroy(&SEM_LONG_TERM_SCHEDULER_NEW);
	sem_destroy(&SEM_LONG_TERM_SCHEDULER_EXIT);
	sem_destroy(&SEM_SHORT_TERM_SCHEDULER);
	sem_destroy(&SEM_MULTIPROGRAMMING_LEVEL);
	sem_destroy(&SEM_MULTIPROGRAMMING_POSTER);
	sem_destroy(&SEM_SWITCHING_STATES_COUNT);

}

void wait_list_process_states(void) {
    pthread_mutex_lock(&MUTEX_LIST_PROCESS_STATES);
        while(LIST_PROCESS_STATES)
            pthread_cond_wait(&COND_LIST_PROCESS_STATES, &MUTEX_LIST_PROCESS_STATES);
		sem_post(&SEM_SWITCHING_STATES_COUNT);
    pthread_mutex_unlock(&MUTEX_LIST_PROCESS_STATES);
}

void signal_list_process_states(void) {
	sem_wait(&SEM_SWITCHING_STATES_COUNT);
	pthread_cond_signal(&COND_SWITCHING_STATES);
}

void read_module_config(t_config *module_config) {
	COORDINATOR_IO = (t_Server) {.server_type = KERNEL_PORT_TYPE, .clients_type = IO_PORT_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA")};
	CONNECTION_MEMORY = (t_Connection) {.client_type = KERNEL_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(module_config, "IP_MEMORIA"), .port = config_get_string_value(module_config, "PUERTO_MEMORIA")};
	CONNECTION_CPU_DISPATCH = (t_Connection) {.client_type = KERNEL_PORT_TYPE, .server_type = CPU_DISPATCH_PORT_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_DISPATCH")};
	CONNECTION_CPU_INTERRUPT = (t_Connection) {.client_type = KERNEL_PORT_TYPE, .server_type = CPU_INTERRUPT_PORT_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_INTERRUPT")};
	if(find_scheduling_algorithm(config_get_string_value(module_config, "ALGORITMO_PLANIFICACION"), &SCHEDULING_ALGORITHM)) {
		log_error(MODULE_LOGGER, "ALGORITMO_PLANIFICACION invalido");
		exit(EXIT_FAILURE);
	}
	QUANTUM = config_get_int_value(module_config, "QUANTUM");
	resources_read_module_config(module_config);
	MULTIPROGRAMMING_LEVEL = config_get_int_value(module_config, "GRADO_MULTIPROGRAMACION");
}