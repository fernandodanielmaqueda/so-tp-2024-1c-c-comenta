/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *module_name = "cpu";
char *module_log_pathname = "cpu.log";
char *module_config_pathname = "cpu.config";

t_log* module_logger;
extern t_log *connections_logger;
t_config* module_config;

Server COORDINATOR_CPU_DISPATCH;
int FD_CLIENT_KERNEL_CPU_DISPATCH;
Server COORDINATOR_CPU_INTERRUPT;
int FD_CLIENT_KERNEL_CPU_INTERRUPT;
Connection CONNECTION_MEMORY;

int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs();
    initialize_sockets();

    log_info(module_logger, "Modulo %s inicializado correctamente\n", module_name);

    inicializar_ciclo_cpu();

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();

    return EXIT_SUCCESS;
}

void read_module_config(t_config* module_config) {
	CONNECTION_MEMORY = (struct Connection) {.client_type = CPU_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(module_config, "IP_MEMORIA"), .port = config_get_string_value(module_config, "PUERTO_MEMORIA")};
    COORDINATOR_CPU_DISPATCH = (struct Server) {.server_type = CPU_DISPATCH_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA_DISPATCH")};
	COORDINATOR_CPU_INTERRUPT = (struct Server) {.server_type = CPU_INTERRUPT_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(module_config, "PUERTO_ESCUCHA_INTERRUPT")};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(module_config, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(module_config, "ALGORITMO_TLB");
}

void initialize_sockets(void) {
	pthread_t thread_cpu_dispatch_start_server_for_kernel;
	pthread_t thread_cpu_interrupt_start_server_for_kernel;
	pthread_t thread_cpu_connect_to_memory;

	// [Server] CPU (Dispatch) <- [Cliente] Kernel
	pthread_create(&thread_cpu_dispatch_start_server_for_kernel, NULL, cpu_dispatch_start_server_for_kernel, (void*) &COORDINATOR_CPU_DISPATCH);
	// [Server] CPU (Interrupt) <- [Cliente] Kernel
	pthread_create(&thread_cpu_interrupt_start_server_for_kernel, NULL, cpu_interrupt_start_server_for_kernel, (void*) &COORDINATOR_CPU_INTERRUPT);
	// [Client] CPU -> [Server] Memoria
	pthread_create(&thread_cpu_connect_to_memory, NULL, client_thread_connect_to_server, (void*) &CONNECTION_MEMORY);

	// Se bloquea hasta que se realicen todas las conexiones
	pthread_join(thread_cpu_dispatch_start_server_for_kernel, NULL);
	pthread_join(thread_cpu_interrupt_start_server_for_kernel, NULL);
	pthread_join(thread_cpu_connect_to_memory, NULL);
}

void finish_sockets(void) {
	close(COORDINATOR_CPU_DISPATCH.fd_listen);
    close(FD_CLIENT_KERNEL_CPU_DISPATCH);
	close(COORDINATOR_CPU_INTERRUPT.fd_listen);
    close(FD_CLIENT_KERNEL_CPU_INTERRUPT);
	close(CONNECTION_MEMORY.fd_connection);
}

void *cpu_dispatch_start_server_for_kernel(void *server_parameter) {
	Server *server = (Server*) server_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

	server_start(server);

    while(1) {
        while(1) {
            log_info(connections_logger, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            FD_CLIENT_KERNEL_CPU_DISPATCH = server_accept(server->fd_listen);

            if(FD_CLIENT_KERNEL_CPU_DISPATCH != -1) break;
            else {
                log_warning(connections_logger, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
                sleep(2);
            }
        }

        log_info(connections_logger, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(FD_CLIENT_KERNEL_CPU_DISPATCH, &handshake, sizeof(int32_t), MSG_WAITALL);

        if((enum PortType) handshake == server->clients_type) break;
        else {
            log_warning(connections_logger, "Error de Handshake con [Cliente] No reconocido");
            bytes = send(FD_CLIENT_KERNEL_CPU_DISPATCH, &resultError, sizeof(int32_t), 0);
            close(FD_CLIENT_KERNEL_CPU_DISPATCH);
        }
    }

    log_info(connections_logger, "OK Handshake con [Cliente] %s", "Kernel");
    bytes = send(FD_CLIENT_KERNEL_CPU_DISPATCH, &resultOk, sizeof(int32_t), 0);

	return NULL;
}

void *cpu_interrupt_start_server_for_kernel(void *server_parameter) {
	Server *server = (Server*) server_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

	server_start(server);

    while(1) {
        while(1) {
            log_info(connections_logger, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            FD_CLIENT_KERNEL_CPU_INTERRUPT = server_accept(server->fd_listen);

            if(FD_CLIENT_KERNEL_CPU_INTERRUPT != -1) break;
            else {
                log_warning(connections_logger, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
                sleep(2);
            }
        }

        log_info(connections_logger, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(FD_CLIENT_KERNEL_CPU_INTERRUPT, &handshake, sizeof(int32_t), MSG_WAITALL);

        if((enum PortType) handshake == server->clients_type) break;
        else {
            log_warning(connections_logger, "Error Handshake con [Cliente] %s", "No reconocido");
            bytes = send(FD_CLIENT_KERNEL_CPU_INTERRUPT, &resultError, sizeof(int32_t), 0);
            close(FD_CLIENT_KERNEL_CPU_INTERRUPT);
        }
    }

    log_info(connections_logger, "OK Handshake con [Cliente] %s", "Kernel");
    bytes = send(FD_CLIENT_KERNEL_CPU_INTERRUPT, &resultOk, sizeof(int32_t), 0);

	return NULL;
}

void inicializar_ciclo_cpu(void) {
	// pthread_create(&hilo_ciclo_cpu, NULL, (void*)ciclo_cpu, NULL);
	// pthread_join(hilo_ciclo_cpu, NULL);
}

void ciclo_cpu(void){

	while(1){
		/*
        t_paquete* paquete = recibir_paquete(conexion_kernel);
		if (paquete == NULL) {
			continue;
		}
		t_pcb* pcb = deserializar_pcb(paquete->buffer);
		
        t_list* instrucciones = parsearInstrucciones(pcb->instrucciones);

        cicloInstruccion(pcb, instrucciones);

		list_destroy(instrucciones);
        */
	}
}

/* 
void cicloInstruccion(t_pcb *pcb, t_list *instrucciones)
{ // esto es para ejecutar una serie de instrucciones

    switch // (identificador de instruccion)
    {

    case SET:
    { 
        
    }
    case EXIT:
    {

    }
    //FALTA AGREGAR LAS INSTRUCCIONES FALTANTANTES
    //FALTA AGREGAR ALGO QUE MANDE MENSAJES A KERNEL Y DEMAS MODULOS
}
}
*/