/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

char *INTERFACE_NAME;

char *MODULE_NAME = "entradasalida";
char *MODULE_LOG_PATHNAME = "entradasalida.log";

t_log *MODULE_LOGGER;
t_config *MODULE_CONFIG;

t_Connection CONNECTION_KERNEL;
t_Connection CONNECTION_MEMORY;

int WORK_UNIT_TIME;

char *PATH_BASE_DIALFS;
size_t BLOCK_SIZE;
size_t BLOCK_COUNT;
int COMPRESSION_DELAY;
FILE* FILE_BLOCKS;
FILE* FILE_METADATA;
FILE* FILE_BITMAP;
char* PTRO_BITMAP;
size_t BITMAP_SIZE;

t_list *LIST_FILES;
t_bitarray *BITMAP;
char* PTRO_BLOCKS;
size_t BLOCKS_TOTAL_SIZE;

t_IO_Type IO_TYPES[] = {
    [GENERIC_IO_TYPE] = {.name = "GENERICA", .function = generic_interface_function },
    [STDIN_IO_TYPE] = {.name = "STDIN", .function = stdin_interface_function },
    [STDOUT_IO_TYPE] = {.name = "STDOUT", .function = stdout_interface_function },
    [DIALFS_IO_TYPE] = {.name = "DIALFS", .function = dialfs_interface_function}
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

t_PID PID;

int module(int argc, char *argv[]) {

	if(argc != 3) {
		fprintf(stderr, "Uso: %s <NOMBRE_INTERFAZ> <ARCHIVO_DE_CONFIGURACION>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	INTERFACE_NAME = argv[1];

	initialize_loggers();
	initialize_configs(argv[2]);

	initialize_sockets();

	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	t_Return_Value return_value;
	if(receive_expected_header(INTERFACE_DATA_REQUEST_HEADER, CONNECTION_KERNEL.fd_connection)) {
		// TODO
		exit(1);
	}	
	if(send_interface_data(INTERFACE_NAME, IO_TYPE, CONNECTION_KERNEL.fd_connection)) {
		// TODO
		exit(1);
	}
	if(receive_return_value_with_expected_header(INTERFACE_DATA_REQUEST_HEADER, &return_value, CONNECTION_KERNEL.fd_connection)) {
		// TODO
		exit(1);
	}
	
	if(return_value) {
		log_error(MODULE_LOGGER, "No se pudo registrar la interfaz %s en el Kernel", INTERFACE_NAME);
		exit(EXIT_FAILURE);
	}

	// Invoco a la función que corresponda
	IO_TYPES[IO_TYPE].function();

	t_Payload io_operation;
	//escuchar peticion siempre
	while(1) {
		if(receive_io_operation_dispatch(&PID, &io_operation, CONNECTION_KERNEL.fd_connection)) {
			exit(1);
		}

		t_Return_Value return_value = (t_Return_Value) io_operation_execute(&io_operation);

		if(send_io_operation_finished(PID, return_value, CONNECTION_KERNEL.fd_connection)) {
			exit(1);
		}
	}

	free_bitmap_blocks();
	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();
   
    return EXIT_SUCCESS;
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
		    WORK_UNIT_TIME = config_get_int_value(MODULE_CONFIG, "TIEMPO_UNIDAD_TRABAJO");
			break;
		case STDIN_IO_TYPE:
		case STDOUT_IO_TYPE:
			CONNECTION_MEMORY = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
			break;
		case DIALFS_IO_TYPE:
			WORK_UNIT_TIME = config_get_int_value(MODULE_CONFIG, "TIEMPO_UNIDAD_TRABAJO");
			CONNECTION_MEMORY = (t_Connection) {.client_type = IO_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
			PATH_BASE_DIALFS = config_get_string_value(MODULE_CONFIG, "PATH_BASE_DIALFS");
			BLOCK_SIZE = config_get_int_value(MODULE_CONFIG, "BLOCK_SIZE");
			BLOCK_COUNT = config_get_int_value(MODULE_CONFIG, "BLOCK_COUNT");
			COMPRESSION_DELAY = config_get_int_value(MODULE_CONFIG, "RETRASO_COMPACTACION");
			break;
	}
}

int io_type_find(char *name, e_IO_Type *destination) {
    if(name == NULL || destination == NULL)
        return 1;
    
    size_t io_types_number = sizeof(IO_TYPES) / sizeof(IO_TYPES[0]);
    for (register e_IO_Type io_type = 0; io_type < io_types_number; io_type++)
        if (strcmp(IO_TYPES[io_type].name, name) == 0) {
            *destination = io_type;
            return 0;
        }

    return 1;
}

void initialize_sockets(void) {
	

	// [Client] Entrada/Salida -> [Server] Kernel
	pthread_t thread_io_connect_to_kernel;
	pthread_create(&thread_io_connect_to_kernel, NULL, (void *(*)(void *)) client_thread_connect_to_server, (void *) &CONNECTION_KERNEL);
	switch(IO_TYPE) {
		case GENERIC_IO_TYPE:
			break;
		case STDIN_IO_TYPE:
		case STDOUT_IO_TYPE:
		case DIALFS_IO_TYPE:
		{
			// [Client] Entrada/Salida -> [Server] Memoria
			pthread_t thread_io_connect_to_memory;
			pthread_create(&thread_io_connect_to_memory, NULL, (void *(*)(void *)) client_thread_connect_to_server, (void *) &CONNECTION_MEMORY);
			pthread_join(thread_io_connect_to_memory, NULL);
			break;
		}
	}
	
	pthread_join(thread_io_connect_to_kernel, NULL);
}

void finish_sockets(void) {
	close(CONNECTION_KERNEL.fd_connection);
	close(CONNECTION_MEMORY.fd_connection);
}

void generic_interface_function(void) {
	
}

void stdin_interface_function(void) {

}

void stdout_interface_function(void) {

}

void dialfs_interface_function(void) {
	initialize_blocks();
	initialize_bitmap();
	LIST_FILES = list_create();
}

int io_operation_execute(t_Payload *io_operation) {

	e_CPU_OpCode io_opcode;
	cpu_opcode_deserialize(io_operation, &io_opcode);

    if(IO_OPERATIONS[io_opcode].function == NULL) {
		payload_destroy(io_operation);
        log_warning(MODULE_LOGGER, "Funcion de operacion de IO no encontrada");
        return 1;
    }

    int exit_status = IO_OPERATIONS[io_opcode].function(io_operation);
	payload_destroy(io_operation);
	return exit_status;
}

int io_gen_sleep_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != GENERIC_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

	uint32_t work_units;
	payload_shift(operation_arguments, &work_units, sizeof(work_units));

	log_debug(MINIMAL_LOGGER, "PID: <%d> - OPERACION <IO_GEN_SLEEP>", (int) PID);

	usleep(WORK_UNIT_TIME * work_units * 1000);

    return 0;
}

int io_stdin_read_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != STDIN_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}
			
	t_list *physical_addresses = list_create();
	t_MemorySize bytes;
	char* text_to_send[bytes];
	char* pointer_verifier;
	int char_to_verify = '\n';

	//Empiezo a "desencolar" el payload recibido
	payload_shift(operation_arguments, &PID, sizeof(PID));
	list_deserialize(operation_arguments, physical_addresses, physical_address_deserialize_element);
	payload_shift(operation_arguments, &bytes, sizeof(bytes));

	//Aviso que operacion voy a hacer
	log_debug(MINIMAL_LOGGER, "PID: <%d> - OPERACION <IO_STDIN_READ>", (int) PID);

	log_info(MODULE_LOGGER, "Escriba una cadena de %d caracteres", (int)bytes);
	fgets(*text_to_send, bytes + 1, stdin);
	
	/* 	while(strlen(text_to_send) != bytes){
		log_info("Escriba una cadena de %d caracteres", (int)bytes);
		fgets(text_to_send, bytes + 1, stdin);		
	} */

	pointer_verifier = strchr(*text_to_send, char_to_verify);

	while(pointer_verifier != NULL){
		log_info(MODULE_LOGGER, "Escriba una cadena de %d caracteres", (int) bytes);
		fgets(*text_to_send, bytes + 1, stdin);
		pointer_verifier = strchr(*text_to_send, char_to_verify);
	}

	//Creo paquete y argumentos necesarios para enviarle a memoria
	t_Package *package = package_create_with_header(IO_STDIN_WRITE_MEMORY);
	payload_append(&(package->payload), &PID, sizeof(PID));
	list_serialize(&(package->payload), *physical_addresses, physical_address_deserialize_element);
	payload_append(&(package->payload), &bytes, sizeof(bytes));
	payload_append(&(package->payload), text_to_send, sizeof(text_to_send));
	package_send(package, CONNECTION_MEMORY.fd_connection);
	package_destroy(package);

	//Recibo si salio bien la operacion
	receive_return_value_with_expected_header(WRITE_REQUEST, 0, CONNECTION_MEMORY.fd_connection);

	free(pointer_verifier);
	
	return 0;	
}

int io_stdout_write_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != STDOUT_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}
			
	t_list *physical_addresses = list_create();
	t_MemorySize bytes;
	char* text_received = NULL;

	//empiezo a "desencolar" el payload recibido
	payload_shift(operation_arguments, &PID, sizeof(PID));
	//text_deserialize(operation_arguments, &text_received);
	list_deserialize(operation_arguments, physical_addresses, physical_address_deserialize_element);
	payload_shift(operation_arguments, &bytes, sizeof(bytes));

	//Aviso la operacion que voy a hacer
	log_debug(MINIMAL_LOGGER, "PID: <%d> - OPERACION <IO_STDOUT_WRITE>", (int) PID);

	t_Package* package;

	//Creo header para memoria y el paquete con los argumentos
	package = package_create_with_header(IO_STDOUT_READ_MEMORY);
	payload_append(&(package->payload), &PID, sizeof(PID));
	list_serialize(&(package->payload), *physical_addresses, physical_address_deserialize_element);
	payload_append(&(package->payload), &bytes, sizeof(bytes));
	package_send(package, CONNECTION_MEMORY.fd_connection);
	package_destroy(package);
	
	//Recibo nuevo paquete para imprimir por pantalla
	package_receive(&package, CONNECTION_MEMORY.fd_connection);
	//Desencolar e imprimir por pantalla
	payload_shift(&(package->payload), text_received, bytes);

	fputs(text_received, stdout);
	free(text_received);
    
    return 0;
}

int io_fs_create_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_CREATE recibido.");
	char* file_name;
	t_PID op_pid;

	usleep(WORK_UNIT_TIME * 1000);
    payload_shift(operation_arguments, &op_pid, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
	uint32_t location = seek_first_free_block();

	//Crear variable de control de archivo
	t_FS_File* new_entry = NULL;
	strcpy(new_entry->name , file_name);
	new_entry->process_pid = op_pid;
	new_entry->initial_bloq = location;
	new_entry->len = 1;
	new_entry->size = 0;

	create_file(file_name, location); //Creo archivo y lo seteo
	bitarray_set_bit(BITMAP, location);

	list_add(LIST_FILES, new_entry);

    log_debug(MINIMAL_LOGGER, "PID: <%d> - Crear archivo: <%s>", (int) op_pid, file_name);

/* 	t_Package* respond = package_create_with_header(IO_FS_CREATE_CPU_OPCODE);
	payload_append(respond->payload, &op_pid, sizeof(t_PID));
	package_send(respond, CONNECTION_KERNEL.fd_connection);
	package_destroy(respond); */

    return 0;
}

int io_fs_delete_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

	t_PID op_pid = 0;
	char* file_name = NULL;

	usleep(WORK_UNIT_TIME * 1000);
    payload_shift(operation_arguments, &op_pid, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
	
	uint32_t size = list_size(LIST_FILES);

	if(size > 0){
		t_FS_File* file = list_get(LIST_FILES,0);
		size_t file_target = -1;

		for (size_t i = 0; i < size; i++) //busqueda del file indicado
		{
			t_FS_File* file = list_get(LIST_FILES,i);
			if (strcmp(file->name, file_name)){
				i = size;
				file_target = i;
			}
		}

		if(file_target == -1) {
			log_info(MODULE_LOGGER, "[ERROR] PID: <%d> - Archivo <%s> a eliminar no encontrado", (int) op_pid, file_name);
			return 1;
		}

		//Liberacion del bitarray
		uint32_t initial_pos = file->initial_bloq;
		for (size_t i = 0; i < file->len; i++)
		{
			bitarray_clean_bit(BITMAP, initial_pos);
			initial_pos++;
		}

		if (msync(PTRO_BITMAP, BITMAP_SIZE, MS_SYNC) == -1) {
        	log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bloques.dat con el archivo: %s", strerror(errno));
        	exit(EXIT_FAILURE);
    	}

		list_remove(LIST_FILES, file_target);
		update_file(file_name,0,-1);
	}
	
    log_debug(MINIMAL_LOGGER, "PID: <%d> - Eliminar archivo: <%s>", (int) op_pid, file_name);
	
/* 	t_Package* respond = package_create_with_header(IO_FS_DELETE_CPU_OPCODE);
	payload_append(respond->payload, &op_pid, sizeof(t_PID));
	package_send(respond, CONNECTION_KERNEL.fd_connection);
	package_destroy(respond); */

    return 0;
}

int io_fs_truncate_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_TRUNCATE recibido.");

	char* file_name = NULL;
	char* value = NULL;
	t_PID op_pid = 0;
	
	usleep(WORK_UNIT_TIME * 1000);
    payload_shift(operation_arguments, &op_pid, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
    text_deserialize(operation_arguments, &(value));

	uint32_t valueSize = atoi(value);
	uint32_t valueNUM = ceil(valueSize/BLOCK_SIZE);

	t_FS_File* file = seek_file(file_name);
	uint32_t initial_pos = file->initial_bloq + file->len;
	if (file->len > valueNUM)
	{//Se restan bloques
		size_t diff = file->len - valueNUM;
		for (size_t i = 0; i < diff; i++)
		{
			bitarray_clean_bit(BITMAP, initial_pos);
			initial_pos--;
		}
		file->len = valueNUM;
		file->size = valueSize;
	}
	if (file->len < valueNUM)
	{// Se agregan bloques
		size_t diff = valueNUM - file->len;
		if(can_assign_block(file->initial_bloq, file->len, valueNUM)){

			for (size_t i = 0; i < diff; i++)
			{
				bitarray_set_bit(BITMAP, initial_pos);
				initial_pos++;
			}
			file->len = valueNUM;
			file->size = valueSize;

		}
		else if(quantity_free_blocks() >= valueNUM){//VERIFICA SI COMPACTAR SOLUCIONA EL PROBLEMA

			log_info(MINIMAL_LOGGER, "PID: <%d> - Inicio Compactacion", op_pid);
			compact_blocks();
			log_info(MINIMAL_LOGGER, "PID: <%d> - Fin Compactacion", op_pid);

			initial_pos = file->initial_bloq + file->len;
			for (size_t i = 0; i < diff; i++)
			{
				bitarray_set_bit(BITMAP, initial_pos);
				initial_pos++;
			}
			file->len = valueNUM;
			file->size = valueSize;

		}
		else{
        	log_error(MODULE_LOGGER, "[FS] ERROR: OUT_OF_MEMORY --> Can't assing blocks");
			return 1;
		}
	}

	update_file(file_name,valueSize,file->initial_bloq);
	
		if (msync(PTRO_BITMAP, BITMAP_SIZE, MS_SYNC) == -1) {
        	log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bloques.dat con el archivo: %s", strerror(errno));
        	exit(EXIT_FAILURE);
    	}
	
    log_debug(MINIMAL_LOGGER, "PID: <%d> - Truncar archivo: <%s> - Tamaño: <%s>", (int) op_pid, file_name, value);
	
/* 	t_Package* respond = package_create_with_header(IO_FS_TRUNCATE_CPU_OPCODE);
	payload_append(respond->payload, &op_pid, sizeof(t_PID));
	package_send(respond, CONNECTION_KERNEL.fd_connection);
	package_destroy(respond); */
    
    return 0;
}

int io_fs_write_io_operation(t_Payload *operation_arguments) {

/*O_FS_WRITE (Interfaz, Nombre Archivo, Registro Dirección, Registro Tamaño, Registro
Puntero Archivo): Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se
lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la
dirección lógica que se encuentra en el Registro Dirección y se escriban en el archivo a partir
del valor del Registro Puntero Archivo.*/

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_READ recibido.");

	char* file_name = NULL;
	t_MemorySize ptro = 0;
	t_MemorySize bytes = 0;
	t_PID op_pid = 0;
	t_list* list_dfs = list_create();

	usleep(WORK_UNIT_TIME * 1000);
	//Leo el payload recibido de kernel
    payload_shift(operation_arguments, &op_pid, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
    payload_shift(operation_arguments, &ptro, sizeof(t_MemorySize));
    payload_shift(operation_arguments, &bytes, sizeof(t_MemorySize));
	list_deserialize(operation_arguments, list_dfs, physical_address_deserialize_element);

	
	//Busco el file buscado
	t_FS_File* file = seek_file(file_name);
	//uint32_t block_initial = (uint32_t) floor(ptro / BLOCK_SIZE);
	uint32_t block_initial = file->initial_bloq;

	//Envio paquete a memoria
	t_Package* pack_request = package_create_with_header(IO_FS_WRITE_MEMORY);
	payload_append(&pack_request->payload, &op_pid, sizeof(t_PID));
    list_serialize(&pack_request->payload, *list_dfs, physical_address_serialize_element);
	payload_append(&pack_request->payload, &bytes, sizeof(bytes));
	package_send(pack_request,CONNECTION_MEMORY.fd_connection);
	package_destroy(pack_request);

	//Recibo respuesta de memoria con el contenido
	t_Package* package_memory = NULL;
	package_receive(&package_memory, CONNECTION_MEMORY.fd_connection);
	void *posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + (block_initial * BLOCK_SIZE + ptro));
	//void *posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + (ptro));
    payload_shift(&package_memory->payload, posicion, (size_t) bytes);
	package_destroy(package_memory);
    //void* context = malloc(bytes);
    //memcpy(posicion, context, bytes); 
	//free(context);

    if(send_return_value_with_header(WRITE_REQUEST, 0, CONNECTION_KERNEL.fd_connection)) {
        // TODO
        exit(1);
    }

	log_debug(MINIMAL_LOGGER, "PID: <%d> - Escribir Archivo: <%s> - Tamaño a Escribir: <%d> - Puntero Archivo: <%d>",
				 (int) PID, file_name, (int)bytes, (int)ptro);

	
    if (msync(PTRO_BLOCKS, BLOCKS_TOTAL_SIZE, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bloques.dat con el archivo: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
	
    return 0;
}

int io_fs_read_io_operation(t_Payload *operation_arguments) {

	/*O_FS_READ (Interfaz, Nombre Archivo, Registro Dirección, Registro Tamaño, Registro
Puntero Archivo): Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se
lea desde el archivo a partir del valor del Registro Puntero Archivo la cantidad de bytes
indicada por Registro Tamaño y se escriban en la Memoria a partir de la dirección lógica
indicada en el Registro Dirección*/

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_READ recibido.");

	char* file_name = NULL;
	t_MemorySize ptro = 0;
	t_MemorySize bytes = 0;
	t_PID op_pid = 0;
	t_list* list_dfs = list_create();

	usleep(WORK_UNIT_TIME * 1000);
	//Leo el payload recibido
    payload_shift(operation_arguments, &op_pid, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
    payload_shift(operation_arguments, &ptro, sizeof(t_MemorySize));
    payload_shift(operation_arguments, &bytes, sizeof(t_MemorySize));
	list_deserialize(operation_arguments, list_dfs, physical_address_deserialize_element);

	//Busco el file buscado
	t_FS_File* file = seek_file(file_name);
	//uint32_t block_initial = (uint32_t) floor(ptro / BLOCK_SIZE);
	uint32_t block_initial = file->initial_bloq;
    //uint32_t offset = (uint32_t) (ptro - block_initial * BLOCK_SIZE);;
	//uint32_t block_quantity_required = seek_quantity_blocks_required(ptro, bytes);

    void* context = malloc(bytes);
	void *posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + (block_initial * BLOCK_SIZE + ptro));
	//void *posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + (ptro));
    memcpy(context, posicion, bytes); 
/* 	posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + ((location - free_spaces) * BLOCK_SIZE));
    memcpy(posicion, context, size); 
*/

	//Se crea paquete y se envia a memoria
	t_Package* pack_request = package_create_with_header(IO_FS_READ_MEMORY);
	payload_append(&pack_request->payload, &op_pid, sizeof(t_PID));
    list_serialize(&pack_request->payload, *list_dfs, physical_address_serialize_element);
	payload_append(&pack_request->payload, &bytes, sizeof(bytes));
	payload_append(&pack_request->payload, &context, sizeof(bytes));
	package_send(pack_request,CONNECTION_MEMORY.fd_connection);
	package_destroy(pack_request);

	free(context);

	log_debug(MINIMAL_LOGGER, "PID: <%d> - Leer Archivo: <%s> - Tamaño a Leer: <%d> - Puntero Archivo: <%d>",
				 (int) PID, file_name, (int)bytes, (int)ptro);

	if(receive_return_value_with_expected_header(WRITE_REQUEST,0,CONNECTION_MEMORY.fd_connection)){
		
        exit(1);
	}

    if(send_return_value_with_header(WRITE_REQUEST, 0, CONNECTION_KERNEL.fd_connection)) {
        // TODO
        exit(1);
    }

    return 0;
}

void initialize_blocks() {
    BLOCKS_TOTAL_SIZE = BLOCK_SIZE * BLOCK_COUNT;
    //size_t path_len_bloqs = strlen(PATH_BASE_DIALFS) + 1 +strlen("bitmap.dat"); //1 por la '/'
	char* path_file_blocks = string_new();
	strcpy (path_file_blocks, PATH_BASE_DIALFS);
	string_append(&path_file_blocks, "/");
	string_append(&path_file_blocks, "bloques.dat");

	//Checkeo si el file ya esta creado, sino lo elimino
	if (access(path_file_blocks, F_OK) == 0)remove(path_file_blocks);

    int fd = open(path_file_blocks, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        log_error(MODULE_LOGGER, "Error al abrir el archivo bloques.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, BLOCKS_TOTAL_SIZE) == -1) {
        log_error(MODULE_LOGGER, "Error al ajustar el tamaño del archivo bloques.dat: %s", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    PTRO_BLOCKS = mmap(NULL, BLOCKS_TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (PTRO_BLOCKS == MAP_FAILED) {
        log_error(MODULE_LOGGER, "Error al mapear el archivo bloques.dat a memoria: %s", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (msync(PTRO_BLOCKS, BLOCKS_TOTAL_SIZE, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bloques.dat con el archivo: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
/*
    if (munmap(BLOCKS_DATA, BLOCKS_TOTAL_SIZE) == -1) {
        log_error(MODULE_LOGGER, "Error al desmapear el archivo bloques.dat de la memoria: %s", strerror(errno));
    }
*/
    log_info(MODULE_LOGGER, "Bloques creados y mapeados correctamente.");
}


void initialize_bitmap() {
	BITMAP_SIZE = ceil(BLOCK_COUNT / 8);
	
    //size_t path_len_bloqs = strlen(PATH_BASE_DIALFS) + 1 +strlen("bitmap.dat"); //1 por la '/'
	char* path_file_bitmap = string_new();
	strcpy (path_file_bitmap, PATH_BASE_DIALFS);
	string_append(&path_file_bitmap, "/");
	string_append(&path_file_bitmap, "bitmap.dat");

	//Checkeo si el file ya esta creado, sino lo elimino
	if (access(path_file_bitmap, F_OK) == 0)remove(path_file_bitmap);
	
    int fd = open("bitmap.dat", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        log_error(MODULE_LOGGER, "Error al abrir el archivo bitmap.dat: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, BITMAP_SIZE) == -1) {
        log_error(MODULE_LOGGER, "Error al ajustar el tamaño del archivo bitmap.dat: %s", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    PTRO_BITMAP = mmap(NULL, BITMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (PTRO_BITMAP == MAP_FAILED) {
        log_error(MODULE_LOGGER, "Error al mapear el archivo bitmap.dat a memoria: %s", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    BITMAP = bitarray_create_with_mode((char *)PTRO_BITMAP, BITMAP_SIZE,LSB_FIRST);
    if (BITMAP == NULL) {
        log_error(MODULE_LOGGER, "Error al crear la estructura del bitmap");
        munmap(PTRO_BITMAP, BITMAP_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (msync(PTRO_BITMAP, BITMAP_SIZE, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bitmap.dat con el archivo: %s", strerror(errno));
    }
/*
    if (munmap(PTRO_BITMAP, BITMAP_SIZE) == -1) {
        log_error(MODULE_LOGGER, "Error al desmapear el archivo bitmap.dat de la memoria: %s", strerror(errno));
    }
*/
    log_info(MODULE_LOGGER, "Bitmap creado y mapeado correctamente.");
}

uint32_t seek_first_free_block(){
	int magic = 0;

	for (size_t i = 0; i < (BLOCK_COUNT +1); i++)
	{
		if(!(bitarray_test_bit(BITMAP, i))){
			magic = i;
			i = BLOCK_COUNT;
		}
	}

	return magic;
}

t_FS_File* seek_file(char* file_name){

	t_FS_File* file = list_get(LIST_FILES,0);

	for (size_t i = 0; i < list_size(LIST_FILES); i++)
	{
		
		t_FS_File* file = list_get(LIST_FILES,i);
		if (strcmp(file->name, file_name)) i = list_size(LIST_FILES);
	}
	
	return file;
}


bool can_assign_block(uint32_t initial_position, uint32_t len, uint32_t final_len){
	uint32_t addition = final_len - len;
	uint32_t final_pos = initial_position + len + addition;

	for (size_t i = (initial_position + len); i < final_pos; i++)
	{
		if(	bitarray_test_bit (BITMAP, i)) return false;
	}

	return true;
}

int quantity_free_blocks(){
	int magic = 0;

	if(BLOCK_COUNT == 0) return magic;

	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		if(	!(bitarray_test_bit (BITMAP, i))) magic++;
	}

	return magic;
}

uint32_t seek_quantity_blocks_required(uint32_t puntero, size_t bytes){
    uint32_t quantity_blocks = 0;

    uint32_t nro_page = (uint32_t) floor(puntero / BLOCK_SIZE);
    uint32_t offset = (uint32_t) (puntero - nro_page * BLOCK_SIZE);;

    if (offset != 0)
    {
        bytes -= (BLOCK_SIZE - offset);
        quantity_blocks++;
    }

    quantity_blocks += (uint32_t) floor(bytes / BLOCK_SIZE);
    
    return quantity_blocks;
}


void free_bitmap_blocks(){
	
    if (munmap(PTRO_BLOCKS, (BLOCK_SIZE * BLOCK_COUNT)) == -1) {
        log_error(MODULE_LOGGER, "Error al desmapear el archivo bloques.dat de la memoria: %s", strerror(errno));
    }

	free(BITMAP);

}
/*
void create_blocks(){
    size_t path_len_bloqs = strlen(PATH_BASE_DIALFS) + 1 +strlen("bloques.dat"); //1 por la '/'
	char* path_file_bloqs = string_new();
	strcpy (path_file_bloqs, PATH_BASE_DIALFS);
	string_append(&path_file_bloqs, "/");
	string_append(&path_file_bloqs, "bloques.dat");

	//Checkeo si el file ya esta creado, sino lo elimino
	if (access(path_file_bloqs, F_OK) == 0)remove(path_file_bloqs);

	FILE_BLOCKS = fopen(path_file_bloqs, "w+"); 
	
    if (FILE_BLOCKS == -1)
    {
        log_error(MODULE_LOGGER, "Error al abrir el archivo bloques.dat");
        exit(EXIT_FAILURE);
    }
}
*/
void create_file(char* file_name, uint32_t initial_block){
    //size_t path_len = strlen(PATH_BASE_DIALFS) + 1 +strlen(file_name); //1 por la '/'
	char* path_file = string_new();
	strcpy (path_file, PATH_BASE_DIALFS);
	string_append(&path_file, "/");
	string_append(&path_file, file_name);

	//Checkeo si el file ya esta creado, sino lo elimino
	if (access(path_file, F_OK) == 0)remove(path_file);
 
	FILE_METADATA = fopen(path_file, "wb");
    if (FILE_METADATA == NULL) {
        log_error(MODULE_LOGGER, "Error al abrir el archivo %s", file_name);
        exit(EXIT_FAILURE);
    }

	t_config* config_temp = config_create(path_file);
    config_set_value(config_temp, "BLOQUE_INICIAL", "0");
    config_set_value(config_temp, "TAMAÑO_ARCHIVO", string_itoa(initial_block));
	config_save_in_file(config_temp,path_file);
	config_destroy(config_temp);
		
	free(path_file);
	fclose(FILE_METADATA);
}

void update_file(char* file_name, uint32_t size, uint32_t location){
	char* path_file = string_new();
	strcpy (path_file, PATH_BASE_DIALFS);
	string_append(&path_file, "/");
	string_append(&path_file, file_name);

	t_config* config_temp = config_create(path_file);
    config_set_value(config_temp, "BLOQUE_INICIAL", string_itoa(location));
    config_set_value(config_temp, "TAMAÑO_ARCHIVO", string_itoa(size));
	config_save_in_file(config_temp,path_file);
	config_destroy(config_temp);

	free(path_file);
}


t_FS_File* seek_file_by_header_index(uint32_t position){

	t_FS_File* magic = NULL;

	for (size_t i = 0; i < list_size(LIST_FILES); i++)
	{
		magic = list_get(LIST_FILES,i);
		if (magic->initial_bloq == position) return magic;
	}

	return magic;
}

void compact_blocks(){
	usleep(COMPRESSION_DELAY * 1000);
	int total_free_spaces = 0;
	int len = 0;

			for (uint32_t i = 0; i < BLOCK_COUNT; i++)
			{
				if (!(bitarray_test_bit(BITMAP,i)))//Cuento los espacios vacios
				{
					total_free_spaces++;
				}
				else{//Busco el header
					t_FS_File* temp_entry = seek_file_by_header_index(i);
					len = temp_entry->len;
					if (total_free_spaces != 0){//Mueve el bloque y actualiza el bitmap
						moveBlock(temp_entry->len, temp_entry->size, total_free_spaces, i);
						temp_entry->initial_bloq -= total_free_spaces;
						update_file(temp_entry->name, temp_entry->size, i);
					}
					i+=len; //Salteo los casos ya contemplados en moveBlock

				}
				
			}
			
    if (msync(PTRO_BITMAP, BITMAP_SIZE, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bitmap.dat con el archivo: %s", strerror(errno));
    }
	
    if (msync(PTRO_BLOCKS, BLOCKS_TOTAL_SIZE, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bloques.dat con el archivo: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

}

void moveBlock(uint32_t blocks_to_move, uint32_t size, uint32_t free_spaces, uint32_t location){
	//Mueve el bloque y actualiza el bitmap

    void* context = malloc(size);
	void *posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + (location * BLOCK_SIZE));
    memcpy(context, posicion, size); 
	posicion = (void *)(((uint8_t *) PTRO_BLOCKS) + ((location - free_spaces) * BLOCK_SIZE));
    memcpy(posicion, context, size); 

	for (size_t i = 0; i < blocks_to_move; i++)
	{
		bitarray_clean_bit(BITMAP,(location + i));
		bitarray_set_bit(BITMAP,(location + i - free_spaces));
	}

	free(context);
	
}