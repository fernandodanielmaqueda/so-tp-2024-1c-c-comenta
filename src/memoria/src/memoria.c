
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

    initialize_sockets();

    log_info(module_logger, "Modulo %s inicializado correctamente\n", module_name);
 
 /*
    pthread_create(&hilo_cpu, NULL, (void *)manejar_conexion_cpu, (void *)fd_cpu);
    pthread_create(&hilo_kernel, NULL, (void *)manejar_conexion_kernel, (void *)fd_kernel);
    pthread_create(&hilo_io, NULL, (void *)manejar_conexion_io, (void *)fd_io);

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

void leer_archivo_pseudocodigo(char* nombreArchivo){

    //char* path_buscado = PATH_INSTRUCCIONES;
}