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
	init_resource_sync(&SCHEDULING_SYNC);
	init_resource_sync(&INTERFACES_SYNC);
	initialize_mutexes();
	initialize_semaphores();
	initialize_sockets();
	initialize_scheduling();

	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	initialize_kernel_console(NULL);

	finish_scheduling();
	//finish_threads();
	finish_sockets();
	destroy_resource_sync(&SCHEDULING_SYNC);
	destroy_resource_sync(&INTERFACES_SYNC);
	//finish_configs();
	finish_loggers();
	finish_semaphores();
	finish_mutexes();

    return EXIT_SUCCESS;
}

void initialize_mutexes(void) {
	pthread_mutex_init(&COORDINATOR_IO.shared_list_clients.mutex, NULL);

	pthread_mutex_init(&MUTEX_PID_COUNTER, NULL);
	pthread_mutex_init(&MUTEX_PCB_ARRAY, NULL);
	pthread_mutex_init(&MUTEX_LIST_RELEASED_PIDS, NULL);
	pthread_cond_init(&COND_LIST_RELEASED_PIDS, NULL);

	pthread_mutex_init(&(SHARED_LIST_NEW.mutex), NULL);
	pthread_mutex_init(&(SHARED_LIST_READY.mutex), NULL);
	pthread_mutex_init(&(SHARED_LIST_READY_PRIORITARY.mutex), NULL);
	pthread_mutex_init(&(SHARED_LIST_EXEC.mutex), NULL);
	pthread_mutex_init(&(SHARED_LIST_EXIT.mutex), NULL);

	pthread_mutex_init(&MUTEX_QUANTUM_INTERRUPT, NULL);

	pthread_mutex_init(&MUTEX_KILL_EXEC_PROCESS, NULL);

	pthread_mutex_init(&MUTEX_MULTIPROGRAMMING_DIFFERENCE, NULL);

	pthread_mutex_init(&MUTEX_SCHEDULING_PAUSED, NULL);
}

void finish_mutexes(void) {
	pthread_mutex_destroy(&COORDINATOR_IO.shared_list_clients.mutex);

	pthread_mutex_destroy(&MUTEX_PID_COUNTER);
	pthread_mutex_destroy(&MUTEX_PCB_ARRAY);
	pthread_mutex_destroy(&MUTEX_LIST_RELEASED_PIDS);
	pthread_cond_destroy(&COND_LIST_RELEASED_PIDS);

	pthread_mutex_destroy(&(SHARED_LIST_NEW.mutex));
	pthread_mutex_destroy(&(SHARED_LIST_READY.mutex));
	pthread_mutex_destroy(&(SHARED_LIST_READY_PRIORITARY.mutex));
	pthread_mutex_destroy(&(SHARED_LIST_EXEC.mutex));
	pthread_mutex_destroy(&(SHARED_LIST_EXIT.mutex));

	pthread_mutex_destroy(&MUTEX_QUANTUM_INTERRUPT);

	pthread_mutex_destroy(&MUTEX_KILL_EXEC_PROCESS);

	pthread_mutex_destroy(&MUTEX_MULTIPROGRAMMING_DIFFERENCE);
	
	pthread_mutex_destroy(&MUTEX_SCHEDULING_PAUSED);
}

void initialize_semaphores(void) {

	sem_init(&SEM_LONG_TERM_SCHEDULER_NEW, 0, 0);
	sem_init(&SEM_LONG_TERM_SCHEDULER_EXIT, 0, 0);
	sem_init(&SEM_SHORT_TERM_SCHEDULER, 0, 0);

	sem_init(&SEM_MULTIPROGRAMMING_LEVEL, 0, MULTIPROGRAMMING_LEVEL);
	sem_init(&SEM_MULTIPROGRAMMING_POSTER, 0, 0);
}

void finish_semaphores(void) {

	sem_destroy(&SEM_LONG_TERM_SCHEDULER_NEW);
	sem_destroy(&SEM_LONG_TERM_SCHEDULER_EXIT);
	sem_destroy(&SEM_SHORT_TERM_SCHEDULER);

	sem_destroy(&SEM_MULTIPROGRAMMING_LEVEL);
	sem_destroy(&SEM_MULTIPROGRAMMING_POSTER);
}

void read_module_config(t_config *module_config) {
	COORDINATOR_IO = (t_Server) {.server_type = KERNEL_PORT_TYPE, .clients_type = IO_PORT_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA"), .shared_list_clients.list = list_create()};
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

void pcb_free(t_PCB *pcb) {
	payload_destroy(&(pcb->io_operation));
	free(pcb);
}