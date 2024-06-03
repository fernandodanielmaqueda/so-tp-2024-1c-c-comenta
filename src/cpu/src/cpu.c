/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";
char *MODULE_LOG_PATHNAME = "cpu.log";
char *MODULE_CONFIG_PATHNAME = "cpu.config";

t_log *MODULE_LOGGER;
extern t_log *CONNECTIONS_LOGGER;
t_config *MODULE_CONFIG;

Server COORDINATOR_CPU_DISPATCH;
int FD_CLIENT_KERNEL_CPU_DISPATCH;
Server COORDINATOR_CPU_INTERRUPT;
int FD_CLIENT_KERNEL_CPU_INTERRUPT;
Connection CONNECTION_MEMORY;

int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

int module(int argc, char *argv[]) {

    initialize_loggers();
    initialize_configs();
    initialize_sockets();

    log_info(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

    instruction_cycle();

    // finish_threads();
    finish_sockets();
    // finish_configs();
    finish_loggers();

    return EXIT_SUCCESS;
}

void read_module_config(t_config *MODULE_CONFIG)
{
    CONNECTION_MEMORY = (struct Connection){.client_type = CPU_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
    COORDINATOR_CPU_DISPATCH = (struct Server){.server_type = CPU_DISPATCH_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH")};
    COORDINATOR_CPU_INTERRUPT = (struct Server){.server_type = CPU_INTERRUPT_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT")};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB");
}

void initialize_sockets(void)
{
    pthread_t thread_cpu_dispatch_start_server_for_kernel;
    pthread_t thread_cpu_interrupt_start_server_for_kernel;
    pthread_t thread_cpu_connect_to_memory;

    // [Server] CPU (Dispatch) <- [Cliente] Kernel
    pthread_create(&thread_cpu_dispatch_start_server_for_kernel, NULL, cpu_dispatch_start_server_for_kernel, (void *)&COORDINATOR_CPU_DISPATCH);
    // [Server] CPU (Interrupt) <- [Cliente] Kernel
    pthread_create(&thread_cpu_interrupt_start_server_for_kernel, NULL, cpu_interrupt_start_server_for_kernel, (void *)&COORDINATOR_CPU_INTERRUPT);
    // [Client] CPU -> [Server] Memoria
    pthread_create(&thread_cpu_connect_to_memory, NULL, client_thread_connect_to_server, (void *)&CONNECTION_MEMORY);

    // Se bloquea hasta que se realicen todas las conexiones
    pthread_join(thread_cpu_dispatch_start_server_for_kernel, NULL);
    pthread_join(thread_cpu_interrupt_start_server_for_kernel, NULL);
    pthread_join(thread_cpu_connect_to_memory, NULL);
}

void finish_sockets(void)
{
    close(COORDINATOR_CPU_DISPATCH.fd_listen);
    close(FD_CLIENT_KERNEL_CPU_DISPATCH);
    close(COORDINATOR_CPU_INTERRUPT.fd_listen);
    close(FD_CLIENT_KERNEL_CPU_INTERRUPT);
    close(CONNECTION_MEMORY.fd_connection);
}

void *cpu_dispatch_start_server_for_kernel(void *server_parameter)
{
    Server *server = (Server *)server_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    server_start(server);

    while (1)
    {
        while (1)
        {
            log_info(CONNECTIONS_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            FD_CLIENT_KERNEL_CPU_DISPATCH = server_accept(server->fd_listen);

            if (FD_CLIENT_KERNEL_CPU_DISPATCH != -1)
                break;
            else
            {
                log_warning(CONNECTIONS_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
                sleep(2);
            }
        }

        log_info(CONNECTIONS_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(FD_CLIENT_KERNEL_CPU_DISPATCH, &handshake, sizeof(int32_t), MSG_WAITALL);

        if ((enum PortType)handshake == server->clients_type)
            break;
        else
        {
            log_warning(CONNECTIONS_LOGGER, "Error de Handshake con [Cliente] No reconocido");
            bytes = send(FD_CLIENT_KERNEL_CPU_DISPATCH, &resultError, sizeof(int32_t), 0);
            close(FD_CLIENT_KERNEL_CPU_DISPATCH);
        }
    }

    log_info(CONNECTIONS_LOGGER, "OK Handshake con [Cliente] %s", "Kernel");
    bytes = send(FD_CLIENT_KERNEL_CPU_DISPATCH, &resultOk, sizeof(int32_t), 0);

    return NULL;
}

void *cpu_interrupt_start_server_for_kernel(void *server_parameter)
{
    Server *server = (Server *)server_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    server_start(server);

    while (1)
    {
        while (1)
        {
            log_info(CONNECTIONS_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            FD_CLIENT_KERNEL_CPU_INTERRUPT = server_accept(server->fd_listen);

            if (FD_CLIENT_KERNEL_CPU_INTERRUPT != -1)
                break;
            else
            {
                log_warning(CONNECTIONS_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
                sleep(2);
            }
        }

        log_info(CONNECTIONS_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(FD_CLIENT_KERNEL_CPU_INTERRUPT, &handshake, sizeof(int32_t), MSG_WAITALL);

        if ((enum PortType)handshake == server->clients_type)
            break;
        else
        {
            log_warning(CONNECTIONS_LOGGER, "Error Handshake con [Cliente] %s", "No reconocido");
            bytes = send(FD_CLIENT_KERNEL_CPU_INTERRUPT, &resultError, sizeof(int32_t), 0);
            close(FD_CLIENT_KERNEL_CPU_INTERRUPT);
        }
    }

    log_info(CONNECTIONS_LOGGER, "OK Handshake con [Cliente] %s", "Kernel");
    bytes = send(FD_CLIENT_KERNEL_CPU_INTERRUPT, &resultOk, sizeof(int32_t), 0);

    return NULL;
}

void instruction_cycle(void)
{

    //   t_contexto* context = recibe_contexto(fd_kernel);
}

void execute(t_instruction_use *instruction, t_pcb *pcb)
{
    // size_t largo_nombre = 0;
    // uint32_t dir_logica = 0;
    // uint32_t dir_fisica = 0;
    uint32_t value = 0;
    char *parameter = NULL;
    char *parameter2 = NULL;
    // char *recurso = NULL;
    t_pcb new_pcb;
    t_register register_origin;
    t_register register_destination;

    uint32_t unit_work = 0;
    char *interfaz = NULL;

    switch (instruction->operation)
    {
    case SET:
    {

        parameter = list_get(instruction->parameters, 0);  // acarecibo el registro
        parameter2 = list_get(instruction->parameters, 1); // acarecibo el valor a asignarle al registro
        register_destination = string_to_register(parameter);
        value = atoi(parameter2); // aca recibo el valor a asignarle al registro
        register_destination = value;
        break;
    }

    case SUM:
    {
        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        register_origin = string_to_register(parameter2);
        register_destination = register_destination + register_origin;
        break;
    }

    case SUB:
    {
        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        register_origin = string_to_register(parameter2);
        register_destination = register_destination - register_origin;
        break;
    }

    case JNZ:
    {
        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        value = atoi(parameter2);
        if (register_destination != 0)
        {
            pcb->pc = value;
        }
        break;
    }

     
    case IO_GEN_SLEEP:
    {
        interfaz = list_get(instruction->parameters, 0);
        unit_work = atoi(list_get(instruction->parameters, 1));   
        usleep(unit_work);
        break;
    }

    default:
    {
        log_error(MODULE_LOGGER, "Código %d desconocido.", instruction->operation);
        exit(EXIT_FAILURE);
    }
    }
}

int string_to_register(const char *string)
{
    if (strcmp(string, "AX") == 0)
    {
        return AX;
    }
    else if (strcmp(string, "BX") == 0)
    {
        return BX;
    }
    else if (strcmp(string, "CX") == 0)
    {
        return CX;
    }
    else if (strcmp(string, "DX") == 0)
    {
        return DX;
    }
    else if (strcmp(string, "EAX") == 0)
    {
        return EAX;
    }
    else if (strcmp(string, "EBX") == 0)
    {
        return EBX;
    }
    else if (strcmp(string, "ECX") == 0)
    {
        return ECX;
    }
    else if (strcmp(string, "EDX") == 0)
    {
        return EDX;
    }
    else if (strcmp(string, "RAX") == 0)
    {
        return RAX;
    }
    else if (strcmp(string, "RBX") == 0)
    {
        return RBX;
    }
    else if (strcmp(string, "RCX") == 0)
    {
        return RCX;
    }
    else if (strcmp(string, "SI") == 0)
    {
        return SI;
    }
    else if (strcmp(string, "DI") == 0)
    {
        return DI;
    }

    else
    {
        log_error(MODULE_LOGGER, "Se intentó convertir string a registro un parámetro que no es registro.");
        exit(EXIT_FAILURE);
    }
}
