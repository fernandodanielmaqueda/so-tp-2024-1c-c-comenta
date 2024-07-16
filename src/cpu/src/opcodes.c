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
        if (strcmp(CPU_OPERATIONS[cpu_opcode].name, name) == 0) {
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
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "SET %s %s", argv[1], argv[2]);

    e_CPU_Register destination_register;
    if(decode_register(argv[1], &destination_register)) {
        log_error(MODULE_LOGGER, "<REGISTRO> %s no encontrado", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    uint32_t value;
    if(str_to_uint32(argv[2], &value)) {
        log_error(MODULE_LOGGER, "%s: No es un valor valido", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", EXEC_CONTEXT.PID, argv[0], argv[1], argv[2]);

    set_register_value(&EXEC_CONTEXT, destination_register, value);

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int mov_in_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_IN <REGISTRO DATOS> <REGISTRO DIRECCION>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "MOV_IN %s %s", argv[1], argv[2]);

    e_CPU_Register register_data;
    if (decode_register(argv[1], &register_data)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    e_CPU_Register register_address;
    if(decode_register(argv[2], &register_address)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Registro datos: %s - Registro direccion: %s ", EXEC_CONTEXT.PID, argv[0], argv[1], argv[2]);

    t_Logical_Address logical_address;
    get_register_value(EXEC_CONTEXT, register_address, &logical_address);

    size_t bytes = get_register_size(register_data);

    t_list *list_physical_addresses = mmu(EXEC_CONTEXT.PID, logical_address, bytes);

    void *destination = get_register_pointer(&EXEC_CONTEXT, register_data);
    void *source;
    attend_read(EXEC_CONTEXT.PID, list_physical_addresses, &source, bytes);
    memcpy(destination, source, bytes);
    free(source);

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int mov_out_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_OUT <REGISTRO DIRECCION> <REGISTRO DATOS>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "MOV_OUT %s %s", argv[1], argv[2]);

    e_CPU_Register register_address;
    if(decode_register(argv[1], &register_address)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    e_CPU_Register register_data;
    if(decode_register(argv[2], &register_data)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Registro direccion: %s - Registro datos: %s ", EXEC_CONTEXT.PID, argv[0], argv[1], argv[2]);

    void *source = get_register_pointer(&EXEC_CONTEXT, register_data);
    size_t bytes = get_register_size(register_data);

    t_Logical_Address logical_address;
    get_register_value(EXEC_CONTEXT, register_address, &logical_address);

    t_list *list_physical_addresses = mmu(EXEC_CONTEXT.PID, logical_address, bytes);
    attend_write(EXEC_CONTEXT.PID, list_physical_addresses, source, bytes);

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int sum_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUM <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "SUM %s %s", argv[1], argv[2]);

    e_CPU_Register register_destination;
    if (decode_register(argv[1], &register_destination)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    e_CPU_Register register_origin;
    if(decode_register(argv[2], &register_origin)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s ", EXEC_CONTEXT.PID, argv[0], argv[1], argv[2]);

    uint32_t value_register_destination;
    uint32_t value_register_origin;
    get_register_value(EXEC_CONTEXT, register_destination, &value_register_destination);
    get_register_value(EXEC_CONTEXT, register_origin, &value_register_origin);

    set_register_value(&EXEC_CONTEXT, register_destination, (value_register_destination + value_register_origin));

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int sub_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUB <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "SUB %s %s", argv[1], argv[2]);

    e_CPU_Register register_destination;
    if (decode_register(argv[1], &register_destination)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    e_CPU_Register register_origin;
    if(decode_register(argv[2], &register_origin)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s", (int) EXEC_CONTEXT.PID, argv[0], argv[1], argv[2]);

    uint32_t value_register_destination;
    uint32_t value_register_origin;
    get_register_value(EXEC_CONTEXT, register_destination, &value_register_destination);
    get_register_value(EXEC_CONTEXT, register_origin, &value_register_origin);

    set_register_value(&EXEC_CONTEXT, register_destination, (value_register_destination - value_register_origin));

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int jnz_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: JNZ <REGISTRO> <INSTRUCCION>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "JNZ %s %s", argv[1], argv[2]);

    e_CPU_Register cpu_register;
    if(decode_register(argv[1], &cpu_register)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    t_PC instruction;
    if(str_to_pc(argv[2], &instruction)) {
        log_error(MODULE_LOGGER, "%s: No es un valor valido", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", EXEC_CONTEXT.PID, argv[0], argv[1], argv[2]);

    uint32_t value_cpu_register;
    get_register_value(EXEC_CONTEXT, cpu_register, &value_cpu_register);

    if(value_cpu_register)
        EXEC_CONTEXT.PC = instruction;
    else
        EXEC_CONTEXT.PC++;
    
    SYSCALL_CALLED = 0;

    return 0;
}

int resize_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: RESIZE <TAMANIO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    t_MemorySize size;
    if(str_to_memory_size(argv[1], &size)) {
        log_error(MODULE_LOGGER, "%s: No es un tamaño valido", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "RESIZE %s", argv[1]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s ", EXEC_CONTEXT.PID, argv[0], argv[1]);

    t_Package *package = package_create_with_header(RESIZE_REQUEST);
    payload_append(package->payload, &(EXEC_CONTEXT.PID), sizeof(EXEC_CONTEXT.PID));
	payload_append(package->payload, &size, sizeof(size));
	package_send(package, CONNECTION_MEMORY.fd_connection);
	package_destroy(package);

    t_Return_Value return_value;
    receive_return_value_with_expected_header(RESIZE_REQUEST, &return_value, CONNECTION_MEMORY.fd_connection);
    if(return_value) {
        EVICTION_REASON = OUT_OF_MEMORY_EVICTION_REASON;
        return 1;
    }

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int copy_string_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: COPY_STRING <TAMANIO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    t_MemorySize size;
    if(str_to_memory_size(argv[1], &size)) {
        log_error(MODULE_LOGGER, "%s: No es un tamaño valido", argv[1]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "COPY_STRING %s", argv[1]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s", EXEC_CONTEXT.PID, argv[0], argv[1]);

    t_Logical_Address logical_address_si;
    get_register_value(EXEC_CONTEXT, SI_REGISTER, &logical_address_si);

    t_list *list_physical_addresses_si = mmu(EXEC_CONTEXT.PID, logical_address_si, size);
    
    void *source;
    attend_read(EXEC_CONTEXT.PID, list_physical_addresses_si, &source, size); 

    t_Logical_Address logical_address_di;
    get_register_value(EXEC_CONTEXT, DI_REGISTER, &logical_address_di);

    t_list *list_physical_addresses_di = mmu(EXEC_CONTEXT.PID, logical_address_di, size);

    attend_write(EXEC_CONTEXT.PID, list_physical_addresses_di, source, size);

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 0;

    return 0;
}

int wait_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: WAIT <RECURSO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "WAIT %s", argv[1]);

    EXEC_CONTEXT.PC++;

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
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "SIGNAL %s", argv[1]);

    EXEC_CONTEXT.PC++;

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
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

    EXEC_CONTEXT.PC++;

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
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    e_CPU_Register register_address;
    if(decode_register(argv[2], &register_address)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    e_CPU_Register register_size;
    if (decode_register(argv[3], &register_size)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[3]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Interfaz: %s - Registro direccion: %s - Registro tamanio: %s", 
        EXEC_CONTEXT.PID, argv[0], argv[1], argv[2], argv[3]);    

    t_MemorySize size;
    get_register_value(EXEC_CONTEXT, register_size, &size);

    t_Logical_Address logical_address;
    get_register_value(EXEC_CONTEXT, register_address, &logical_address);

    t_list *list_physical_addresses = mmu(EXEC_CONTEXT.PID, logical_address, size);

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_STDIN_READ_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    payload_append(SYSCALL_INSTRUCTION, &size, sizeof(size));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses, physical_address_serialize_element);

    return 0;
}

int io_stdout_write_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

    e_CPU_Register register_address;
    if(decode_register(argv[2], &register_address)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    e_CPU_Register register_size;
    if (decode_register(argv[3], &register_size)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[3]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s - Interfaz: %s - Registro direccion: %s - Registro tamanio: %s", 
        EXEC_CONTEXT.PID, argv[0], argv[1], argv[2], argv[3]);

    t_MemorySize size;
    get_register_value(EXEC_CONTEXT, register_size, &size);

    t_Logical_Address logical_address;
    get_register_value(EXEC_CONTEXT, register_address, &logical_address);

    t_list *list_physical_addresses = mmu(EXEC_CONTEXT.PID, logical_address, size);

    EXEC_CONTEXT.PC++;

    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_STDOUT_WRITE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    payload_append(SYSCALL_INSTRUCTION, &size, sizeof(size));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses, physical_address_serialize_element);

    return 0;
}

int io_fs_create_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    EXEC_CONTEXT.PC++;
    
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
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    EXEC_CONTEXT.PC++;
    
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
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    e_CPU_Register register_size;
    if(decode_register(argv[3], &register_size)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[2]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);

    t_MemorySize bytes = 0;
    get_register_value(EXEC_CONTEXT, register_size, &bytes);

    EXEC_CONTEXT.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_TRUNCATE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    payload_append(SYSCALL_INSTRUCTION, &bytes, sizeof(bytes));

    return 0;
}

int io_fs_write_cpu_operation(int argc, char **argv)
{
    if (argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    e_CPU_Register register_address_ptro;
    e_CPU_Register register_size_destino;
    e_CPU_Register register_size;
    if(decode_register(argv[3], &register_size_destino)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[3]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    if (decode_register(argv[4], &register_size)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[4]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    if (decode_register(argv[5], &register_address_ptro)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[5]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    t_Logical_Address logical_address_destino;
    t_MemorySize puntero = 0;
    t_MemorySize bytes = 0;

    get_register_value(EXEC_CONTEXT, register_size_destino, &logical_address_destino);
    get_register_value(EXEC_CONTEXT, register_address_ptro, &puntero);
    get_register_value(EXEC_CONTEXT, register_size, &bytes);
    t_list *list_physical_addresses_origin = mmu(EXEC_CONTEXT.PID, logical_address_destino, bytes);

    EXEC_CONTEXT.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_WRITE_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    payload_append(SYSCALL_INSTRUCTION, &puntero, sizeof(puntero));
    payload_append(SYSCALL_INSTRUCTION, &bytes, sizeof(bytes));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses_origin, physical_address_serialize_element);

    return 0;
}

int io_fs_read_cpu_operation(int argc, char **argv)
{
    if (argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    e_CPU_Register register_address_ptro;
    e_CPU_Register register_size_destino;
    e_CPU_Register register_size;
    if(decode_register(argv[3], &register_size_destino)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[3]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    if (decode_register(argv[4], &register_size)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[4]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    if (decode_register(argv[5], &register_address_ptro)) {
        log_error(MODULE_LOGGER, "%s: Registro no encontrado", argv[5]);
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }
    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    t_Logical_Address logical_address_destino;
    t_MemorySize puntero = 0;
    t_MemorySize bytes = 0;

    get_register_value(EXEC_CONTEXT, register_size_destino, &logical_address_destino);
    get_register_value(EXEC_CONTEXT, register_address_ptro, &puntero);
    get_register_value(EXEC_CONTEXT, register_size, &bytes);
    t_list *list_physical_addresses_origin = mmu(EXEC_CONTEXT.PID, logical_address_destino, bytes);

    EXEC_CONTEXT.PC++;
    
    SYSCALL_CALLED = 1;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, IO_FS_READ_CPU_OPCODE);
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    payload_append(SYSCALL_INSTRUCTION, &puntero, sizeof(puntero));
    payload_append(SYSCALL_INSTRUCTION, &bytes, sizeof(bytes));
    list_serialize(SYSCALL_INSTRUCTION, *list_physical_addresses_origin, physical_address_serialize_element);

    return 0;
}

int exit_cpu_operation(int argc, char **argv)
{

    if (argc != 1)
    {
        log_error(MODULE_LOGGER, "Uso: EXIT");
        EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
        return 1;
    }

    log_trace(MODULE_LOGGER, "EXIT");

    // Saco de la TLB
    for (int i = list_size(TLB) - 1; i >= 0; i--)
    {

        t_TLB *delete_tlb_entry = list_get(TLB, i);
        if (delete_tlb_entry->PID == EXEC_CONTEXT.PID)
        {
            list_remove(TLB, i);
        }
    }
    log_info(MODULE_LOGGER, "Proceso %i finalizado y en TLB", EXEC_CONTEXT.PID);

    EXEC_CONTEXT.PC++;
    
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

int str_to_memory_size(char *string, t_MemorySize *destination)
{
    char *end;

    *destination = (t_MemorySize) strtoul(string, &end, 10);

    if(!*string || *end)
        return 1;
        
    return 0;
}


int str_to_pc(char *string, t_PC *destination)
{
    char *end;

    *destination = (t_PC) strtoul(string, &end, 10);

    if(!*string || *end)
        return 1;
        
    return 0;
}