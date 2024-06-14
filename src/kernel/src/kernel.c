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
	initialize_cpu_command_line_interface();

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

void initialize_cpu_command_line_interface(void) {
	pthread_create(&hilo_mensajes_cpu, NULL, receptor_mensajes_cpu, NULL);
	//log_info(MODULE_LOGGER, "Inicio mensajes cpu");
	pthread_detach(hilo_mensajes_cpu);
}

/*
void listen_cpu(int fd_cpu) {
    while(1) {
        t_Package* paquete = package_receive(fd_cpu);
        e_Header header = paquete->header;
        //e_CPU_Memory_Request memory_request = 0; //enum HeaderCode headerCode = package_receive_header(fd_cpu);
        switch (header) {
            
            case DISCONNECTION_HEADERCODE:
                log_warning(MODULE_LOGGER, "Se desconecto CPU.");
                log_destroy(MODULE_LOGGER);
                return;
                
            case PAGE_SIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                send_int(PAGE_SIZE_REQUEST, TAM_PAGINA,FD_CLIENT_CPU);
                break;

            case RESIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                resize_process(paquete->payload);
                break;

            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
            }
    }
} */ //tomar como inspiración para la función de abajo

void *receptor_mensajes_cpu(void *parameter) {
	// Package *package;

	while(1) {
		
		//FALTA RECIBIR PAQUETE DE LA CONEXION CON CPU
		//FALTA DESERIALIZAR PCB
		
		/*
		switch (package->codigo_operacion)
		{
			case EXIT:
			{
				switch_process_state(pcb, EXITED);

				break;
			}
			case SET:
			{

			}
			case MOVE_IN:
			{

			}
			case MOVE_OUT:
			{

			}
			case SUM:
			{

			}
			case SUB:
			{

			}
			case JNZ:
			{

			}
			case RESIZE:
			{
				//mando mensaje a memoria
				//Recibo mensaje de memorai
				//mando mensaje a fs
				//recibo mensaje de fs
				//mando a cpu de vuelta
			}
			case COPY_STRING:
			{

			}
			case WAIT:
			{

			}
			case SIGNAL:
			{

			}
			case IO_GEN_SLEEP:
			{

			}
			case IO_STDIN_READ:
			{

			}
			case IO_STDOUT_WRITE:
			{

			}
			case IO_FS_CREATE:
			{

			}
			case IO_FS_DELETE:
			{

			}
			case IO_FS_TRUNCATE:
			{

			}
			case IO_FS_WRITE:
			{

			}
			case IO_FS_READ:
			{

			}
			case EXIT:
			{

			}
			// ACA SIGO AGREGANDO CASES SEGUN LA INSTRUCCION
		}
		*/
	}

	return NULL;
}