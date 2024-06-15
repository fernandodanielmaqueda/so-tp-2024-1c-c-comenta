/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

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

t_IO_Type IO_TYPES[] = {
    {.name = "GENERIC", .type = GENERIC_IO_TYPE, .function = generic_function },
    {.name = "STDIN", .type = STDIN_IO_TYPE, .function = stdin_function },
    {.name = "STDOUT", .type = STDOUT_IO_TYPE, .function = stdout_function },
    {.name = "DIALFS", .type = DIALFS_IO_TYPE, .function = NULL},
    {.name = NULL}
};

t_IO_Type *IO_TYPE;

t_IO_Operation IO_OPERATIONS[] = {
    {.name = "IO_GEN_SLEEP", .function = io_gen_sleep_io_operation},
    {.name = "IO_STDIN_READ", .function = io_stdin_read_io_operation},
    {.name = "IO_STDOUT_WRITE", .function = io_stdout_write_io_operation},
    {.name = "IO_FS_CREATE", .function = io_fs_create_io_operation},
    {.name = "IO_FS_DELETE", .function = io_fs_delete_io_operation},
    {.name = "IO_FS_TRUNCATE", .function = io_fs_truncate_io_operation},
    {.name = "IO_FS_WRITE", .function = io_fs_write_io_operation},
    {.name = "IO_FS_READ", .function = io_fs_read_io_operation},
    {.name = NULL}
};

t_IO_Operation *IO_OPERATION;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs(argv[2]);
	initialize_sockets();

	log_info(MODULE_LOGGER, "el parametro 1 es: %s", argv[1]);
	log_info(MODULE_LOGGER, "el parametro 2 es: %s", argv[2]);
	log_info(MODULE_LOGGER, "El tipo del modulo es: %s",TIPO_INTERFAZ);

	IO_TYPE = io_type_find(TIPO_INTERFAZ);
	if(IO_TYPE == NULL) {
		log_error(MODULE_LOGGER, "No se reconoce el tipo de interfaz");
		exit(EXIT_FAILURE);
	}

	// Invoco a la funcion que corresponda
	IO_TYPE->function();

    log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();
   
    return EXIT_SUCCESS;
}

t_IO_Type *io_type_find(char *name) {
    for(register int i = 0; IO_TYPES[i].name != NULL; i++)
        if(!strcmp(IO_TYPES[i].name, name))
            return (&IO_TYPES[i]);

    return NULL;
}

t_IO_Operation *io_operation_find(char *name) {
    for(register int i = 0; IO_OPERATIONS[i].name != NULL; i++)
        if(!strcmp(IO_OPERATIONS[i].name, name))
            return (&IO_OPERATIONS[i]);

    return NULL;
}

void read_module_config(t_config* MODULE_CONFIG) {
    TIPO_INTERFAZ = config_get_string_value(MODULE_CONFIG, "TIPO_INTERFAZ");
    TIEMPO_UNIDAD_TRABAJO = config_get_int_value(MODULE_CONFIG, "TIEMPO_UNIDAD_TRABAJO");
    CONNECTION_KERNEL = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = KERNEL_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_KERNEL"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_KERNEL")};
    CONNECTION_MEMORY = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
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

void generic_function(void) {

	//escuchar peticion siempre
	t_Package* package;
	t_Arguments* instruction;
	//recibe peticion
	while(1) {

		package = package_receive(CONNECTION_KERNEL.fd_connection);
		instruction = arguments_deserialize(package->payload);
		package_destroy(package);

		IO_OPERATION = io_operation_find(instruction->argv[0]);
		if(IO_OPERATION == NULL) {
			log_error(MODULE_LOGGER, "No se reconoce la operacion");
			exit(EXIT_FAILURE);
		}

		int exit_status = IO_OPERATION->function(instruction->argc, instruction->argv);

		// arguments_free(instruction);

		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
		package = package_create_with_header(EXIT_STATUS_HEADER);
		payload_enqueue(package->payload, &(exit_status), sizeof(uint8_t));
		package_send(package, CONNECTION_KERNEL.fd_connection);
		package_destroy(package);
	}
}

void stdin_function(){

	t_Package* package;
	t_Arguments* instruction;
	//escuchar peticion siempre
	while(1){
		//recibe peticion
		package = package_receive(CONNECTION_KERNEL.fd_connection);
		instruction = arguments_deserialize(package->payload);
		
		package_destroy(package);
		IO_OPERATION = io_operation_find(instruction->argv[0]);
		if(IO_OPERATION == NULL){
			log_error(MODULE_LOGGER,"No se reconoce la operacion");
			exit(EXIT_FAILURE);
		}
		int exit_status = IO_OPERATION->function(instruccion->argc, instruction->argv);
		// arguments_free(instruction);

		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
		package = package_create_with_header(EXIT_STATUS_HEADER);
		payload_enqueue(package->payload, &(exit_status), sizeof(uint8_t));
		package_send(package, CONNECTION_KERNEL.fd_connection);
		package_destroy(package);
}
}

int IO_STDIN_READ(int argc, char* argv[]){

	char* text = malloc(sizeof(argv[2]));
	
	//char* text = malloc(sizeof(registroTamanio));
	printf("Ingrese un texto: ");
}

void stdout_function(){
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


typedef enum instruction_made { // CONTEXT_SWITCH_CAUSE
    REALIZADA, // por ejemplo decode
    NO_REALIZADA, //incluye el EXIT
    
} instruction_made;


int io_gen_sleep_io_operation(int argc, char *argv[]) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

	switch(IO_TYPE->type) {
		case GENERIC_IO_TYPE:
		// LA PUEDE HACER (Y LA HACE)
			sleep(atoi(argv[2]) * TIEMPO_UNIDAD_TRABAJO);
			break;
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}

int io_stdin_read_io_operation(int argc, char *argv[]) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDIN_READ <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

	switch(IO_TYPE->type){
		case STDIN_IO_TYPE:
		break;
	default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return EXIT_FAILURE;
	}	

	return EXIT_SUCCESS;	
}

int io_stdout_write_io_operation(int argc, char *argv[]) {
    
    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

    // TODO
    
    return EXIT_SUCCESS;
}

int io_fs_create_io_operation(int argc, char *argv[]) {
    
    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_delete_io_operation(int argc, char *argv[]) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_DELETE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_truncate_io_operation(int argc, char *argv[]) {

    if (argc != 4)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_TRUNCATE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);
    
    return EXIT_SUCCESS;
}

int io_fs_write_io_operation(int argc, char *argv[]) {
    if(argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return EXIT_SUCCESS;
}

int io_fs_read_io_operation(int argc, char *argv[]) {
    if(argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return EXIT_SUCCESS;
}