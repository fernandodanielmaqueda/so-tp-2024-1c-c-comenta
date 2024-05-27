
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "memoria.h"

char *module_name = "memoria";
char *module_log_pathname = "memoria.log";
char *module_config_pathname = "memoria.config";

t_log *module_logger;
extern t_log *connections_logger;
t_config *module_config;

void *memoria_principal;
pthread_t hilo_kernel;
pthread_t hilo_cpu;
pthread_t hilo_io;
t_list* lista_procesos;
t_list* lista_marcos;

Server COORDINATOR_MEMORY;
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
    memset(memoria_principal, (u_int32_t)'0', TAM_MEMORIA); //Llena de 0's el espacio de memoria
    lista_procesos = list_create();
    lista_marcos = list_create();

    initialize_sockets();

    log_info(module_logger, "Modulo %s inicializado correctamente\n", module_name);
 
 /*   
    pthread_create(&hilo_cpu, NULL, (void *)listen_cpu, (void *)fd_cpu);
    pthread_create(&hilo_kernel, NULL, (void *)listen_kernel, (void *)fd_kernel);
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

void read_module_config(t_config* module_config) {
    COORDINATOR_MEMORY = (struct Server) {.server_type = MEMORY_TYPE, .clients_type = TO_BE_DEFINED_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA")};
    TAM_MEMORIA = config_get_int_value(module_config, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(module_config, "TAM_PAGINA");
    PATH_INSTRUCCIONES = config_get_string_value(module_config, "PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(module_config, "RETARDO_RESPUESTA");
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
	Server *server = (Server*) server_parameter;

	int *fd_new_client;
	pthread_t thread_new_client;

	server_start(server);

	while(1) {
		fd_new_client = malloc(sizeof(int));
		log_info(connections_logger, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		*fd_new_client = server_accept(server->fd_listen);

		if(*fd_new_client == -1) {
			log_warning(connections_logger, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
			free(fd_new_client);
            sleep(2);
			continue;
		}

		log_info(connections_logger, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
		pthread_create(&thread_new_client, NULL, memory_client_handler, (void*) fd_new_client);
		pthread_detach(thread_new_client);
	}

	return NULL;
}

void *memory_client_handler(void *fd_new_client_parameter) {
	int* fd_new_client = (int*) fd_new_client_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    bytes = recv(*fd_new_client, &handshake, sizeof(int32_t), MSG_WAITALL);

    switch((enum PortType) handshake) {
        case KERNEL_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UN KERNEL MAS DE UNA VEZ
            log_info(connections_logger, "OK Handshake con [Cliente(s)] %s", "Kernel");
            FD_CLIENT_KERNEL = *fd_new_client;
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            sem_post(&sem_coordinator_kernel_client_connected);
        break;
        case CPU_TYPE:
            // REVISAR QUE NO SE PUEDA CONECTAR UNA CPU MAS DE UNA VEZ
            log_info(connections_logger, "OK Handshake con [Cliente(s)] %s", "CPU");
            FD_CLIENT_CPU = *fd_new_client;
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            sem_post(&sem_coordinator_cpu_client_connected);
        break;
        case IO_TYPE:
            log_info(connections_logger, "OK Handshake con [Cliente(s)] %s", "Entrada/Salida");
            bytes = send(*fd_new_client, &resultOk, sizeof(int32_t), 0);
            // Lógica de manejo de cliente Entrada/Salida
            free(fd_new_client);
        break;
        default:
            log_warning(connections_logger, "Error Handshake con [Cliente(s)] %s", "No reconocido");
            bytes = send(*fd_new_client, &resultError, sizeof(int32_t), 0);
            free(fd_new_client);
        break;
    }

    return NULL;
}

void listen_kernel(int fd_kernel) {
    while(1){
        t_opcode opcode = get_codOp(fd_kernel);
        switch (opcode)
            {
            case PROCESS_NEW:
                log_info(module_logger, "KERNEL: Proceso nuevo recibido.");
                create_process(fd_kernel);
                break;

            case DESCONEXION:
                log_warning(module_logger, "Se desconecto kernel.");
                log_destroy(module_logger);
                return;
            
            default:
                log_warning(module_logger, "Operacion desconocida..");
                break;
            }
    }
}

void create_process(int socketRecibido) {

    t_process* nuevo_proceso;
    t_list* lista_instrucciones = list_create();
    t_list* tabla_paginas = list_create();

    //Leo los valores recibidos por parametro
    t_list *lista_elememtos = get_package_like_list(socketRecibido);
    int cursor = 0;
    nuevo_proceso->nombre = string_duplicate(list_get(lista_elememtos, ++cursor));
    nuevo_proceso->pid = *(int *)list_get(lista_elememtos, ++cursor);
    list_destroy_and_destroy_elements(lista_elememtos, &free);

    //Busco el archivo deseado
    char* path_buscado = string_duplicate(PATH_INSTRUCCIONES);
    string_append(path_buscado, "/");
    string_append(path_buscado, nuevo_proceso->nombre);
    log_debug(module_logger, "Archivo Buscado: %s", path_buscado);

    //CREAR LISTA INST CON EL PARSER
    parser_file(path_buscado,lista_instrucciones);

    nuevo_proceso->cantidadInstrucciones = list_size(lista_instrucciones);
    nuevo_proceso->lista_instrucciones = lista_instrucciones;
    nuevo_proceso->tabla_paginas = tabla_paginas;
    list_add(lista_procesos,nuevo_proceso);
    
    log_debug(module_logger, "Archivo leido: %s", path_buscado);

    //ENVIAR RTA OK A KERNEL --> En este caso solo envio el pid del proceso origen
    send_message(PROCESS_CREATED, string_itoa(nuevo_proceso->pid), FD_CLIENT_KERNEL);
    
}

void create_instruction(FILE* file, t_list* list_instruction) {

    t_instruction_use* nueva_instruccion = malloc(sizeof(t_instruction_use));
    char *linea = string_new();
    int tamanio_buffer = 0;
 
    getline(&linea, (size_t *restrict)&tamanio_buffer, file);

    if (linea[strlen(linea) - 1] == '\n') linea[strlen(linea) - 1] = '\0';
  
    char** campos = string_split(linea," ");

    nueva_instruccion->operation = (t_opcode)(campos[0]);
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
        log_error(module_logger, "[ERROR] No se pudo abrir el archivo de pseudocodigo indicado.");
        exit(EXIT_FAILURE);
    }
        
        while(!feof(file)) create_instruction(file, list_instruction);
       
        fclose(file);
    
}

void listen_cpu(int fd_cpu) {
    while(1){
        t_opcode opcode = get_codOp(fd_cpu);
        switch (opcode)
            {
            case INSTUCTION_REQUEST:
                log_info(module_logger, "CPU: Pedido de instruccion recibido.");
                seek_instruccion(fd_cpu);
                break;

            case DESCONEXION:
                log_warning(module_logger, "Se desconecto CPU.");
                log_destroy(module_logger);
                return;
            
            default:
                log_warning(module_logger, "Operacion desconocida..");
                break;
            }
    }
}

t_process* seek_process_by_pid(int pidBuscado) {

    t_process* procesoBuscado;
    int size= list_size(lista_procesos);

    procesoBuscado = list_get(lista_procesos,0); //SUPONEMOS QUE SIEMPRE ENCUENTRA EL PID

    for (size_t i = 0; i < size; i++)
    {
        procesoBuscado = list_get(lista_procesos,i);
        if (procesoBuscado->pid == pidBuscado) i=size;
        
    }
    
    return procesoBuscado;
}

void seek_instruccion(int socketRecibido) {
    t_list *lista_elememtos = get_package_like_list(socketRecibido);
    int cursor = 0;
    int pid = *(int *)list_get(lista_elememtos, ++cursor);
    int pc = *(int *)list_get(lista_elememtos, ++cursor);
    list_destroy_and_destroy_elements(lista_elememtos, &free);

    
    t_process* procesoBuscado = seek_process_by_pid(pid);
    //Suponemos que la instruccion es encontrada siempre
    t_instruction_use* instruccionBuscada = list_get(procesoBuscado->lista_instrucciones,pc);

    send_instruccion(instruccionBuscada, FD_CLIENT_CPU);
    log_info(module_logger, "Instruccion enviada.");
}