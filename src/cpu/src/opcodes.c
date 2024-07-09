/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "opcodes.h"

t_CPU_Operation CPU_OPERATIONS[] = {
    [SET_CPU_OPCODE] = {.name = "SET" , .function = set_cpu_operation},
    [MOV_IN_CPU_OPCODE] = {.name = "MOV_IN" , .function = mov_in_cpu_operation},
    [MOV_OUT_CPU_OPCODE] = {.name = "MOV_OUT" , .function = mov_out_cpu_operation},
    [SUM_CPU_OPCODE] = {.name = "SUM" , .function = sum_cpu_operation},
    [SUB_CPU_OPCODE] = {.name = "SUB" , .function = sub_cpu_operation},
    [JNZ_CPU_OPCODE] = {.name = "JNZ" , .function = jnz_cpu_operation},
    [RESIZE_CPU_OPCODE] = {.name = "RESIZE" , .function = resize_cpu_operation},
    [COPY_STRING_CPU_OPCODE] = {.name = "COPY_STRING" , .function = copy_string_cpu_operation},
    [WAIT_CPU_OPCODE] = {.name = "WAIT" , .function = wait_cpu_operation},
    [SIGNAL_CPU_OPCODE] = {.name = "SIGNAL" , .function = signal_cpu_operation},
    [IO_GEN_SLEEP_CPU_OPCODE] = {.name = "IO_GEN_SLEEP" , .function = io_gen_sleep_cpu_operation},
    [IO_STDIN_READ_CPU_OPCODE] = {.name = "IO_STDIN_READ" , .function = io_stdin_read_cpu_operation},
    [IO_STDOUT_WRITE_CPU_OPCODE] = {.name = "IO_STDOUT_WRITE" , .function = io_stdout_write_cpu_operation},
    [IO_FS_CREATE_CPU_OPCODE] = {.name = "IO_FS_CREATE" , .function = io_fs_create_cpu_operation},
    [IO_FS_DELETE_CPU_OPCODE] = {.name = "IO_FS_DELETE" , .function = io_fs_delete_cpu_operation},
    [IO_FS_TRUNCATE_CPU_OPCODE] = {.name = "IO_FS_TRUNCATE" , .function = io_fs_truncate_cpu_operation},
    [IO_FS_WRITE_CPU_OPCODE] = {.name = "IO_FS_WRITE" , .function = io_fs_write_cpu_operation},
    [IO_FS_READ_CPU_OPCODE] = {.name = "IO_FS_READ" , .function = io_fs_read_cpu_operation},
    [EXIT_CPU_OPCODE] = {.name = "EXIT" , .function = exit_cpu_operation}
};

int decode_instruction(char *name, e_CPU_OpCode *destination)
{
    
    if(name == NULL || destination == NULL)
        return 1;
    
    size_t opcodes_number = sizeof(CPU_OPERATIONS) / sizeof(CPU_OPERATIONS[0]);
    for (register e_CPU_OpCode cpu_opcode = 0; cpu_opcode < opcodes_number; cpu_opcode++)
        if (!strcmp(CPU_OPERATIONS[cpu_opcode].name, name)) {
            *destination = cpu_opcode;
            return 0;
        }

    return 1;
}

int set_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SET <REGISTRO> <VALOR>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "SET %s %s", argv[1], argv[2]);

    e_CPU_Register destination_register;
    if(decode_register(argv[1], &destination_register)) {
        log_error(MODULE_LOGGER, "<REGISTRO> %s no encontrado", argv[1]);
        return 1;
    }

    uint32_t value;
    if(str_to_uint32(argv[2], &value)) {
        log_error(MODULE_LOGGER, "%s: No es un valor valido", argv[2]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", PCB.PID, argv[0], argv[1], argv[2]);

    set_register_value(&PCB, destination_register, value);

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int mov_in_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_IN <REGISTRO DATOS> <REGISTRO DIRECCION>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "MOV_IN %s %s", argv[1], argv[2]);

    e_CPU_Register register_data;
    if (decode_register(argv[1], &register_data)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[1]);
        return 1;
    }

    e_CPU_Register register_address;
    if(decode_register(argv[2], &register_address)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[2]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Registro datos: %s - Registro direccion: %s ", PCB.PID, argv[0], argv[1], argv[2]);

    //Busco el valor (DL) del registro y lo asigno a la variable logical_address
    t_Logical_Address logical_address;
    get_register_value(PCB, register_address, &logical_address);

    size_t bytes = get_register_size(register_data);

    t_list *list_physical_addresses = mmu(PCB.PID, logical_address, bytes);
    void* leido = attend_read(PCB.PID, list_physical_addresses, bytes);
    set_register_value(&PCB, register_data, *((uint32_t*)leido));     

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int mov_out_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_OUT <REGISTRO DATOS> <REGISTRO DIRECCION>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "MOV_OUT %s %s", argv[1], argv[2]);

    e_CPU_Register register_address;
    if (decode_register(argv[1], &register_address)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[1]);
        return 1;
    }

    e_CPU_Register register_data;
    if(decode_register(argv[2], &register_data)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[2]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Registro datos: %s - Registro direccion: %s ", PCB.PID, argv[0], argv[1], argv[2]);

    //Busco el valor (DL) del registro y lo asigno a la variable logical_address
    t_Logical_Address logical_address;
    get_register_value(PCB, register_address, &logical_address);

    size_t bytes = get_register_size(register_data);
    void* data = NULL;
    get_register_value(PCB, register_data, data);

    t_list *list_physical_addresses = mmu(PCB.PID, logical_address, bytes);
    attend_write(PCB.PID, list_physical_addresses, bytes, data);

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int sum_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUM <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "SUM %s %s", argv[1], argv[2]);

    e_CPU_Register register_destination;
    e_CPU_Register register_origin;
    if (decode_register(argv[1], &register_destination)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[1]);
        return 1;
    }
    if(decode_register(argv[2], &register_origin)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[2]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s ", PCB.PID, argv[0], argv[1], argv[2]);


    uint32_t valor_reg_dest = 0;
    uint32_t valor_reg_origin = 0;
    get_register_value(PCB, register_destination, &valor_reg_dest);
    get_register_value(PCB, register_origin, &valor_reg_origin);

    set_register_value(&PCB, register_destination, (valor_reg_dest + valor_reg_origin));

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int sub_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUB <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "SUB %s %s", argv[1], argv[2]);

    e_CPU_Register register_destination;
    e_CPU_Register register_origin;
    if (decode_register(argv[1], &register_destination)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[1]);
        return 1;
    }
    if(decode_register(argv[2], &register_origin)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[2]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s", PCB.PID, argv[0], argv[1], argv[2]);

    uint32_t valor_reg_dest = 0;
    uint32_t valor_reg_origin = 0;
    get_register_value(PCB, register_destination, &valor_reg_dest);
    get_register_value(PCB, register_origin, &valor_reg_origin);

    set_register_value(&PCB, register_destination, (valor_reg_dest - valor_reg_origin));

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int jnz_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: JNZ <REGISTRO> <INSTRUCCION>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "JNZ %s %s", argv[1], argv[2]);

    e_CPU_Register register_name;
    if(decode_register(argv[1], &register_name)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[1]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", PCB.PID, argv[0], argv[1], argv[2]);

    uint32_t valor_reg_name = 0;
    get_register_value(PCB, valor_reg_name, &valor_reg_name);
    if (valor_reg_name == 0)
    {
        PCB.PC++;
    }
    else{
        PCB.PC = (t_PC)atoi(argv[2]);
    }
    
    SYSCALL_CALLED = 0;

    return 0;
}

int resize_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: RESIZE <TAMANIO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "RESIZE %s", argv[1]);

    //value = atoi(argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s ", PCB.PID, argv[0], argv[1]);

    //send_2int(PCB.PID, value, CONNECTION_MEMORY.fd_connection, RESIZE_REQUEST);

    t_MemorySize size = (t_MemorySize)atoi(argv[1]);

    t_Package *package = package_create_with_header(RESIZE_REQUEST);
    payload_enqueue(package->payload, &PCB.PID, sizeof(t_PID) );
    payload_enqueue(package->payload, &size, sizeof(t_MemorySize) );
    package_send(package, CONNECTION_MEMORY.fd_connection);
    package_destroy(package);
    

    package_receive(&package, CONNECTION_MEMORY.fd_connection);
    e_Header header_rec = package->header;
    package_destroy(package);
    if (package == NULL)
    {
        log_error(MODULE_LOGGER, "Error al recibir el paquete");
        return 1;
    }

    if (header_rec == RESIZE_REQUEST)
    {
        log_info(MODULE_LOGGER, "Se redimensiono correctamente");
    }
    else if (header_rec == OUT_OF_MEMORY)
    {// COMUNICAR CON KERNEL QUE NO HAY MAS MEMORIA
        t_Package *package_OFM = package_create_with_header(OUT_OF_MEMORY);
        pcb_serialize(package_OFM->payload, PCB);
        package_destroy(package_OFM);

        return 1;
    }

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int copy_string_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: COPY_STRING <TAMANIO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "COPY_STRING %s", argv[1]);

    //value = atoi(argv[1]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s", PCB.PID, argv[0], argv[1]);

    // COPY_STRING (Tamaño): Toma del string apuntado por el registro SI y copia la cantidad de bytes indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI.

    //Busco el valor (DL) del registro y lo asigno a la variable logical_address
    t_Logical_Address logical_address_origin;
    t_Logical_Address logical_address_destino;
    t_MemorySize bytes = (t_MemorySize)atoi(argv[1]);

    get_register_value(PCB, PCB.cpu_registers.SI, &logical_address_origin);
    get_register_value(PCB, PCB.cpu_registers.DI, &logical_address_destino);

    t_list *list_physical_addresses_origin = mmu(PCB.PID, logical_address_origin, bytes);
    void* data = attend_read(PCB.PID, list_physical_addresses_origin, bytes); 

    t_list *list_physical_addresses_dest = mmu(PCB.PID, logical_address_destino, bytes);
    attend_write(PCB.PID, list_physical_addresses_dest, bytes, data);

    PCB.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int wait_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: WAIT <RECURSO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "WAIT %s", argv[1]);

    PCB.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, WAIT_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);

    return 0;
}

int signal_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: SIGNAL <RECURSO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "SIGNAL %s", argv[1]);

    PCB.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, SIGNAL_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);

    return 0;
}

int io_gen_sleep_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

    PCB.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_GEN_SLEEP_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);

    return 0;
}

int io_stdin_read_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDIN_READ <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    e_CPU_Register register_address;
    if(decode_register(argv[2], &register_address)) {
        log_error(MODULE_LOGGER, "<REGISTRO DIRECCION> %s no encontrado", argv[2]);
        return 1;
    }

    e_CPU_Register register_size;
    if (decode_register(argv[3], &register_size)) {
        log_error(MODULE_LOGGER, "<REGISTRO TAMANIO> %s no encontrado", argv[3]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Interfaz: %s - Registro direccion: %s - Registro tamanio: %s", 
                    PCB.PID, argv[0], argv[1], argv[2], argv[3]);

    t_Logical_Address logical_address;
    t_MemorySize bytes = 0;

    get_register_value(PCB, register_address, &logical_address);
    get_register_value(PCB, register_size, &bytes);
    t_list *list_physical_addresses_origin = mmu(PCB.PID, logical_address, bytes);

    PCB.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_STDIN_READ_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    payload_enqueue(SYSCALL_INSTRUCTION, &bytes, sizeof(t_MemorySize));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses_origin, physical_address_serialize_element);

    return 0;
}

int io_stdout_write_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

    e_CPU_Register register_address;
    e_CPU_Register register_size;
    if(decode_register(argv[2], &register_address)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[2]);
        return 1;
    }
    if (decode_register(argv[3], &register_size)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[3]);
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Interfaz: %s - Registro direccion: %s - Registro tamanio: %s", 
                    PCB.PID, argv[0], argv[1], argv[2], argv[3]);

    t_Logical_Address logical_address;
    t_MemorySize bytes = 0;

    get_register_value(PCB, register_address, &logical_address);
    get_register_value(PCB, register_size, &bytes);
    t_list *list_physical_addresses_origin = mmu(PCB.PID, logical_address, bytes);

    PCB.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_STDOUT_WRITE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    payload_enqueue(SYSCALL_INSTRUCTION, &bytes, sizeof(t_MemorySize));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses_origin, physical_address_serialize_element);

    return 0;
}

int io_fs_create_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    PCB.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_CREATE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);

    return 0;
}

int io_fs_delete_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_DELETE <INTERFAZ> <NOMBRE ARCHIVO>");
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    PCB.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_DELETE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);

    return 0;
}

int io_fs_truncate_cpu_operation(int argc, char **argv)
{

    if (argc != 4)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_TRUNCATE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO TAMANIO>");
        return 1;
    }
    e_CPU_Register register_size;
    if(decode_register(argv[3], &register_size)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[2]);
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);

    t_MemorySize bytes = 0;
    get_register_value(PCB, register_size, &bytes);

    PCB.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_TRUNCATE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    payload_enqueue(SYSCALL_INSTRUCTION, &bytes, sizeof(t_MemorySize));

    return 0;
}

int io_fs_write_cpu_operation(int argc, char **argv)
{
    if (argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        return 1;
    }
    e_CPU_Register register_address_ptro;
    e_CPU_Register register_size_destino;
    e_CPU_Register register_size;
    if(decode_register(argv[3], &register_size_destino)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[3]);
        return 1;
    }
    if (decode_register(argv[4], &register_size)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[4]);
        return 1;
    }
    if (decode_register(argv[5], &register_address_ptro)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[5]);
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    t_Logical_Address logical_address_destino;
    t_MemorySize puntero = 0;
    t_MemorySize bytes = 0;

    get_register_value(PCB, register_size_destino, &logical_address_destino);
    get_register_value(PCB, register_address_ptro, &puntero);
    get_register_value(PCB, register_size, &bytes);
    t_list *list_physical_addresses_origin = mmu(PCB.PID, logical_address_destino, bytes);

    PCB.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_WRITE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    payload_enqueue(SYSCALL_INSTRUCTION, &puntero, sizeof(t_MemorySize));
    payload_enqueue(SYSCALL_INSTRUCTION, &bytes, sizeof(t_MemorySize));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses_origin, physical_address_serialize_element);

    return 0;
}

int io_fs_read_cpu_operation(int argc, char **argv)
{
    if (argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        return 1;
    }
    e_CPU_Register register_address_ptro;
    e_CPU_Register register_size_destino;
    e_CPU_Register register_size;
    if(decode_register(argv[3], &register_size_destino)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[3]);
        return 1;
    }
    if (decode_register(argv[4], &register_size)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[4]);
        return 1;
    }
    if (decode_register(argv[5], &register_address_ptro)) {
        log_error(MODULE_LOGGER, "Registro %s no encontrado", argv[5]);
        return 1;
    }
    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    t_Logical_Address logical_address_destino;
    t_MemorySize puntero = 0;
    t_MemorySize bytes = 0;

    get_register_value(PCB, register_size_destino, &logical_address_destino);
    get_register_value(PCB, register_address_ptro, &puntero);
    get_register_value(PCB, register_size, &bytes);
    t_list *list_physical_addresses_origin = mmu(PCB.PID, logical_address_destino, bytes);

    PCB.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_READ_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    payload_enqueue(SYSCALL_INSTRUCTION, &puntero, sizeof(t_MemorySize));
    payload_enqueue(SYSCALL_INSTRUCTION, &bytes, sizeof(t_MemorySize));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses_origin, physical_address_serialize_element);

    return 0;
}

int exit_cpu_operation(int argc, char **argv)
{

    if (argc != 1)
    {
        log_error(MODULE_LOGGER, "Uso: EXIT");
        return 1;
    }

    log_trace(MODULE_LOGGER, "EXIT");

    // Saco de la TLB
    for (int i = list_size(TLB) - 1; i >= 0; i--)
    {

        t_TLB *delete_tlb_entry = list_get(TLB, i);
        if (delete_tlb_entry->PID == PCB.PID)
        {
            list_remove(TLB, i);
        }
    }
    log_info(MODULE_LOGGER, "Proceso %i finalizado y en TLB", PCB.PID);

    PCB.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, EXIT_CPU_OPCODE);

    return 0;
}

int str_to_uint32(char *string, uint32_t *destination)
{
    char *end;

    *destination = (uint32_t) strtoul(string, &end, 10);

    if(!*string || *end)
        return 1;
        
    return 0;
}