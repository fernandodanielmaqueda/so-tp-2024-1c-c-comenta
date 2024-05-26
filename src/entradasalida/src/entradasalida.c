/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

char *module_name = "entradasalida";
char *module_log_pathname = "entradasalida.log";
char *module_config_pathname = "entradasalida.config";

t_log* module_logger;
extern t_log *connections_logger;
t_config* module_config;

char* TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;

Connection CONNECTION_KERNEL;
Connection CONNECTION_MEMORY;

char*  PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs();
    initialize_sockets();

    log_info(module_logger, "Modulo %s inicializado correctamente\n", module_name);

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();
   
    return EXIT_SUCCESS;
}

void read_module_config(t_config* module_config) {
    TIPO_INTERFAZ = config_get_string_value(module_config, "TIPO_INTERFAZ");
    TIEMPO_UNIDAD_TRABAJO = config_get_int_value(module_config, "TIEMPO_UNIDAD_TRABAJO");
    CONNECTION_KERNEL = (struct Connection) {.client_type = IO_TYPE, .server_type = KERNEL_TYPE, .ip = config_get_string_value(module_config, "IP_KERNEL"), .port = config_get_string_value(module_config, "PUERTO_KERNEL")};
    CONNECTION_MEMORY = (struct Connection) {.client_type = IO_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(module_config, "IP_MEMORIA"), .port = config_get_string_value(module_config, "PUERTO_MEMORIA")};
    PATH_BASE_DIALFS = config_get_string_value(module_config, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(module_config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(module_config, "BLOCK_COUNT");
}

void initialize_sockets(void) {
	pthread_t thread_io_connect_to_memory;
	pthread_t thread_io_connect_to_kernel;

	// [Client] Entrada/Salida -> [Server] Memoria
	pthread_create(&thread_io_connect_to_memory, NULL, client_thread_connect_to_server, (void*) &CONNECTION_MEMORY);
	// [Client] Entrada/Salida -> [Server] Kernel
	pthread_create(&thread_io_connect_to_kernel, NULL, client_thread_connect_to_server, (void*) &CONNECTION_KERNEL);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_io_connect_to_memory, NULL);
	pthread_join(thread_io_connect_to_kernel, NULL);
}

void finish_sockets(void) {
	close(CONNECTION_KERNEL.fd_connection);
	close(CONNECTION_MEMORY.fd_connection);
}