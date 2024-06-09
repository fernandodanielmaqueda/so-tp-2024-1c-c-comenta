/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";
char *MODULE_LOG_PATHNAME = "cpu.log";
char *MODULE_CONFIG_PATHNAME = "cpu.config";

t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
t_config *MODULE_CONFIG;

// Tipos de interrupciones para el ciclo
int interruption_io;

int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

int size_pag;
long timestamp;
int direccion_logica; // momentaneo hasta ver de donde la saco
t_list *tlb;          // tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

pthread_mutex_t sem_mutex_tlb;

const char *t_instruction_type_string[] = {
    [SET_OPCODE] = "SET",
    [MOV_IN_OPCODE] = "MOV_IN",
    [MOV_OUT_OPCODE] = "MOV_OUT",
    [SUM_OPCODE] = "SUM",
    [SUB_OPCODE] = "SUB",
    [JNZ_OPCODE] = "JNZ",
    [RESIZE_OPCODE] = "RESIZE",
    [COPY_STRING_OPCODE] = "COPY_STRING",
    [WAIT_OPCODE] = "WAIT",
    [SIGNAL_OPCODE] = "SIGNAL",
    [IO_GEN_SLEEP_OPCODE] = "IO_GEN_SLEEP",
    [EXIT_OPCODE] = "EXIT",
    [IO_STDIN_READ_OPCODE] = "IO_STDIN_READ",
    [IO_STDOUT_WRITE_OPCODE] = "IO_STDOUT_WRITE",
    [IO_FS_CREATE_OPCODE] = "IO_FS_CREATE",
    [IO_FS_DELETE_OPCODE] = "IO_FS_DELETE",
    [IO_FS_TRUNCATE_OPCODE] = "IO_FS_TRUNCATE",
    [IO_FS_WRITE_OPCODE] = "IO_FS_WRITE",
    [IO_FS_READ_OPCODE] = "IO_FS_READ"};
const char *t_register_string[] = {
    [AX_REGISTER] = "AX",
    [BX_REGISTER] = "BX",
    [CX_REGISTER] = "CX",
    [DX_REGISTER] = "DX",
    [EAX_REGISTER] = "EAX",
    [EBX_REGISTER] = "EBX",
    [ECX_REGISTER] = "ECX",
    [EDX_REGISTER] = "EDX",
    [RAX_REGISTER] = "RAX",
    [RBX_REGISTER] = "RBX",
    [RCX_REGISTER] = "RCX",
    [RDX_REGISTER] = "RDX",
    [SI_REGISTER] = "SI",
    [DI_REGISTER] = "DI"};

/*

const char *t_interrupt_type_string[] = {
    [ERROR_CAUSE] = "ERROR_CAUSE",
    [SYSCALL_CAUSE] = "SYSCALL_CAUSE",
    [INTERRUPTION_CAUSE] = "INTERRUPTION_CAUSE"
};

*/

int module(int argc, char *argv[])
{

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
    CONNECTION_MEMORY = (t_Connection){.client_type = CPU_TYPE, .server_type = MEMORY_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
    SERVER_CPU_DISPATCH = (t_Single_Client_Server){.server = (t_Server){.server_type = CPU_DISPATCH_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH")}};
    SERVER_CPU_INTERRUPT = (t_Single_Client_Server){.server = (t_Server){.server_type = CPU_INTERRUPT_TYPE, .clients_type = KERNEL_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT")}};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB");
}

void instruction_cycle(void)
{

    t_PCB *pcb;
    t_CPU_Instruction *instruction;
    e_Interrupt *interrupt;

    tlb = list_create();

    while (1)
    {

        pcb = cpu_receive_pcb();
        instruction = cpu_receive_cpu_instruction();
        interrupt = cpu_receive_interrupt_type();

        t_CPU_Instruction *instruction_get;
        log_trace(MODULE_LOGGER, "PCB recibido del proceso : %i - Ciclo de instruccion ejecutando", pcb->PID);

        // Ejecuta lo que tenga que hacer el proceso hasta que llegue la interrupcion

        /*

        while(*interrupt == SYSCALL_CAUSE) // TYPE_INTERRUPT_SIN_INT
        {

            log_trace(MODULE_LOGGER, "Fetch de instruccion del proceso");
            instruction_get = list_get(instruction->parameters, pcb->PC);

            log_trace(MODULE_LOGGER, "Decode Y execute de instruccion del proceso");
            decode_execute(instruction_get, pcb);

            log_trace(MODULE_LOGGER, "Chequeo si llego interrupion del kernel");

            // CHEQUEAR EL TIPO DE INTERRUPCION


                    if(interrupt != TYPE_INTERRUPT_SIN_INT){

                        if()

                    }
            */
    }

    // TODO :: MANDO PCB CON LA INFO DEL FIN DE PROCESO

    t_Package *package = package_create_with_header(PCB_HEADER);
    pcb_serialize(package->payload, pcb);
    interrupt_serialize(package->payload, interrupt);
    // package_send
}

void decode_execute(t_CPU_Instruction *instruction, t_PCB *pcb)
{
    // size_t largo_nombre = 0;
    int nro_page = 0;
    uint32_t value = 0;
    char *parameter = NULL;
    char *parameter2 = NULL;
    // char *recurso = NULL;
    // no sirveaca me aprece--> t_PCB new_pcb;
    e_Register register_origin;
    e_Register register_destination;

    int dir_logica_origin = 0;
    int dir_logica_destination = 0;

    int dir_fisica_origin = 0;
    int dir_fisica_destination = 0;

    uint32_t unit_work = 0;
    char *interfaz = NULL;
    // int size_pag = request_sizePag_memory(); // TODO: DESARROLLAR EN MEMORIA
    // inncesario aca me parece---->int nro_frame_required = request_frame_memory(nro_page, pcb->PID);

    switch ((e_CPU_Opcode)instruction->opcode)
    {
    case SET_OPCODE:

        parameter = list_get(instruction->parameters, 0);  // acarecibo el registro
        parameter2 = list_get(instruction->parameters, 1); // acarecibo el valor a asignarle al registro
        register_destination = string_to_register(parameter);
        value = atoi(parameter2); // aca recibo el valor a asignarle al registro
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %d ", pcb->PID, t_instruction_type_string[instruction->opcode], t_register_string[register_destination], parameter2);
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

        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro datos: %s - Registro direccion: %s ", pcb->PID, t_instruction_type_string[instruction->opcode], t_register_string[register_origin], t_register_string[register_destination]);

        // preguntar a fer
        // message_send(PAGE_SIZE_REQUEST, "Tamanio Pag", SERVER_CPU_DISPATCH.client.fd_client);
        // size_pag = atoi(message_receive(SERVER_CPU_DISPATCH.client.fd_client));

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

        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro direccion: %s - Registro datos: %s ", pcb->PID, t_instruction_type_string[instruction->opcode], t_register_string[register_origin], t_register_string[register_destination]);

        // preguntar a fer
        // message_send(PAGE_SIZE_REQUEST, "Tamanio Pag", SERVER_CPU_DISPATCH.client.fd_client);
        // size_pag = atoi(message_receive(SERVER_CPU_DISPATCH.client.fd_client));

        dir_fisica_origin = mmu(dir_logica_origin, pcb, size_pag, register_origin, register_destination, OUT);
        dir_fisica_destination = mmu(dir_logica_destination, pcb, size_pag, register_origin, register_destination, OUT);

        dir_fisica_destination = dir_fisica_origin;

        break;

    case SUM_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        register_origin = string_to_register(parameter2);
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s ", pcb->PID, t_instruction_type_string[instruction->opcode], t_register_string[register_destination], t_register_string[register_origin]);
        register_destination = register_destination + register_origin;
        pcb->PC++;
        break;

    case SUB_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        register_origin = string_to_register(parameter2);
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s ", pcb->PID, t_instruction_type_string[instruction->opcode], t_register_string[register_destination], t_register_string[register_origin]);
        register_destination = register_destination - register_origin;

        break;

    case JNZ_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        parameter2 = list_get(instruction->parameters, 1);
        register_destination = string_to_register(parameter);
        value = atoi(parameter2);
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %d ", pcb->PID, t_instruction_type_string[instruction->opcode], t_register_string[register_destination], value);
        if (register_destination != 0)
        {
            pcb->PC = value;
        }
        pcb->PC++;

        break;

    case RESIZE_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        value = atoi(parameter);
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %d ", pcb->PID, t_instruction_type_string[instruction->opcode], value);
        // TODO: BRIAAN PEDIR A MEMORIA QUE HAGA ESTA FUNCION
        /*
        message_send(RESIZE_REQUEST, "Tamanio Pag", SERVER_CPU_DISPATCH.client.fd_client);

        if(message_receive(SERVER_CPU_DISPATCH.client.fd_client) == "Out of Memory"){

            //devuelvo contexto ejecucion a kernel


        }
        */

        pcb->PC++;
        break;

        // COPY_STRING (Tamaño): Toma del string apuntado por el registro SI y copia la cantidad de bytes indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI.

    case COPY_STRING_OPCODE:

        parameter = list_get(instruction->parameters, 0);
        value = atoi(parameter);
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %d ", pcb->PID, t_instruction_type_string[instruction->opcode], value);

        /*TODO:: DESARROLLAR*/

        break;

    case WAIT_OPCODE:

    case IO_GEN_SLEEP_OPCODE:

        interfaz = list_get(instruction->parameters, 0);
        unit_work = atoi(list_get(instruction->parameters, 1));
        log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Interfaz: %s - Unidad de trabajo: %d ", pcb->PID, t_instruction_type_string[instruction->opcode], interfaz, unit_work);
        usleep(unit_work);
        pcb->PC++;

        break;

    case EXIT_OPCODE:

        pcb->current_state = EXIT;
        // Saco de la TLB
        for (int i = list_size(tlb) - 1; i >= 0; i--)
        {

            t_TLB *delete_tlb_entry = list_get(tlb, i);
            if (delete_tlb_entry->PID == pcb->PID)
            {
                list_remove(tlb, i);
            }
        }
        log_info(MODULE_LOGGER, "Proceso %i finalizado y en TLB", pcb->PID);

        break;

    default:

        log_error(MODULE_LOGGER, "Código %d desconocido.", instruction->opcode);
        exit(EXIT_FAILURE);
    }

    // TODO:::: MANDAR POR PCB EL TIPO D EINTERRUPCION DEL PROCESO
}

int string_to_register(const char *string)
{

    if (strcmp(string, "AX") == 0)
        return AX_REGISTER;
    else if (strcmp(string, "BX") == 0)
        return BX_REGISTER;
    else if (strcmp(string, "CX") == 0)
        return CX_REGISTER;
    else if (strcmp(string, "DX") == 0)
        return DX_REGISTER;
    else if (strcmp(string, "EAX") == 0)
        return EAX_REGISTER;
    else if (strcmp(string, "EBX") == 0)
        return EBX_REGISTER;
    else if (strcmp(string, "ECX") == 0)
        return ECX_REGISTER;
    else if (strcmp(string, "EDX") == 0)
        return EDX_REGISTER;
    else if (strcmp(string, "RAX") == 0)
        return RAX_REGISTER;
    else if (strcmp(string, "RBX") == 0)
        return RBX_REGISTER;
    else if (strcmp(string, "RCX") == 0)
        return RCX_REGISTER;
    else if (strcmp(string, "SI") == 0)
        return SI_REGISTER;
    else if (strcmp(string, "DI") == 0)
        return DI_REGISTER;

    else
    {
        log_error(MODULE_LOGGER, "Se intentó convertir string a registro un parámetro que no es registro.");
        exit(EXIT_FAILURE);
    }
}

int mmu(uint32_t dir_logica, t_PCB *pcb, int tamanio_pagina, int register_otrigin, int register_destination, int in_out)
{

    int nro_page = floor(dir_logica / tamanio_pagina);
    int offset = dir_logica - nro_page * tamanio_pagina;
    int nro_frame_required = 0;
    int dir_fisica = 0;

    // CHEQUEO SI ESTA EN TLB EL FRAME QUE NECESITO
    pthread_mutex_lock(&sem_mutex_tlb); // DUDA CONE ESTO!!
    int frame_tlb = check_tlb(pcb->PID, nro_page);
    pthread_mutex_unlock(&sem_mutex_tlb); // DUDA CONE ESTO!!

    if (frame_tlb != -1)
    {
        nro_frame_required = frame_tlb;
        log_info(MODULE_LOGGER, "PID: %i - TLB HIT - PAGINA: %i ", pcb->PID, nro_page);
        tlb_access(pcb, nro_page, nro_frame_required, dir_logica, register_otrigin, register_destination, in_out);
    }
    else
    {
        nro_frame_required = request_frame_memory(nro_page, pcb->PID);
        log_info(MODULE_LOGGER, "PID: %i - TLB MISS - PAGINA: %i", pcb->PID, nro_page);
    }

    dir_fisica = nro_frame_required * tamanio_pagina + offset;
    return dir_fisica;
}

int check_tlb(int process_id, int page_number)
{

    t_TLB *tlb_entry = NULL;
    int frame_number = -1;
    for (int i = 0; i < list_size(tlb); i++)
    {

        tlb_entry = list_get(tlb, i);
        if (tlb_entry->PID == process_id && tlb_entry->page_number == page_number)
        {
            frame_number = tlb_entry->frame;

            // ==============TODO:FALTA ALGORITMO FIFO ===============//

            if (strcmp(ALGORITMO_TLB, "LRU") == 0)
            {
                tlb_entry->time = timestamp;
                timestamp++;
            }
        }
    }
    return frame_number;
}

void tlb_access(t_PCB *pcb, int nro_page, int frame_number_required, int direc, int register_origin, int register_destination, int in_out)
{

    if (in_out == IN)
    {

        // TODO : CON BRIAN PEDIR A MEMORIA QUE HAGA ESTAS FUNCIONES..
        request_data_in_memory(frame_number_required, pcb->PID, nro_page, direc, register_origin, register_destination);
        log_info(MODULE_LOGGER, "PID: %i -Accion:LEER - Pagina: %i - Direccion Fisica: %i %i ", pcb->PID, nro_page, frame_number_required, direc);
    }
    else
    {
        request_data_out_memory(frame_number_required, pcb->PID, nro_page, direc, register_origin, register_destination);
        log_info(MODULE_LOGGER, "PID: %i -Accion:ESCRIBIR - Pagina: %i - Direccion Fisica: %i %i ", pcb->PID, nro_page, frame_number_required, direc);
    }
}

void request_data_in_memory(int frame_number_required, int pid, int nro_page, int direc, int register_origin, int register_destination)
{
}

void request_data_out_memory(int frame_number_required, int pid, int nro_page, int direc, int register_origin, int register_destination)
{
}

t_PCB *cpu_receive_pcb(void)
{
    t_PCB *pcb;

    t_Package *package = package_receive(SERVER_CPU_DISPATCH.client.fd_client);
    switch (package->header)
    {
    case PCB_HEADER:
        pcb = pcb_deserialize(package->payload);
        break;
    default:
        log_error(SERIALIZE_LOGGER, "HeaderCode pcb %d desconocido", package->header);
        exit(1);
        break;
    }
    package_destroy(package);

    return pcb;
}

t_CPU_Instruction *cpu_receive_cpu_instruction(void)
{
    t_CPU_Instruction *instruction;

    t_Package *package = package_receive(SERVER_CPU_DISPATCH.client.fd_client);
    switch (package->header)
    {
    case CPU_INSTRUCTION_HEADER:
        instruction = cpu_instruction_deserialize(package->payload);
        break;
    default:
        log_error(SERIALIZE_LOGGER, "Header %d desconocido", package->header);
        exit(1);
        break;
    }
    package_destroy(package);

    return instruction;
}

e_Interrupt *cpu_receive_interrupt_type(void)
{

    e_Interrupt *interrupt;

    t_Package *package = package_receive(SERVER_CPU_INTERRUPT.client.fd_client);
    switch (package->header)
    {
    case INTERRUPT_HEADER:
        interrupt = interrupt_deserialize(package->payload);
        break;
    default:
        log_error(SERIALIZE_LOGGER, "Header interrupt %d desconocido", package->header);
        exit(1);
        break;
    }
    package_destroy(package);

    return interrupt;
}

int request_frame_memory(int page, int pid)

{
    t_Package *package = package_create_with_header(FRAME_REQUEST);
    payload_enqueue(package->payload, &page, sizeof(int));
    payload_enqueue(package->payload, &pid, sizeof(int));
    package_send(package, CONNECTION_MEMORY.fd_connection);
}
