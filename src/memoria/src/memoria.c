
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "memoria.h"

char *MODULE_NAME = "memoria";
char *MODULE_LOG_PATHNAME = "memoria.log";
char *MODULE_CONFIG_PATHNAME = "memoria.config";

t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
t_config *MODULE_CONFIG;

void *memoria_principal;
pthread_t hilo_kernel;
pthread_t hilo_cpu;
pthread_t hilo_io;
t_list* lista_procesos;
t_list* lista_marcos;
t_list* lista_marcos_libres;

t_Server COORDINATOR_MEMORY;
int FD_CLIENT_KERNEL;
int FD_CLIENT_CPU;

sem_t sem_coordinator_kernel_client_connected;
sem_t sem_coordinator_cpu_client_connected;

int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs();

    memoria_principal = (void*) malloc(TAM_MEMORIA);
    memset(memoria_principal, (u_int32_t) '0', TAM_MEMORIA); //Llena de 0's el espacio de memoria
    lista_procesos = list_create();
    create_marcos();

    initialize_sockets();

    log_info(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);
 
 /*   
    pthread_create(&hilo_cpu, NULL, (void *)listen_cpu, (void *)FD_CLIENT_CPU);
    pthread_create(&hilo_kernel, NULL, (void *)listen_kernel, (void *)FD_CLIENT_KERNEL);
    pthread_create(&hilo_io, NULL, (void *)listen_io, (void *)fd_io);

    pthread_join(hilo_cpu, NULL);
    pthread_join(hilo_kernel, NULL);
    pthread_join(hilo_io, NULL);

    */

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();

   return EXIT_SUCCESS;
}

void read_module_config(t_config* MODULE_CONFIG) {
    COORDINATOR_MEMORY = (t_Server) {.server_type = MEMORY_TYPE, .clients_type = TO_BE_DEFINED_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA")};
    TAM_MEMORIA = config_get_int_value(MODULE_CONFIG, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(MODULE_CONFIG, "TAM_PAGINA");
    PATH_INSTRUCCIONES = config_get_string_value(MODULE_CONFIG, "PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(MODULE_CONFIG, "RETARDO_RESPUESTA");
}

void initialize_sockets(void) {

	pthread_t thread_memory_start_server;

    sem_init(&sem_coordinator_kernel_client_connected, 0, 0);
    sem_init(&sem_coordinator_cpu_client_connected, 0, 0);

	// [Server] Memory <- [Cliente(s)] Entrada/Salida + Kernel + CPU
	pthread_create(&thread_memory_start_server, NULL, memory_start_server, (void*) &COORDINATOR_MEMORY);

	// Se bloquea hasta que se realicen todas las conexiones
    sem_wait(&sem_coordinator_kernel_client_connected);
    sem_wait(&sem_coordinator_cpu_client_connected);
	
}

void finish_sockets(void) {
	close(COORDINATOR_MEMORY.fd_listen);
	close(FD_CLIENT_KERNEL);
	close(FD_CLIENT_CPU);
}

void *memory_start_server(void *server_parameter) {
	t_Server *server = (t_Server *) server_parameter;

	int *fd_new_client;
	pthread_t thread_new_client;

	server_start(server);

	while(1) {
		fd_new_client = malloc(sizeof(int));
		log_info(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		*fd_new_client = server_accept(server->fd_listen);

		if(*fd_new_client == -1) {
			log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			free(fd_new_client);
			continue;
		}

		log_info(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		pthread_create(&thread_new_client, NULL, memory_client_handler, (void*) fd_new_client);
		pthread_detach(thread_new_client);
	}

	return NULL;
}

void *memory_client_handler(void *fd_new_client_parameter) {
	int* fd_new_client = (int *) fd_new_client_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    bytes = recv(*fd_new_client, &handshake, sizeof(int32_t), MSG_WAITALL);

    switch((e_PortType) handshake) {
        case KERNEL_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UN KERNEL MAS DE UNA VEZ
            log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] %s", "Kernel");
            FD_CLIENT_KERNEL = *fd_new_client;
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            sem_post(&sem_coordinator_kernel_client_connected);
            // Lógica de manejo de cliente Kernel (crear un hilo para menejo de cliente Kernel)
            break;
        case CPU_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UNA CPU MAS DE UNA VEZ
            log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] %s", "CPU");
            FD_CLIENT_CPU = *fd_new_client;
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            sem_post(&sem_coordinator_cpu_client_connected);
            // Lógica de manejo de cliente CPU (crear un hilo para menejo de cliente CPU)
            break;
        case IO_TYPE:
            log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] %s", "Entrada/Salida");
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            // Lógica de manejo de cliente Entrada/Salida (crear un hilo para menejo de cliente Entrada/Salida)
            free(fd_new_client);
            break;
        default:
            log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] %s", "No reconocido");
            bytes = send(*fd_new_client, &resultError, sizeof(int32_t), 0);
            free(fd_new_client);
            break;
    }

    return NULL;
}

void listen_kernel(int fd_kernel) {
    while(1) {
        e_Header header = 0; //enum HeaderCode headerCode = package_receive_header(fd_kernel);
        switch(header) {
            case PROCESS_NEW:
                log_info(MODULE_LOGGER, "KERNEL: Proceso nuevo recibido.");
                create_process(fd_kernel);
                break;
                
            case PROCESS_FINALIZED:
                log_info(MODULE_LOGGER, "KERNEL: Proceso finalizado recibido.");
                kill_process(fd_kernel);
                break;

            case DISCONNECTION_HEADER:
                log_warning(MODULE_LOGGER, "Se desconecto kernel.");
                log_destroy(MODULE_LOGGER);
                return;
            
            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
        }
    }
}

void create_process(int socketRecibido) {

    t_Process *new_process; // = malloc(sizeof(t_Process));
    t_list* instructions_list = list_create();
    t_list* pages_table = list_create();

    //Leo los valores recibidos por parametro
    t_list *lista_elememtos = NULL; //t_list *lista_elememtos = get_package_like_list(socketRecibido);
    int cursor = 0;
    new_process->name = string_duplicate(list_get(lista_elememtos, ++cursor));
    new_process->PID = *(int *)list_get(lista_elememtos, ++cursor);
    list_destroy_and_destroy_elements(lista_elememtos, &free);

    //Busco el archivo deseado
    char* path_buscado = string_duplicate(PATH_INSTRUCCIONES);
    string_append(&path_buscado, "/");
    string_append(&path_buscado, new_process->name);
    log_debug(MODULE_LOGGER, "Archivo Buscado: %s", path_buscado);

    //CREAR LISTA INST CON EL PARSER
    parser_file(path_buscado, instructions_list);

    new_process->number_of_instructions = list_size(instructions_list);
    new_process->instructions_list = instructions_list;
    new_process->pages_table = pages_table;
    list_add(lista_procesos, new_process);
    
    log_debug(MODULE_LOGGER, "Archivo leido: %s", path_buscado);

    //ENVIAR RTA OK A KERNEL --> En este caso solo envio el PID del proceso origen
    //message_send(PROCESS_CREATED, string_itoa(new_process->PID), FD_CLIENT_KERNEL);
    
}

void kill_process (int socketRecibido){
    int pid = 0; //int pid = atoi(message_receive(socketRecibido));
    t_Process* process = seek_process_by_pid(pid);
    t_Page* paginaBuscada;
    
    int size = list_size(process->pages_table);
    for (size_t i = size; i == 0 ; i--)
    {
        paginaBuscada = list_get(process->pages_table, i);
        t_Frame *marco = list_get(lista_marcos, paginaBuscada->assigned_frame);
        list_add(lista_marcos_libres, marco);
        free(paginaBuscada);
    }
    
}

void create_instruction(FILE* file, t_list* list_instruction) {

    t_CPU_Instruction* nueva_instruccion = malloc(sizeof(t_CPU_Instruction));
    char *linea = string_new();
    int tamanio_buffer = 0;
 
    getline(&linea, (size_t *restrict)&tamanio_buffer, file);

    if (linea[strlen(linea) - 1] == '\n') linea[strlen(linea) - 1] = '\0';
  
    char** campos = string_split(linea," ");

    nueva_instruccion->opcode = (e_Header)(campos[0]);
    nueva_instruccion->parameters = list_create();

    int numero_elementos= 0;
        while (campos[numero_elementos] != NULL) {
        numero_elementos++;
    }
    for (int pos = 1; pos < numero_elementos; pos++) 
	{
        char* parametro = string_new();
        parametro = campos[pos];
        list_add(nueva_instruccion->parameters, parametro); 
	}

	list_add(list_instruction, nueva_instruccion);

    free(linea);
}

void parser_file(char* path, t_list* list_instruction) {

    FILE* file;
    if ((file = fopen(path, "r")) == NULL)
    {
        log_error(MODULE_LOGGER, "[ERROR] No se pudo abrir el archivo de pseudocodigo indicado.");
        exit(EXIT_FAILURE);
    }
        
        while(!feof(file)) create_instruction(file, list_instruction);
       
        fclose(file);
    
}

void listen_cpu(int fd_cpu) {
    while(1) {
        e_CPU_Memory_Request memory_request = 0; //enum HeaderCode headerCode = package_receive_header(fd_cpu);
        switch (memory_request) {
            case INSTRUCTION_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de instruccion recibido.");
                seek_instruccion(fd_cpu);
                break;
                
            case FRAME_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de frame recibido.");
                respond_frame_request(fd_cpu);
                break;

            /*
            case DISCONNECTION_HEADERCODE:
                log_warning(MODULE_LOGGER, "Se desconecto CPU.");
                log_destroy(MODULE_LOGGER);
                return;
            */
                
            case PAGE_SIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                //message_send(PAGE_SIZE_REQUEST, string_itoa(TAM_PAGINA),FD_CLIENT_CPU);
                break;
            
            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
            }
    }
}

t_Process* seek_process_by_pid(int pidBuscado) {

    t_Process* procesoBuscado;
    int size= list_size(lista_procesos);

    procesoBuscado = list_get(lista_procesos,0); //SUPONEMOS QUE SIEMPRE ENCUENTRA EL PID

    for (size_t i = 0; i < size; i++)
    {
        procesoBuscado = list_get(lista_procesos,i);
        if (procesoBuscado->PID == pidBuscado) i=size;
        
    }
    
    return procesoBuscado;
}

void seek_instruccion(int socketRecibido) {
    t_list *lista_elememtos = NULL; //t_list *lista_elememtos = get_package_like_list(socketRecibido);
    int cursor = 0;
    int pid = *(int *)list_get(lista_elememtos, ++cursor);
    int pc = *(int *)list_get(lista_elememtos, ++cursor);
    list_destroy_and_destroy_elements(lista_elememtos, &free);

    
    t_Process* procesoBuscado = seek_process_by_pid(pid);
    //Suponemos que la instruccion es encontrada siempre
    t_CPU_Instruction* instruccionBuscada = list_get(procesoBuscado->instructions_list, pc);

    usleep(RETARDO_RESPUESTA * 1000);
    cpu_instruction_send(instruccionBuscada, FD_CLIENT_CPU);
    log_info(MODULE_LOGGER, "Instruccion enviada.");
}

void create_marcos(){
    int cantidad_marcos = TAM_MEMORIA / TAM_PAGINA;
    
    lista_marcos = list_create();
    lista_marcos_libres = list_create();

    for (size_t i = 0; i < cantidad_marcos; i++)
    {
        t_Frame *marcoNuevo = malloc(sizeof(t_Frame));
        marcoNuevo->id = i;
        marcoNuevo->assigned_page = NULL;
        marcoNuevo->PID = -1;
        list_add(lista_marcos, marcoNuevo);
        list_add(lista_marcos_libres, marcoNuevo);
    }
    
}

void free_marcos(){
    int cantidad_marcos = list_size(lista_marcos);
    t_Frame *marco_liberar;

    for (size_t i = cantidad_marcos; i == 0; i--)
    {
        marco_liberar = list_get(lista_marcos, i);
        free(marco_liberar);
    }

}

void respond_frame_request(int socketRecibido){
//Recibir parametros
  t_list *propiedadesPlanas = NULL; // t_list *propiedadesPlanas = get_package_like_list(socketRecibido);
  int cursor = 0;
  int pageBuscada = *(int*)list_get(propiedadesPlanas, cursor);
  int pidProceso = *(int*)list_get(propiedadesPlanas, cursor);
  list_destroy_and_destroy_elements(propiedadesPlanas, &free);

//Buscar frame
    t_Process* procesoBuscado = seek_process_by_pid(pidProceso);
    int marcoEncontrado = seek_marco_with_page_on_TDP(procesoBuscado->pages_table, pageBuscada);

//Respuesta    
    usleep(RETARDO_RESPUESTA * 1000);
    t_Package* package = package_create_with_header(FRAME_REQUEST);
    payload_add(package->payload, &pidProceso, sizeof(int));
    payload_add(package->payload, &marcoEncontrado, sizeof(int));
    package_send(package, FD_CLIENT_CPU);
}

int seek_marco_with_page_on_TDP (t_list* tablaPaginas, int pagina){
    t_Page* paginaBuscada;
    int marcoObjetivo = -1;
    int size= list_size(tablaPaginas);
    for(size_t i = 0; i < size ; i++) {
        paginaBuscada = list_get(tablaPaginas, i);
        if(paginaBuscada->number == pagina) {
            marcoObjetivo = paginaBuscada->assigned_frame;
            i = size + 1;
        }
    }

    return marcoObjetivo;
}

void read_memory(int socketRecibido) {
    t_list* parametros = NULL; // t_list* parametros = get_package_like_list(socketRecibido);
    int dir_fisica = *(int *)list_get(parametros,0);
    int pidBuscado = *(int *)list_get(parametros,1);
}


void write_memory(int socketRecibido){
    
}