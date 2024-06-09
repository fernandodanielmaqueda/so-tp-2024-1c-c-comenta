/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

char *MODULE_NAME = "entradasalida";
char *MODULE_LOG_PATHNAME = "entradasalida.log";
char *MODULE_CONFIG_PATHNAME = "entradasalida.config";

t_log* MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;k
t_config* MODULE_CONFIG;

char *TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;

t_Connection CONNECTION_KERNEL;
t_Connection CONNECTION_MEMORY;

char *PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs();
    initialize_sockets();

    log_info(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();
   
    return EXIT_SUCCESS;
}

void read_module_config(t_config* MODULE_CONFIG) {
    TIPO_INTERFAZ = config_get_string_value(MODULE_CONFIG, "TIPO_INTERFAZ");
    TIEMPO_UNIDAD_TRABAJO = config_get_int_value(MODULE_CONFIG, "TIEMPO_UNIDAD_TRABAJO");
    CONNECTION_KERNEL = (t_Connection) {.client_type = IO_TYPE, .server_type = KERNEL_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_KERNEL"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_KERNEL")};
    CONNECTION_MEMORY = (t_Connection) {.client_type = IO_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
    PATH_BASE_DIALFS = config_get_string_value(MODULE_CONFIG, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(MODULE_CONFIG, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(MODULE_CONFIG, "BLOCK_COUNT");
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

void* generic(t_config* config){
	//conectar a kernel

	//escuchar peticion siempre

	//recibe peticion

	//chequear si puede realizar instruccion

		/* No puede realizar: 
			Avisa a kernel y se hace cargo kernel
		*/	
	
	//si es IO_GEN_SLEEP : gen_sleep()
	gen_sleep(work_units, config->TIEMPO_UNIDAD_TRABAJO);
	//avisar a kernel que ejecutó 

}

void gen_sleep(int work_units, int work_unit_time){
	sleep(work_units * work_unit_time);
}

void* stdin(){
	//conectar a kernel

	//escuchar peticion siempre

	//recibe peticion

	//chequear si puede realizar instruccion

		/* No puede realizar: 
			Avisa a kernel y se hace cargo kernel
		*/	

	//si es IO_STDIN_READ realizarlo

	//avisar a kernel
}

void IO_STDIN_READ(int direccionMemoria){

	char text[30];
	printf("Ingrese un texto: ");
	scanf("%s",text);

	//enviar el texto a memoria
	//send_to_memory(text,direccionMemoria)
}

void* stdout(){
	//conectar a kernel

	//escuchar peticion siempre

	//recibe peticion

	//chequear si puede realizar instruccion

		/* No puede realizar: 
			Avisa a kernel y se hace cargo kernel
		*/	

	//si es IO_STDOUT_WRITE realizarlo

	//avisar a kernel
}

void IO_STDOUT_WRITE(int direccionMemoria){
	int dir_memoria = receive_from_memory(direccionMemoria);
	printf("El valor hallado en la direccion de memoria es: %d", dir_memoria);
}