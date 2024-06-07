/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";
char *MODULE_LOG_PATHNAME = "cpu.log";
char *MODULE_CONFIG_PATHNAME = "cpu.config";

t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
t_config *MODULE_CONFIG;

Server COORDINATOR_CPU_DISPATCH;
int FD_CLIENT_KERNEL_CPU_DISPATCH;
Server COORDINATOR_CPU_INTERRUPT;
int FD_CLIENT_KERNEL_CPU_INTERRUPT;
Connection CONNECTION_MEMORY;

int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

int size_pag; //momentaneo hasta que me llegue de memoria la size_pagina
long timestamp;
int direccion_logica; //momentaneo hasta ver de donde la saco
t_list  *tlb; //tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

pthread_mutex_t sem_mutex_tlb;

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

void read_module_config(t_config *MODULE_CONFIG) {
    CONNECTION_MEMORY = (struct Connection){.client_type = CPU_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
    COORDINATOR_CPU_DISPATCH = (struct Server){.server_type = CPU_DISPATCH_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH")};
    COORDINATOR_CPU_INTERRUPT = (struct Server){.server_type = CPU_INTERRUPT_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT")};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB");
}

void initialize_sockets(void) {
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

void finish_sockets(void) {
    close(COORDINATOR_CPU_DISPATCH.fd_listen);
    close(FD_CLIENT_KERNEL_CPU_DISPATCH);
    close(COORDINATOR_CPU_INTERRUPT.fd_listen);
    close(FD_CLIENT_KERNEL_CPU_INTERRUPT);
    close(CONNECTION_MEMORY.fd_connection);
}

void *cpu_dispatch_start_server_for_kernel(void *server_parameter) {
    Server *server = (Server *) server_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    server_start(server);

    while(1) {
        while(1) {
            log_info(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            FD_CLIENT_KERNEL_CPU_DISPATCH = server_accept(server->fd_listen);

            if (FD_CLIENT_KERNEL_CPU_DISPATCH != -1)
                break;
            else {
                log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            }
        }

        log_info(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(FD_CLIENT_KERNEL_CPU_DISPATCH, &handshake, sizeof(int32_t), MSG_WAITALL);

        if((enum PortType)handshake == server->clients_type)
            break;
        else {
            log_warning(SOCKET_LOGGER, "Error de Handshake con [Cliente] No reconocido");
            bytes = send(FD_CLIENT_KERNEL_CPU_DISPATCH, &resultError, sizeof(int32_t), 0);
            close(FD_CLIENT_KERNEL_CPU_DISPATCH);
        }
    }

    log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] %s", "Kernel");
    bytes = send(FD_CLIENT_KERNEL_CPU_DISPATCH, &resultOk, sizeof(int32_t), 0);

    return NULL;
}

void *cpu_interrupt_start_server_for_kernel(void *server_parameter) {
    Server *server = (Server *) server_parameter;

    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 0;
    int32_t resultError = -1;

    server_start(server);

    while(1) {
        while (1) {
            log_info(SOCKET_LOGGER, "Esperando [Cliente(s)] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            FD_CLIENT_KERNEL_CPU_INTERRUPT = server_accept(server->fd_listen);

            if(FD_CLIENT_KERNEL_CPU_INTERRUPT != -1)
                break;
            else {
                log_warning(SOCKET_LOGGER, "Fallo al aceptar [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);
            }
        }

        log_info(SOCKET_LOGGER, "Aceptado [Cliente] %s en Puerto: %s", PORT_NAMES[server->clients_type], server->port);

        bytes = recv(FD_CLIENT_KERNEL_CPU_INTERRUPT, &handshake, sizeof(int32_t), MSG_WAITALL);

        if((enum PortType)handshake == server->clients_type)
            break;
        else {
            log_warning(SOCKET_LOGGER, "Error Handshake con [Cliente] %s", "No reconocido");
            bytes = send(FD_CLIENT_KERNEL_CPU_INTERRUPT, &resultError, sizeof(int32_t), 0);
            close(FD_CLIENT_KERNEL_CPU_INTERRUPT);
        }
    }

    log_info(SOCKET_LOGGER, "OK Handshake con [Cliente] %s", "Kernel");
    bytes = send(FD_CLIENT_KERNEL_CPU_INTERRUPT, &resultOk, sizeof(int32_t), 0);

    return NULL;
}

void instruction_cycle(void) {

    Package *package;
    t_PCB *pcb;
    t_CPU_Instruction *instruction;

    tlb = list_create();

    while(1) {

        // 
        package = package_receive(FD_CLIENT_KERNEL_CPU_DISPATCH);
        switch((enum HeaderCode) package->header) {
            case PCB_HEADERCODE:
                pcb = pcb_deserialize(package->payload);
                pcb_print(pcb);
                break;
            default:
                log_error(SERIALIZE_LOGGER, "HeaderCode %d desconocido", package->header);
                exit(1);
                break;
        }
        package_destroy(package);
        //

        
        // 
        package = package_receive(FD_CLIENT_KERNEL_CPU_DISPATCH);
        switch((enum HeaderCode) package->header) {
            case CPU_INSTRUCTION_HEADERCODE:
                    instruction = cpu_instruction_deserialize(package->payload);
                    cpu_instruction_print(instruction);
                break;
            default:
                log_error(SERIALIZE_LOGGER, "HeaderCode %d desconocido", package->header);
                exit(1);
                break;
        }
        package_destroy(package);
        //

        t_CPU_Instruction *instruction_get;
        log_trace(MODULE_LOGGER, "PCB recibido del proceso : %i - Ciclo de instruccion ejecutando", pcb->PID);

        //Ejecuta lo que tenga que hacer el proceso hasta que llegue la interrupcion
        /*
        while(instruction->opcode == TYPE_INTERRUPT_SIN_INT){

            log_trace(MODULE_LOGGER, "Fetch de instruccion del proceso");
            instruction_get = list_get(instruction->parameters, pcb->PC);

            log_trace(MODULE_LOGGER,"Decode Y execute de instruccion del proceso");
            decode_execute(instruction_get, pcb);
            
        }
        */

    }
   
}

void decode_execute(t_CPU_Instruction *instruction, t_PCB *pcb) {
    // size_t largo_nombre = 0;
    int nro_page = 0;
    uint32_t value = 0;
    char *parameter = NULL;
    char *parameter2 = NULL;
    // char *recurso = NULL;
   //no sirveaca me aprece--> t_PCB new_pcb;
    t_register register_origin;
    t_register register_destination;

    int dir_logica_origin = 0;
    int dir_logica_destination = 0;

    int dir_fisica_origin = 0;
    int dir_fisica_destination = 0;

    uint32_t unit_work = 0;
    char *interfaz = NULL;
   // int size_pag = request_sizePag_memory(); // TODO: DESARROLLAR EN MEMORIA


   //inncesario aca me parece---->int nro_frame_required = request_frame_memory(nro_page, pcb->PID);

    switch ((enum t_CPU_Opcode) instruction->opcode)
    {
    case SET_OPCODE:

        parameter = list_get(instruction->parameters, 0);  // acarecibo el registro
        parameter2 = list_get(instruction->parameters, 1); // acarecibo el valor a asignarle al registro
        register_destination = string_to_register(parameter);
        value = atoi(parameter2); // aca recibo el valor a asignarle al registro
        register_destination = value;
        pcb->PC++;
        break;

    case MOV_IN_OPCODE:

        parameter = list_get(instruction->parameters, 0);  // de este registro debo saber la direccion logica
        parameter2 = list_get(instruction->parameters, 1); // de este registro debo saber la direccion logica

        register_origin = string_to_register(parameter);
        register_destination = string_to_register(parameter2);

        dir_logica_origin = atoi(parameter);
        dir_logica_destination = atoi(parameter2);

        dir_fisica_origin = mmu(dir_logica_origin, pcb, size_pag, register_origin, register_destination, IN);
        dir_fisica_destination = mmu(dir_logica_destination, pcb, size_pag, register_origin, register_destination, IN);

        dir_fisica_destination = dir_fisica_origin;
    
        break;

    case MOV_OUT_OPCODE:

        parameter = list_get(instruction->parameters, 0);  // de este registro debo saber la direccion logica
        parameter2 = list_get(instruction->parameters, 1); // de este registro debo saber la direccion logica

        register_origin = string_to_register(parameter);
        register_destination = string_to_register(parameter2);

        dir_logica_origin = atoi(parameter);
        dir_logica_destination = atoi(parameter2);

        dir_fisica_origin = mmu(dir_logica_origin, pcb, size_pag, register_origin, register_destination, OUT);
        dir_fisica_destination = mmu(dir_logica_destination, pcb, size_pag, register_origin, register_destination, OUT);

        dir_fisica_destination = dir_fisica_origin;
   
        break;

    case SUM_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        register_origin = string_to_register(parameter2);
        register_destination = register_destination + register_origin;
        pcb->PC++;
        break;

    case SUB_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        register_origin = string_to_register(parameter2);
        register_destination = register_destination - register_origin;
        
        break;

    case JNZ_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        value = atoi(parameter2);
        if (register_destination != 0)
        {
            pcb->PC = value;
        }
        pcb->PC++;

        break;

    case IO_GEN_SLEEP_OPCODE:

        interfaz = list_get(instruction->parameters, 0);
        unit_work = atoi(list_get(instruction->parameters, 1));
        usleep(unit_work);
        pcb->PC++;

        break;

    case EXIT_OPCODE:

            pcb->current_state = EXIT;
            //Saco de la TLB
            for(int i= list_size(tlb)-1 ; i>=0; i--){
                
                t_tlb *delete_tlb_entry = list_get(tlb, i);
                if(delete_tlb_entry->PID == pcb->PID){
                    list_remove(tlb, i);
                }
            }
            log_info(MODULE_LOGGER, "Proceso %i finalizado y en TLB", pcb->PID);
            
            break;

    default:

        log_error(MODULE_LOGGER, "Código %d desconocido.", instruction->opcode);
        exit(EXIT_FAILURE);
    }


/* CHEQUEAR EL TIPO DE INTERRUPCION  cuandoe ste todo desarrolado
    if(operation != TYPE_INTERRUPT_SIN_INT){


    }

    */

   //TODO:::: MANDAR POR PCB EL TIPO D EINTERRUPCION DEL PROCESO
}

int string_to_register(const char *string) {

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

int mmu(uint32_t dir_logica, t_PCB *pcb, int tamanio_pagina, int register_otrigin , int register_destination, int in_out) {

    int nro_page = floor(dir_logica / tamanio_pagina);
    int offset = dir_logica - nro_page * tamanio_pagina;
    int nro_frame_required = 0;
    int dir_fisica = 0;

    //CHEQUEO SI ESTA EN TLB EL FRAME QUE NECESITO
    pthread_mutex_lock(&sem_mutex_tlb); //DUDA CONE ESTO!!
    int frame_tlb = check_tlb(pcb->PID, nro_page);
    pthread_mutex_unlock(&sem_mutex_tlb); //DUDA CONE ESTO!!
  

    if(frame_tlb != -1){
        nro_frame_required = frame_tlb;
        log_info(MODULE_LOGGER,"PID: %i - TLB HIT - PAGINA: %i ", pcb->PID, nro_page);
        tlb_access(pcb, nro_page, nro_frame_required, dir_logica, register_otrigin, register_destination, in_out);

    }else{
        nro_frame_required = request_frame_memory(nro_page, pcb->PID); 
        log_info(MODULE_LOGGER, "PID: %i - TLB MISS - PAGINA: %i", pcb->PID, nro_page);
    }
   
    dir_fisica = nro_frame_required * tamanio_pagina + offset;
    return dir_fisica;
}

int check_tlb(int process_id, int nro_page) {

    t_tlb *tlb_entry = NULL;
    int nro_frame = -1;
    for (int i = 0; i < list_size(tlb); i++)
    {


        tlb_entry = list_get(tlb, i);
        if (tlb_entry->PID == process_id && tlb_entry->nro_page == nro_page)
        {
            nro_frame = tlb_entry->frame;


        // ==============TODO:FALTA ALGORITMO FIFO ===============//

            if (strcmp(ALGORITMO_TLB, "LRU") == 0)
            {
                tlb_entry->time = timestamp;
                timestamp++;
            }
        }
    }
    return nro_frame;
}


void tlb_access(t_PCB *pcb, int nro_page, int nro_frame_required, int direc ,int register_origin,  int register_destination,int in_out) {

    if(in_out == IN){

        //TODO : CON BRIAN PEDIR A MEMORIA QUE HAGA ESTAS FUNCIONES..
        request_data_in_memory(nro_frame_required, pcb->PID, nro_page, direc, register_origin, register_destination);
        log_info(MODULE_LOGGER, "PID: %i -Accion:LEER - Pagina: %i - Direccion Fisica: %i %i ", pcb->PID, nro_page, nro_frame_required, direc);
    }
    else{
        request_data_out_memory(nro_frame_required, pcb->PID, nro_page, direc, register_origin, register_destination);
        log_info(MODULE_LOGGER, "PID: %i -Accion:ESCRIBIR - Pagina: %i - Direccion Fisica: %i %i ", pcb->PID, nro_page, nro_frame_required, direc);
    }


}

void request_data_in_memory(int nro_frame_required, int pid, int nro_page, int direc, int register_origin, int register_destination) {

}

void request_data_out_memory(int nro_frame_required, int pid, int nro_page, int direc, int register_origin, int register_destination){

}

int request_frame_memory(int page, int pid) {
    Package* package = package_create_with_header(FRAME_REQUEST);
    payload_add(package->payload, &page, sizeof(int));
    payload_add(package->payload, &pid, sizeof(int));
    package_send(package, CONNECTION_MEMORY.fd_connection);
}