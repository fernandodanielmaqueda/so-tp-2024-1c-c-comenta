/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"
/*
char *MODULE_NAME = "entradasalida";
char *MODULE_LOG_PATHNAME = "entradasalida.log";
char *MODULE_CONFIG_PATHNAME = "entradasalida.config";

t_log* MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
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

	//obtengo parametro 1
 	char* parametro1 = argv[1];
	log_info(MODULE_LOGGER, "el parametro 1 es: %s", parametro1);
	
	char* parametro2 = argv[2];
	initialize_configs_io(argv[2]);
	log_info(MODULE_LOGGER, "el parametro 2 es: %s", parametro2);

    initialize_sockets();

	log_info(MODULE_LOGGER, "El tipo del modulo es: %s",TIPO_INTERFAZ);
 	if(strcmp(TIPO_INTERFAZ, "GENERIC") == 0)
	{
		log_info(MODULE_LOGGER, "ES el generico");
		generic_function();

	}
	else if(strcmp(TIPO_INTERFAZ, "STDIN") == 0)
	{
		log_info(MODULE_LOGGER, "Es STDIN");
		stdin_function();
	}
	else if(strcmp(TIPO_INTERFAZ, "STDOUT") == 0)
	{
		log_info(MODULE_LOGGER, "Es STDOUT");
		stdout_function();
	}else
	{
		log_info(MODULE_LOGGER, "No reconozco el tipo interfaz");
		char* mensaje = "No reconozco esta interfaz";
		send(CONNECTION_KERNEL.fd_connection, mensaje, strlen(mensaje), 0);

	}

    log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

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
/*
void generic_function(){

	//escuchar peticion siempre
	t_Package* instruction_package;
	t_Arguments* instruction_arguments;
	//recibe peticion
	while(1){
		//instruction_package = package_receive(CONNECTION_KERNEL);
		instruction_arguments = arguments_deserialize(instruction_package->payload);
		
		package_destroy(instruction_package);
		//Chequear si puede realizar esta instruccion
		//if(strcmp(instruction_arguments.argv[0],"IO_GEN_SLEEP") == 0){
			
			//si la puede realizar entonces hace el gen_sleep
			//gen_sleep(instruction_arguments.argv[2],TIEMPO_UNIDAD_TRABAJO);

			//arguments_free(instruction_arguments);
			//Avisar a kernel que la hizo
			//instruction_made* REALIZADA;
			
		}else{
			//Si no la puede realizar avisa a kernel y se hace cargo kernel
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			e_Interrupt* ERROR_CAUSE;
		//	interrupt_send(ERROR_CAUSE,CONNECTION_KERNEL);
			free(ERROR_CAUSE);
		}

	}

}

void gen_sleep(int work_units, int work_unit_time){
	sleep(work_units * work_unit_time);
}
/*
void* stdin_function(){


	t_Package* instruction_package;
	t_Arguments* instruction_arguments;
	//escuchar peticion siempre
	while(1){
		//recibe peticion
		//instruction_package = package_receive(CONNECTION_KERNEL);
		instruction_arguments = arguments_deserialize(instruction_package->payload);
		
		package_destroy(instruction_package);
		//Chequear si puede realizar esta instruccion
		//if(strcmp(instruction_arguments.argv[0],"IO_STDIN_READ") == 0){
			//si la puede realizar entonces hace el STDIN READ
			//IO_STDIN_READ(2, instruction_arguments.argv[]);

			//arguments_free(instruction_arguments);
			//Avisar a kernel que la hizo
			//instruction_made* REALIZADA;
	//chequear si puede realizar instruccion
	 if(strcmp(nombre_instruccion, "IO_GEN_SLEEP") =! 0){
		log_info(MODULE_LOGGER, "No puedo realizar esa instruccion");
		 No puede realizar: 
			Avisa a kernel y se hace cargo kernel
		 }
		else{ 


	//si es IO_STDIN_READ realizarlo

	//avisar a kernel
}
	}
}*/

/*int IO_STDIN_READ(int argc, char* argv[]){

	
	//char* text = malloc(sizeof(registroTamanio));
	printf("Ingrese un texto: ");
	scanf("%s",text);

	//enviar el texto a memoria
	//send_to_memory(text,registroDireccion);
}

void* stdout_function(){
	//conectar a kernel

	//escuchar peticion siempre

	//recibe peticion

	//chequear si puede realizar instruccion

		/* No puede realizar: 
			Avisa a kernel y se hace cargo kernel
		*/	

	//si es IO_STDOUT_WRITE realizarlo

	//avisar a kernel
//}

/* void IO_STDOUT_WRITE(void* registroDireccion, void* direccionTamanio){
	int dir_memoria = receive_from_memory(direccionMemoria);
	printf("El valor hallado en la direccion de memoria es: %d", dir_memoria);
} */

/* int receive_from_memory(void* direccionMemoria){
	recv(CONNECTION_MEMORY.fd_connection,,,MSG_WAITALL);
} 

void initialize_configs_io(char* path) {
	MODULE_CONFIG = config_create(path);

	if(MODULE_CONFIG == NULL) {
		log_error(MODULE_LOGGER, "No se pudo abrir el archivo config del modulo %s: %s", MODULE_NAME, MODULE_CONFIG_PATHNAME);
        exit(EXIT_FAILURE);
	}

	read_module_config(MODULE_CONFIG);
};

typedef enum instruction_made { // CONTEXT_SWITCH_CAUSE
    REALIZADA, // por ejemplo decode
    NO_REALIZADA, //incluye el EXIT
    
} instruction_made;*/