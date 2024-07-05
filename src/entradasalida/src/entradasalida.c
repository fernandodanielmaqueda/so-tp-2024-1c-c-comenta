/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

char *MODULE_NAME = "entradasalida";
char *MODULE_LOG_PATHNAME = "entradasalida.log";

t_log *MODULE_LOGGER;
t_config *MODULE_CONFIG;

int UNIT_WORK_TIME;

t_Connection CONNECTION_KERNEL;
t_Connection CONNECTION_MEMORY;

char *PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;
int COMPRESSION_DELAY;

t_IO_Type IO_TYPES[] = {
    [GENERIC_IO_TYPE] = {.name = "GENERIC", .function = generic_function },
    [STDIN_IO_TYPE] = {.name = "STDIN", .function = stdin_function },
    [STDOUT_IO_TYPE] = {.name = "STDOUT", .function = stdout_function },
    [DIALFS_IO_TYPE] = {.name = "DIALFS", .function = NULL}
};

enum e_IO_Type IO_TYPE;

t_IO_Operation IO_OPERATIONS[] = {
    [IO_GEN_SLEEP_CPU_OPCODE] = {.name = "IO_GEN_SLEEP" , .function = io_gen_sleep_io_operation},
    [IO_STDIN_READ_CPU_OPCODE] = {.name = "IO_STDIN_READ" , .function = io_stdin_read_io_operation},
    [IO_STDOUT_WRITE_CPU_OPCODE] = {.name = "IO_STDOUT_WRITE" , .function = io_stdout_write_io_operation},
    [IO_FS_CREATE_CPU_OPCODE] = {.name = "IO_FS_CREATE" , .function = io_fs_create_io_operation},
    [IO_FS_DELETE_CPU_OPCODE] = {.name = "IO_FS_DELETE" , .function = io_fs_delete_io_operation},
    [IO_FS_TRUNCATE_CPU_OPCODE] = {.name = "IO_FS_TRUNCATE" , .function = io_fs_truncate_io_operation},
    [IO_FS_WRITE_CPU_OPCODE] = {.name = "IO_FS_WRITE" , .function = io_fs_write_io_operation},
    [IO_FS_READ_CPU_OPCODE] = {.name = "IO_FS_READ" , .function = io_fs_read_io_operation},
};

int module(int argc, char *argv[]) {

	if(argc != 2) {
		log_error(MODULE_LOGGER, "Uso: <NOMBRE_INTERFAZ> <ARCHIVO_DE_CONFIGURACION>");
		exit(EXIT_FAILURE);
	}

	initialize_loggers();
	initialize_configs(argv[2]);

	initialize_sockets();

	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	receive_expected_header(INTERFACE_NAME_REQUEST_HEADER, CONNECTION_KERNEL.fd_connection);
	send_text_with_header(INTERFACE_NAME_REQUEST_HEADER, argv[1], CONNECTION_KERNEL.fd_connection);

	// Invoco a la funcion que corresponda
	IO_TYPES[IO_TYPE].function();

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();
   
    return EXIT_SUCCESS;
}

int io_type_find(char *name, e_IO_Type *destination) {
    if(name == NULL || destination == NULL)
        return 1;
    
    size_t io_types_number = sizeof(IO_TYPES) / sizeof(IO_TYPES[0]);
    for (register e_IO_Type io_type = 0; io_type < io_types_number; io_type++)
        if (!strcmp(IO_TYPES[io_type].name, name)) {
            *destination = io_type;
            return 0;
        }

    return 1;
}

int io_operation_execute(t_Payload *operation) {

	t_EnumValue aux;

    payload_dequeue(operation, &aux, sizeof(t_EnumValue));
		e_CPU_OpCode io_opcode = (e_CPU_OpCode) aux;

    if(IO_OPERATIONS[io_opcode].function == NULL) {
        log_error(MODULE_LOGGER, "Funcion de operacion de IO no encontrada");
        return 1;
    }

    return IO_OPERATIONS[io_opcode].function(operation);
}

void read_module_config(t_config* MODULE_CONFIG) {
    char *io_type_name = config_get_string_value(MODULE_CONFIG, "TIPO_INTERFAZ");
	if(io_type_find(io_type_name, &IO_TYPE)) {
		log_error(MODULE_LOGGER, "%s: No se reconoce el TIPO_INTERFAZ", io_type_name);
		exit(EXIT_FAILURE);
	}

	CONNECTION_KERNEL = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = KERNEL_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_KERNEL"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_KERNEL")};

	switch(IO_TYPE) {
		case GENERIC_IO_TYPE:
		    UNIT_WORK_TIME = config_get_int_value(MODULE_CONFIG, "TIEMPO_UNIDAD_TRABAJO");
			break;
		case STDIN_IO_TYPE:
		case STDOUT_IO_TYPE:
			CONNECTION_MEMORY = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
			break;
		case DIALFS_IO_TYPE:
			UNIT_WORK_TIME = config_get_int_value(MODULE_CONFIG, "TIEMPO_UNIDAD_TRABAJO");
			CONNECTION_MEMORY = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
			PATH_BASE_DIALFS = config_get_string_value(MODULE_CONFIG, "PATH_BASE_DIALFS");
			BLOCK_SIZE = config_get_int_value(MODULE_CONFIG, "BLOCK_SIZE");
			BLOCK_COUNT = config_get_int_value(MODULE_CONFIG, "BLOCK_COUNT");
			COMPRESSION_DELAY = config_get_int_value(MODULE_CONFIG, "RETRASO_COMPACTACION");
			break;
	}
}

void initialize_sockets(void) {
	pthread_t thread_io_connect_to_kernel;
	pthread_t thread_io_connect_to_memory;

	// [Client] Entrada/Salida -> [Server] Kernel
	pthread_create(&thread_io_connect_to_kernel, NULL, client_thread_connect_to_server, (void*) &CONNECTION_KERNEL);
	// [Client] Entrada/Salida -> [Server] Memoria
	pthread_create(&thread_io_connect_to_memory, NULL, client_thread_connect_to_server, (void*) &CONNECTION_MEMORY);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_io_connect_to_kernel, NULL);
	pthread_join(thread_io_connect_to_memory, NULL);
}

void finish_sockets(void) {
	close(CONNECTION_KERNEL.fd_connection);
	close(CONNECTION_MEMORY.fd_connection);
}

void generic_function(void) {

	//escuchar peticion siempre
	t_Package *package;
	t_Payload *instruction = NULL;
	//recibe peticion
	while(1) {

		package = package_receive(CONNECTION_KERNEL.fd_connection);
		subpayload_deserialize(package->payload, instruction);
		package_destroy(package);

		int exit_status = io_operation_execute(instruction);

		// arguments_free(instruction);

		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
	}
}

void stdin_function(){

	t_Package *package;
	t_Payload *instruction = NULL;
	//escuchar peticion siempre
	while(1){
		//recibe peticion
		package = package_receive(CONNECTION_KERNEL.fd_connection);
		subpayload_deserialize(package->payload, instruction);
		package_destroy(package);

		int exit_status = io_operation_execute(instruction);
		// arguments_free(instruction);

		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
}
}


void stdout_function(){
	t_Package *package;
	t_Payload *instruction = NULL;

	//escuchar peticion siempre
	while(1) {
		//recibe peticion
		package = package_receive(CONNECTION_KERNEL.fd_connection);
		subpayload_deserialize(package->payload, instruction);
		package_destroy(package);

		int exit_status = io_operation_execute(instruction);
		// arguments_free(instruction);

		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
	}
}

// IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>
int io_gen_sleep_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

	switch(IO_TYPE) {
		case GENERIC_IO_TYPE:
		// LA PUEDE HACER (Y LA HACE)
			//sleep(atoi(argv[2]) * UNIT_WORK_TIME);
			break;
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return 1;
	}

    return 0;
}

int io_stdin_read_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

	t_Package *package;
	uint32_t registro_direccion;
	int registro_tamanio;

	switch(IO_TYPE){
		case STDIN_IO_TYPE:
			//registro_direccion = atoi(argv[2]);
			//registro_tamanio = atoi(argv[3]);
			//char* text = malloc(registro_tamanio * sizeof(char));
			//log_info(MODULE_LOGGER, "Ingrese un texto: ");
			//fgets(text, registro_tamanio * sizeof(char), stdin);
			//int pid = 0;


			//send_write_request( pid,  registro_direccion,  text, CONNECTION_MEMORY.fd_connection, IO_STDIN_WRITE_MEMORY);
			/*
			package = package_create_with_header(STRING_HEADER);
			payload_enqueue(package->payload, &(registro_direccion), sizeof(registro_direccion)); // YA SE MANDA EL TAMANIO JUNTO CON EL STRING
			text_serialize(package->payload, text); // YA SE MANDA EL TAMANIO JUNTO CON EL STRING
			package_send(package, CONNECTION_MEMORY.fd_connection);
			package_destroy(package);
			*/

			//free(text);

			break;
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return 1;
	}

	return 0;	
}

int io_stdout_write_io_operation(t_Payload *operation) {
    
    // log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

	t_Package *package;
	//t_Arguments* instruction;
	//int registro_tamanio;

	switch(IO_TYPE){
		case STDOUT_IO_TYPE:
/*
			package = package_create_with_header(STRING_HEADER);
			text_serialize(package->payload, argv[2]);
			text_serialize(package->payload, argv[3]);

			package_send(package, CONNECTION_MEMORY.fd_connection);
			package_destroy(package);
*/
			//int pid = 0;			
			//int bytes = atoi(argv[2]);
			//int dir_fis = atoi(argv[3]);
			//send_read_request(pid, dir_fis, bytes, CONNECTION_MEMORY.fd_connection, IO_STDOUT_READ_MEMORY);

			//char* mensaje;
			//package = package_receive(CONNECTION_MEMORY.fd_connection);
			//receive_String_1int(&pid, &mensaje, package->payload);

			//instruction = arguments_deserialize(package->payload);
			//log_info(MODULE_LOGGER, "En la memoria se halla el siguiente contenido: %s", mensaje);
			//package_destroy(package);

			//fputs(mensaje, stdout);

			break;
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return 1;
	}
    
    return 0;
}

int io_fs_create_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    return 0;
}

int io_fs_delete_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    return 0;
}

int io_fs_truncate_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);
    
    return 0;
}

int io_fs_write_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return 0;
}

int io_fs_read_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return 0;
}