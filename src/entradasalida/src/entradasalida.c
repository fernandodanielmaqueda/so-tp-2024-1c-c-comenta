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

t_list *LIST_FILES;
t_Bitmap *BITMAP;
char* BLOCKS_DATA;

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
	
	//free_bitmap_blocks();
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
	initialize_bitmap(BLOCK_COUNT);
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

	usleep(WORK_UNIT_TIME * work_units);

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
	payload_append(&(package->payload), text_to_send, sizeof(text_to_send));
	list_serialize(&(package->payload), *physical_addresses, physical_address_deserialize_element);
	payload_append(&(package->payload), &bytes, sizeof(bytes));
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
	char* new_file_path;


    payload_shift(operation_arguments, &PID, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));

   	log_debug(MINIMAL_LOGGER, "PID: <%d> - Crear archivo: <%s>", (int) PID, file_name);
	usleep(WORK_UNIT_TIME);

	new_file_path = malloc(strlen(file_name) + strlen(PATH_BASE_DIALFS) + 1);

	strcpy(new_file_path, PATH_BASE_DIALFS);
	strcat(new_file_path, file_name);

	//busco bloque libre en el bitarray
	t_list* list_of_free_blocks = free_blocks_list(1, BITMAP->bitarray);

	//obtengo el primer bloque libre
	int* first_block = list_get(list_of_free_blocks, 0);

	//creo el archivo
	FILE* file = fopen(new_file_path, "w");

	//Escribo sobre el archivo
	fprintf(file, "BLOQUE INICIAL = %d\n", first_block);
	fprintf(file, "TAMAÑO ARCHIVO = %d", 0);

	//ocupo los bloques
	fill_blocks(list_of_free_blocks, BITMAP->bitarray);

	log_info(MODULE_LOGGER, "Archivo %s creado\n", file_name);

	fclose(file);
	free(new_file_path);
	free(file_name);

    return 0;
}

int io_fs_delete_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

	//t_PID op_pid = 0;
	char* file_name = NULL;
	char* file_to_delete_path;

	payload_shift(operation_arguments, &PID, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));

	log_debug(MINIMAL_LOGGER, "PID: <%d> - Eliminar archivo: <%s>",(int) PID, file_name);
	usleep(WORK_UNIT_TIME);

	//creo un path para el archivo a borrar
	file_to_delete_path = malloc(strlen(file_name) + strlen(PATH_BASE_DIALFS) + 1);

	//le copio el path del base dial fs
	//le agrego al path el nombre del archivo
	strcpy(file_to_delete_path, PATH_BASE_DIALFS);
	strcat(file_to_delete_path, file_name);

	//obtengo los valores del bloque inicial y del tamaño del archivo
	t_Metadata metadata = obtain_data(file_to_delete_path);

	if(remove(file_name) == 0){
		log_info(MODULE_LOGGER, "Archivo %s borrado", file_name);

		int blocks = ceil((double)metadata.file_size / (double)BLOCK_SIZE);

		free_blocks(metadata.initial_bloq, blocks);
	}else{
		log_info(MODULE_LOGGER, "El archivo a borrar no existe");
	}

	free(file_name);

    return 0;
}

int io_fs_truncate_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_TRUNCATE recibido.");

	char* file_name = NULL;
	int new_value;
	char* file_to_truncate_path;
	t_Metadata* metadata_values = malloc(sizeof(t_Metadata));

    payload_shift(operation_arguments, &PID, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
    payload_shift(operation_arguments, &(new_value),sizeof(new_value));

	log_info(MINIMAL_LOGGER, "PID: <%d> - Inicio Truncate", (int)PID);
	usleep(WORK_UNIT_TIME);

	//le creo un path al archivo a truncar
	file_to_truncate_path = malloc(strlen(file_name) + strlen(PATH_BASE_DIALFS) + 1);

	//le copio el path del base dial fs
	//le agrego al path el nombre del archivo
	strcpy(file_to_truncate_path, PATH_BASE_DIALFS);
	strcat(file_to_truncate_path, file_name);

	//creo un config con los valores para obtener lo que ya tengo de metadata
	t_config* config_metadata = config_create(file_to_truncate_path);

	int first_block = config_get_int_value(config_metadata, "BLOQUE_INICIAL");
	int actual_size = config_get_int_value(config_metadata, "TAMANIO_ARCHIVO");
	
	int new_blocks = ceil((double)new_value / (double)BLOCK_SIZE);
	int actual_blocks = ceil((double)actual_size / (double) BLOCK_SIZE);
	int blocks_to_modify;

	//chequeo que los bloques actuales sean 0 (no puede pasar eso)
	if(actual_blocks == 0){
		actual_blocks = 1;
	}

	metadata_values->initial_bloq = first_block;
	metadata_values->file_size = new_value;

	update_metadata(file_to_truncate_path, metadata_values);

	if(new_blocks > actual_blocks){
		blocks_to_modify = new_blocks - actual_blocks;

		if(verify_availability_of_blocks(first_block + actual_blocks,blocks_to_modify)){

			setearNBits(first_block, new_blocks);
		}else{
			//compacto el bitmap
			compact_bitmap(file_to_truncate_path, file_name);
		}

	}
	else if(actual_blocks > new_blocks){
		blocks_to_modify = actual_blocks - new_blocks;
		free_blocks(first_block + new_blocks, blocks_to_modify);
	}

	log_info(MODULE_LOGGER, "Archivo %s truncado", file_name);

	free(metadata_values);
	free(file_to_truncate_path);
	config_destroy(config_metadata);
    
    return 0;
}

int io_fs_write_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_READ recibido.");

	//defino las variables que voy a usar
	char* path_file_to_write;
	char* file_name;
	t_list* physical_addresses = list_create();
	t_MemorySize bytes;
	char* pointer_for_writing;

	//desencolo lo que me llega de kernel
    payload_shift(operation_arguments, &PID, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
	list_deserialize(operation_arguments, physical_addresses, physical_address_deserialize_element);
	payload_shift(operation_arguments, &bytes, sizeof(bytes));
	list_deserialize(operation_arguments, pointer_for_writing, sizeof(pointer_for_writing));	

	//le mando a memoria lo que necesita y creo el paquete
	t_Package *package = package_create_with_header(IO_FS_WRITE_MEMORY);
	payload_append(&(package->payload), &PID, sizeof(PID));
	list_serialize(&(package->payload), *physical_addresses, physical_address_deserialize_element);
	payload_append(&(package->payload), &bytes, sizeof(bytes));
	package_send(package, CONNECTION_MEMORY.fd_connection);
	package_destroy(package);

	//Recibo nuevo paquete con la info para escribir
	usleep(WORK_UNIT_TIME);	
	package_receive(&package, CONNECTION_MEMORY.fd_connection);

	//Inicio la operacion y guardo en el puntero lo que hay que escribir en el archivo
	log_info(MINIMAL_LOGGER, "PID: <%d> - Escribir archivo: <%s> - Tamaño a escribir <%d> - Puntero Archivo <%s>", (int)PID, file_name, (int) bytes, pointer_for_writing);
	payload_shift(&(package->payload), pointer_for_writing, bytes);

	//Genero el path del archivo
	path_file_to_write = malloc(strlen(file_name) + strlen(PATH_BASE_DIALFS) +1);
	strcpy(path_file_to_write, PATH_BASE_DIALFS);
	strcat(file_name, PATH_BASE_DIALFS);

	//Creo el config del path del archivo
	t_config* config_file = config_create(path_file_to_write);

	//Veo los valores del archivo (bloque inicial y longitud)
	int first_block = config_get_int_value(config_file, "BLOQUE_INICIAL");
	int size = config_get_int_value(config_file, "TAMANIO_ARCHIVO");

	//Agarro el archivo de bloques de datos
	char* blocks_path = malloc(strlen("bloques.dat") + strlen(PATH_BASE_DIALFS) + 1);

	strcpy(blocks_path, PATH_BASE_DIALFS);
	strcat(blocks_path, "bloques.dat");

	//Chequeo que el tamaño del archivo sea mayor a lo que hay que escribir
	if(size > bytes){
		//se abre el archivo de bloques en modo lectura y escritua
		FILE* blocks_file = fopen(blocks_path, "wb+");
		//si existe el archivo de bloques
		if(blocks_file != NULL){
			//vamos al desplazamiento del primer bloque
			fseek(blocks_file, (first_block * BLOCK_SIZE) + pointer_for_writing, SEEK_SET);
			//Escribo en el archivo lo que corresponde
			fwrite(pointer_for_writing, 1, bytes, blocks_file);

			log_info(MODULE_LOGGER, "Archivo escrito correctamente");
			fclose(blocks_file);
		}else{
			log_info(MODULE_LOGGER, "No se pudo abrir el archivo");
		}
	}else{
		log_error(MODULE_LOGGER, "No entra el contenido en el archivo");
	}

	free(blocks_path);
	free(path_file_to_write);

    return 0;
}

int io_fs_read_io_operation(t_Payload *operation_arguments) {

	if(IO_TYPE != DIALFS_IO_TYPE) {
		log_info(MODULE_LOGGER, "No puedo realizar esta instruccion");
		return 1;
	}

    log_trace(MODULE_LOGGER, "[FS] Pedido del tipo IO_FS_READ recibido.");

	char* file_name = NULL;
	uint32_t ptro = 0;
	uint32_t bytes = 0;
	t_PID op_pid = 0;
	t_list* list_dfs = list_create();

	usleep(WORK_UNIT_TIME);
    payload_shift(operation_arguments, &op_pid, sizeof(t_PID));
    text_deserialize(operation_arguments, &(file_name));
    payload_shift(operation_arguments, &ptro, sizeof(uint32_t));
    payload_shift(operation_arguments, &bytes, sizeof(uint32_t));
	list_deserialize(operation_arguments, list_dfs, physical_address_deserialize_element);


//	t_Package* pack_respond = package_create_with_header(IO_FS_READ_MEMORY);

	//FALTA TERMINAR: LEER FS --> REQUEST de WRITE MEMORIA


    return 0;
}

void initialize_blocks() {

    char* path_blocks = malloc(strlen("bloques.dat") + strlen(PATH_BASE_DIALFS) + 1);
	uint32_t size_blocks = BLOCK_COUNT * BLOCK_SIZE;

	//copia lo que hay en pathdialfs a path_blocks
	strcpy(path_blocks, PATH_BASE_DIALFS);
	//pongo .dat al final
	strcat(path_blocks, "bloques.dat");

	FILE* nFile = fopen(path_blocks, "ab+");

	truncate(path_blocks, size_blocks);

	fclose(nFile);

	free(path_blocks);

}


void initialize_bitmap() {
	char* path_bitmap = malloc(strlen("bitmap.dat") + strlen(PATH_BASE_DIALFS) + 1);

	strcpy(path_bitmap, PATH_BASE_DIALFS);
	strcat(path_bitmap, "bitmap.dat");

	BITMAP = malloc(sizeof(t_Bitmap));

	int fdBitmapFile = open(path_bitmap, O_RDWR | O_CREAT, 0644);

	BITMAP->tamanio = (uint32_t)ceil(BLOCK_COUNT / 8);
	if(ftruncate(fdBitmapFile, BITMAP->tamanio) == -1){
        log_info(MODULE_LOGGER,"Error truncando el archivo bitmap");
    }

	BITMAP->posicion = mmap(NULL, BITMAP->tamanio, PROT_READ | PROT_WRITE , MAP_SHARED, fdBitmapFile, 0);
    BITMAP->bitarray = bitarray_create_with_mode(BITMAP->posicion, BITMAP->tamanio, LSB_FIRST);

	if (!BITMAP->bitarray){
        log_error(MODULE_LOGGER, "Error creando el bitarray");
        munmap(BITMAP->posicion, BITMAP->tamanio);
        close(fdBitmapFile);
        free(BITMAP);
    }

    if(msync(BITMAP->posicion, BITMAP->tamanio, MS_SYNC) == -1)
    {
        log_error(MODULE_LOGGER, "Error sincronizando el bitmap");
    }

    close(fdBitmapFile);

    free(path_bitmap);	
}


//Funcion para ver el primer bloque libre
uint32_t seek_first_free_block(){
	int magic = 0;
	bool used_block;

	for (magic = 0; magic < bitarray_get_max_bit(BITMAP->bitarray); magic)
	{
		used_block = bitarray_test_bit(BITMAP->bitarray, magic);
		if(!used_block){
			return -1;
			break;
		}
	}

	return -1;
}

//Funcion para devolver lista de los bloques libres
t_list* free_blocks_list(int quantity_blocks, t_bitarray* block){
    t_list* free_blocks = list_create();
    int i = 0;
    int j = 0;

    //Mientras j sea menor a la cantidad de bloques pedida e i sea menor a la cantidad maxima de bloques
    while((j < quantity_blocks) && (i < bitarray_get_max_bit(block)))
    {
        //Me fijo si el bloque esta ocupado
        if(!bitarray_test_bit(block, i))
        {	
            //Si no lo esta entonces almaceno su index en la lista de bloquesLibres
			int* aux = malloc(sizeof(int));
            *aux = i;

            list_add(free_blocks, aux);

            j++;
        }
        i++;
    }

    if (j < quantity_blocks) {
        list_destroy_and_destroy_elements(free_blocks,free);
        return NULL;
    }

    return free_blocks;
}

//Funcion para ocupar bloques del bitmap
void fill_blocks(t_list* fill_blocks , t_bitarray* block)
{
    int i = 0;
	int* n;

    //Pongo en 1 todos los bits de los bloques que ocupe
    while(i < (list_size(fill_blocks)))
    {
    	n = list_get(fill_blocks, i);
        bitarray_set_bit(block, *n);
        i++;
    }
}

//funcion para obtener los datos de la metadata
t_Metadata obtain_data(char* path){
	t_config* data = config_create(path);
	t_Metadata metadata;

	metadata.initial_bloq = config_get_int_value(data, "BLOQUE_INICIAL");
	metadata.file_size = config_get_int_value(data, "TAMANIO_ARCHIVO");

	config_destroy(data);

	return metadata;
}

//funcion para liberar bloques
void free_blocks(int initial_bloq, int quantity_blocks)
{
    if(quantity_blocks == 0)
    {
        bitarray_clean_bit(BITMAP->bitarray, initial_bloq);
    }

    else
    {
        int final_bloq = initial_bloq + quantity_blocks;

        //Limpio el bitarray
        for(int i = initial_bloq; i < final_bloq; i++)
        {
            bitarray_clean_bit(BITMAP->bitarray, i);
        }
    }
}

//funcion para actualizar los valores de un archivo de metadata
void update_metadata(char* path, t_Metadata* metadata)
{
    t_config* metadata_to_modify = config_create(path);

    char initial_bloq[12];
    sprintf(initial_bloq, "%d", metadata->initial_bloq);
    char size[12];
    sprintf(size, "%d", metadata->file_size);

    config_set_value(metadata_to_modify,"BLOQUE_INICIAL",  initial_bloq);
    config_set_value(metadata_to_modify,"TAMANIO_ARCHIVO", size);

    config_save(metadata_to_modify);
    config_destroy(metadata_to_modify);
}

//funcion para verificar la disponibilidad de los bloques
bool verify_availability_of_blocks(int initial_bloq, int blocks_to_modify)
{
    for(int i = 0; i < blocks_to_modify; i++)
    {
        if(bitarray_test_bit(BITMAP->bitarray, i + initial_bloq))
        {
            return false;
        }
    }
    return true;
}

//Funcion para setear la cantidad de bits especificada
void setearNBits(int first, int bits_quantity)
{
    for(int i = first; i < bits_quantity; i++)
    {
        bitarray_set_bit(BITMAP->bitarray, i);
    }
}

void compact_bitmap(char* file_to_truncate_path, char* file_name){
	log_info(MODULE_LOGGER, "Iniciar compactacion...");

	//obtengo los datos del archivo que causó la compactacion
	t_Metadata metadata = obtain_data(file_to_truncate_path);
	free_blocks(metadata.initial_bloq, metadata.file_size);

	int contador_bloques = compact_first_files(file_name);

	metadata.initial_bloq = contador_bloques;

	if(metadata.file_size == 0){
		contador_bloques += 1;
	}else{
		contador_bloques += ceil((double)metadata.file_size / (double)BLOCK_SIZE);
	}

	//Actualizo la metadata con los nuevos valores
	update_metadata(file_to_truncate_path, &metadata);

	//seteo los bits necesarios
	setearNBits(0, contador_bloques);

	//se hace el sleep de la compactacion
	usleep(COMPRESSION_DELAY * 1000);

	log_info(MODULE_LOGGER, "Fin compatación");

	free(file_to_truncate_path);
}

int compact_first_files(char* file_name)
{

    //Creo las estructuras del directorio de dialfs
    DIR* directorioFS = opendir(PATH_BASE_DIALFS);
    struct dirent* archivoDirectorio;

    //Declaro las funciones que necesito para modificar
    int contadorBloques = 0;
    t_Metadata aux;

    //Leo hasta que no hay mas archivos en el directorio
    while((archivoDirectorio = readdir(directorioFS))!=NULL)
    {
        //Si el directorio es .txt y no es el culpable, lo voy compactando
        if(strstr(archivoDirectorio->d_name, ".txt") != NULL && strcmp(archivoDirectorio->d_name, file_name) != 0) 
        {
            //Reservo espacio para el path de este archivo .txt
            char* pathArchivo = malloc(strlen(archivoDirectorio->d_name) + strlen(PATH_BASE_DIALFS) + 1);
            strcpy(pathArchivo, PATH_BASE_DIALFS);
            strcat(pathArchivo, archivoDirectorio->d_name);

            //Extraigo los datos y libero sus bloques
            aux = obtain_data(pathArchivo);
            free_blocks(aux.initial_bloq, aux.file_size);

            //Actualizo su metadata e incremento el contador
            aux.initial_bloq = contadorBloques;
            if(aux.file_size == 0)
            {
                contadorBloques += 1;
            }
            else
            {
                contadorBloques += ceil((double)aux.file_size / BLOCK_SIZE);
            }
            update_metadata(pathArchivo, &aux);

            free(pathArchivo);
        }
    }

    //Cierro el directorio y retorno el contador
    closedir(directorioFS);

    return contadorBloques;
}
