/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

char *INTERFACE_NAME;

t_PID PID;
char *MODULE_NAME = "entradasalida";
char *MODULE_LOG_PATHNAME = "entradasalida.log";

t_log *MODULE_LOGGER;
t_config *MODULE_CONFIG;

int WORK_UNIT_TIME;

t_Connection CONNECTION_KERNEL;
t_Connection CONNECTION_MEMORY;

char *PATH_BASE_DIALFS;
size_t BLOCK_SIZE;
size_t BLOCK_COUNT;
int COMPRESSION_DELAY;

t_list *LIST_FILES;
t_bitarray *BITMAP;

t_IO_Type IO_TYPES[] = {
    [GENERIC_IO_TYPE] = {.name = "GENERIC", .function = generic_interface_function },
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

int module(int argc, char *argv[]) {

	if(argc != 3) {
		fprintf(stderr, "Uso: %s <NOMBRE_INTERFAZ> <ARCHIVO_DE_CONFIGURACION>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	INTERFACE_NAME = argv[1];

	initialize_loggers();
	initialize_configs(argv[2]);

	LIST_FILES = list_create();

	initialize_sockets();

	log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

	t_Return_Value return_value;
	receive_expected_header(INTERFACE_DATA_REQUEST_HEADER, CONNECTION_KERNEL.fd_connection);	
	send_interface_data(INTERFACE_NAME, IO_TYPE, CONNECTION_KERNEL.fd_connection);
	receive_return_value_with_expected_header(INTERFACE_DATA_REQUEST_HEADER, &return_value, CONNECTION_KERNEL.fd_connection);
	if(return_value) {
		log_error(MODULE_LOGGER, "No se pudo registrar la interfaz %s en el Kernel", INTERFACE_NAME);
		exit(EXIT_FAILURE);
	}

	switch(IO_TYPE){
		case DIALFS_IO_TYPE:
		initialize_blocks();
		initialize_bitmap(BLOCK_COUNT);
	}

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
	pthread_t thread_io_connect_to_kernel;
	pthread_t thread_io_connect_to_memory;

	// [Client] Entrada/Salida -> [Server] Kernel
	pthread_create(&thread_io_connect_to_kernel, NULL, client_thread_connect_to_server, (void *) &CONNECTION_KERNEL);
	// [Client] Entrada/Salida -> [Server] Memoria
	pthread_create(&thread_io_connect_to_memory, NULL, client_thread_connect_to_server, (void *) &CONNECTION_MEMORY);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_io_connect_to_kernel, NULL);
	pthread_join(thread_io_connect_to_memory, NULL);
}

void finish_sockets(void) {
	close(CONNECTION_KERNEL.fd_connection);
	close(CONNECTION_MEMORY.fd_connection);
}

void generic_interface_function(void) {

	t_Payload *io_operation = NULL;
	//Espero peticiones
	while(1) {
		
		//Recibo peticion
		receive_io_operation_dispatch(&PID, io_operation, CONNECTION_KERNEL.fd_connection);

		//Ejecuto operacion y retorno valor
		t_Return_Value return_value = io_operation_execute(io_operation);
		
		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
		send_io_operation_finished(PID, return_value, CONNECTION_KERNEL.fd_connection);
	}
}

void stdin_interface_function(void) {

	t_Payload *io_operation = NULL;
	//escuchar peticion siempre
	while(1){
		
		//Recibo peticion
		receive_io_operation_dispatch(&PID, io_operation, CONNECTION_KERNEL.fd_connection);

		//Ejecuto operacion y retorno valor
		t_Return_Value return_value = io_operation_execute(io_operation);
		
		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
		send_io_operation_finished(PID, return_value, CONNECTION_KERNEL.fd_connection);
	}
}

void stdout_interface_function(void) {

	t_Payload *io_operation = NULL;
	//escuchar peticion siempre
	while(1) {
		
		//Recibo peticion
		receive_io_operation_dispatch(&PID, io_operation, CONNECTION_KERNEL.fd_connection);

		//Ejecuto operacion y retorno valor
		t_Return_Value return_value = io_operation_execute(io_operation);
		
		// LE AVISO A KERNEL CÓMO SALIÓ LA OPERACIÓN
		send_io_operation_finished(PID, return_value, CONNECTION_KERNEL.fd_connection);
	}
}

void dialfs_interface_function(void) {

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

void initialize_blocks() {
    size_t blocks_size = BLOCK_SIZE * BLOCK_COUNT;

    int fd = open("bloques.dat", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        log_error(MODULE_LOGGER, "Error al abrir el archivo bloques.dat: %s", strerror(errno));
        return;
    }

    if (ftruncate(fd, blocks_size) == -1) {
        log_error(MODULE_LOGGER, "Error al ajustar el tamaño del archivo bloques.dat: %s", strerror(errno));
        close(fd);
        return;
    }

    char *blocks_data = mmap(NULL, blocks_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (blocks_data == MAP_FAILED) {
        log_error(MODULE_LOGGER, "Error al mapear el archivo bloques.dat a memoria: %s", strerror(errno));
        close(fd);
        return;
    }

    for (int i = 0; i < BLOCK_COUNT; ++i) {
        int *block_data = (int *)(blocks_data + i * BLOCK_SIZE);
        *block_data = i;
    }

    if (msync(blocks_data, blocks_size, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bloques.dat con el archivo: %s", strerror(errno));
    }

    if (munmap(blocks_data, blocks_size) == -1) {
        log_error(MODULE_LOGGER, "Error al desmapear el archivo bloques.dat de la memoria: %s", strerror(errno));
    }

    close(fd);
    log_info(MODULE_LOGGER, "Bloques creados y mapeados correctamente.");
}


void initialize_bitmap(size_t block_count) {
	size_t BITMAP_SIZE = ceil(block_count / 8);
    int fd = open("bitmap.dat", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        log_error(MODULE_LOGGER, "Error al abrir el archivo bitmap.dat: %s", strerror(errno));
        return;
    }

    if (ftruncate(fd, BITMAP_SIZE) == -1) {
        log_error(MODULE_LOGGER, "Error al ajustar el tamaño del archivo bitmap.dat: %s", strerror(errno));
        close(fd);
        return;
    }

    unsigned char *bitmap_data = mmap(NULL, BITMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bitmap_data == MAP_FAILED) {
        log_error(MODULE_LOGGER, "Error al mapear el archivo bitmap.dat a memoria: %s", strerror(errno));
        close(fd);
        return;
    }

    BITMAP = bitarray_create_with_mode((char *)bitmap_data, BITMAP_SIZE,LSB_FIRST);
    if (BITMAP == NULL) {
        log_error(MODULE_LOGGER, "Error al crear la estructura del bitmap");
        munmap(bitmap_data, BITMAP_SIZE);
        close(fd);

        return;
    }

    if (msync(bitmap_data, BITMAP_SIZE, MS_SYNC) == -1) {
        log_error(MODULE_LOGGER, "Error al sincronizar los cambios en bitmap.dat con el archivo: %s", strerror(errno));
    }

    if (munmap(bitmap_data, BITMAP_SIZE) == -1) {
        log_error(MODULE_LOGGER, "Error al desmapear el archivo bitmap.dat de la memoria: %s", strerror(errno));
    }

    //free(bitmap);
    close(fd);
    log_info(MODULE_LOGGER, "Bitmap creado y mapeado correctamente.");
}

int io_gen_sleep_io_operation(t_Payload *operation) {

	switch(IO_TYPE) {
		case GENERIC_IO_TYPE:
		{
			uint32_t work_units;
			payload_dequeue(operation, &PID, sizeof(t_PID));
			payload_dequeue(operation, &work_units, sizeof(uint32_t));

			log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %" PRIu32, INTERFACE_NAME, work_units);

			log_info(MODULE_LOGGER, "PID: <%d> - OPERACION <IO_GEN_SLEEP>", PID);
			sleep(WORK_UNIT_TIME * work_units);


			break;
		}
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return 1;
	}

    return 0;
}

int io_stdin_read_io_operation(t_Payload *operation) {

    // log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

	switch(IO_TYPE){
		case STDIN_IO_TYPE:
		
			//Inicializo Headers para memoria
			//e_Header IO_STDIN_WRITE_MEMORY;	
			//e_Header WRITE_REQUEST;
			//Creo paquete y argumentos necesarios para enviarle a memoria
			t_Package *package = package_create_with_header(IO_STDIN_WRITE_MEMORY);
			
			t_list *physical_addresses = list_create();
			t_MemorySize bytes;

			//Empiezo a "desencolar" el payload recibido
			payload_dequeue(operation, &PID, sizeof(t_PID));
			list_deserialize(operation, physical_addresses, physical_address_deserialize_element);
			payload_dequeue(operation, &bytes, sizeof(t_MemorySize));

			//Aviso que operacion voy a hacer
			log_info(MODULE_LOGGER, "PID: <%d> - OPERACION <IO_STDIN_READ>", PID);

			//Encolo el payload en el paquete creado antes
			payload_enqueue(package->payload, &(PID), sizeof(t_PID));
			list_serialize(package->payload, *physical_addresses, physical_address_deserialize_element);
			payload_enqueue(package->payload, &bytes, sizeof(t_MemorySize));

			//Envio el paquete y lo destruyo
			package_send(package, CONNECTION_MEMORY.fd_connection);
			package_destroy(package);

			//Recibo si salio bien la operacion
			receive_return_value_with_expected_header(WRITE_REQUEST, 0, CONNECTION_MEMORY.fd_connection);
	
			break;
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return 1;
	}

	return 0;	
}

int io_stdout_write_io_operation(t_Payload *operation) {
    
	switch(IO_TYPE){
		case STDOUT_IO_TYPE:


			//Creo header para memoria y el paquete con los argumentos
			//e_Header IO_STDOUT_READ_MEMORY;
			t_Package* package = package_create_with_header(IO_STDOUT_READ_MEMORY);
			
			t_list *physical_addresses = list_create();
			t_MemorySize bytes;
			char* text_receive = NULL;

			//empiezo a "desencolar" el payload recibido
			payload_dequeue(operation, &PID, sizeof(t_PID));
			text_deserialize(operation, &text_receive);
			list_deserialize(operation, physical_addresses, physical_address_deserialize_element);
			payload_dequeue(operation, &bytes, sizeof(t_MemorySize));

			//Aviso la operacion que voy a hacer
			log_info(MODULE_LOGGER, "PID: <%d> - OPERACION <IO_STDOUT_WRITE>", PID);

			//Encolo devuelta el payload dentro del paquete creado
			payload_enqueue(package->payload, &(PID), sizeof(t_PID));
			list_serialize(package->payload, *physical_addresses, physical_address_deserialize_element);
			payload_enqueue(package->payload, &bytes, sizeof(t_MemorySize));

			//Envio el paquete y lo destruyo
			package_send(package, CONNECTION_MEMORY.fd_connection);
			package_destroy(package);
			
			//Recibo nuevo paquete para imprimir por pantalla
			t_Package *memory_package = NULL;
			package_receive(memory_package,CONNECTION_MEMORY.fd_connection);
			//Desencolar e imprimir por pantalla

			break;
		default:
			log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
			return 1;
	}
    
    return 0;
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

int io_fs_create_io_operation(t_Payload *operation) {


    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_CREATE recibido.");
	char* file_name;
	t_PID op_pid;

    payload_dequeue(operation, &op_pid, sizeof(t_PID));
    text_deserialize(operation, &(file_name));
	uint32_t location = seek_first_free_block();

	t_FS_File* new_entry = NULL;
	strcpy(new_entry->name , file_name);
	new_entry->process_pid = op_pid;
	new_entry->initial_bloq = location;
	new_entry->len = 0;

	//Checkiar si el FS es solo para este hilo o para todo el modulo
	//Agregar un mutex

	bitarray_set_bit(BITMAP, location);

	list_add(LIST_FILES, new_entry);

    log_debug(MINIMAL_LOGGER, "PID: <%d> - Crear archivo: <%s>", (int) op_pid, file_name);

	t_Package* respond = package_create_with_header(IO_FS_CREATE_CPU_OPCODE);
	payload_enqueue(respond->payload, &op_pid, sizeof(t_PID));
	package_send(respond, CONNECTION_KERNEL.fd_connection);

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