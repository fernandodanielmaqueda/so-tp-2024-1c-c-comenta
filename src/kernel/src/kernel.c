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
	initialize_configs();
	initialize_sockets();
	PID_COUNTER = 0;

	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);
	
	sem_init(&SEM_LONG_TERM_SCHEDULER, 0, 0);
	sem_init(&SEM_SHORT_TERM_SCHEDULER, 0, 0);
	sem_init(&SEM_MULTIPROGRAMMING_LEVEL, 0, MULTIPROGRAMMING_LEVEL);

	START_PROCESS = list_create();

	LIST_NEW = list_create();
	LIST_READY = list_create();
	LIST_READY_PRIORITARY = list_create();
	LIST_EXECUTING = list_create();
	LIST_BLOCKED = list_create();
	LIST_EXIT = list_create();

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
	COORDINATOR_IO = (t_Server) {.server_type = KERNEL_TYPE, .clients_type = IO_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA")};
	CONNECTION_MEMORY = (t_Connection) {.client_type = KERNEL_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(module_config, "IP_MEMORIA"), .port = config_get_string_value(module_config, "PUERTO_MEMORIA")};
	CONNECTION_CPU_DISPATCH = (t_Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_DISPATCH_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_DISPATCH")};
	CONNECTION_CPU_INTERRUPT = (t_Connection) {.client_type = KERNEL_TYPE, .server_type = CPU_INTERRUPT_TYPE, .ip = config_get_string_value(module_config, "IP_CPU"), .port = config_get_string_value(module_config, "PUERTO_CPU_INTERRUPT")};
	SCHEDULING_ALGORITHM = find_scheduling_algorithm(config_get_string_value(module_config, "ALGORITMO_PLANIFICACION"));
	QUANTUM = config_get_int_value(module_config, "QUANTUM");
	resources_read_module_config(module_config);
	MULTIPROGRAMMING_LEVEL = config_get_int_value(module_config, "GRADO_MULTIPROGRAMACION");
}

void *cpu_dispatch_handler(void *NULL_parameter) {

	client_thread_connect_to_server((void*) &CONNECTION_CPU_DISPATCH);
	sem_post(&CONNECTED_CPU_DISPATCH);

	t_PCB *pcb;
	e_Interrupt *interrupt;
	t_Arguments *instruction;
	
	while(1) {

    	t_Package *package = package_receive(CONNECTION_CPU_DISPATCH.fd_connection);
		switch (package->header) {
		case SUBHEADER_HEADER:
			pcb = pcb_deserialize(package->payload);
			interrupt = interrupt_deserialize(package->payload);
			instruction = arguments_deserialize(package->payload);
			break;
		default:
			log_error(SERIALIZE_LOGGER, "HeaderCode pcb %d desconocido", package->header);
			exit(EXIT_FAILURE);
			break;
		}
		package_destroy(package);

		int exit_status;

		switch(*interrupt) {
			case SYSCALL_CAUSE:
				SYSCALL_PCB = pcb;
				exit_status = syscall_execute(instruction);

				if(exit_status) {
					switch_process_state(pcb, EXIT_STATE);
					break;
				}

				if(BLOCKING_SYSCALL) {
					switch_process_state(SYSCALL_PCB, BLOCKED_STATE);
					break;
				}

				// En caso de que sea una syscall no bloqueante
				pcb_send(pcb, CONNECTION_CPU_DISPATCH.fd_connection);
				break;
			default:
				break;

		}

		// pcb_free(pcb)
		// interrupt_free(interrupt);
		// instruction_free(instruction);
		
	}

	return NULL;
}