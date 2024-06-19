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

e_CPU_OpCode decode_instruction(char *name)
{
    if(name == NULL)
        return -1;
    
    size_t opcodes_number = sizeof(CPU_OPERATIONS) / sizeof(CPU_OPERATIONS[0]);
    for (register e_CPU_OpCode cpu_opcode = 0; cpu_opcode < opcodes_number; cpu_opcode++)
        if (!strcmp(CPU_OPERATIONS[cpu_opcode].name, name))
            return cpu_opcode;

    return -1;
}

int set_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SET <REGISTRO> <VALOR>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SET %s %s", argv[1], argv[2]);

    register_destination = string_to_register(argv[1]);
    value = atoi(argv[2]); // aca recibo el valor a asignarle al registro
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", PCB->PID, argv[0], argv[1], argv[2]);
    register_destination = value;

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int mov_in_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_IN <REGISTRO DATOS> <REGISTRO DIRECCION>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "MOV_IN %s %s", argv[1], argv[2]);

    register_origin = string_to_register(argv[1]);
    register_destination = string_to_register(argv[2]);

    dir_logica_origin = atoi(argv[1]);
    dir_logica_destination = atoi(argv[2]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro datos: %s - Registro direccion: %s ", PCB->PID, argv[0], argv[1], argv[2]);

    // pedir tamanioo pagina a memoria
    send_2int(PCB->PID, value, CONNECTION_MEMORY.fd_connection, PAGE_SIZE_REQUEST);

    t_Package *package = package_receive(CONNECTION_MEMORY.fd_connection);
    if (package == NULL)
    {
        log_error(MODULE_LOGGER, "Error al recibir el paquete");
        exit(EXIT_FAILURE);
    }
    else
    {
        int size_pag = PAGE_SIZE_REQUEST;
    }

    dir_fisica_origin = mmu(dir_logica_origin, PCB, size_pag, register_origin, register_destination, IN);
    dir_fisica_destination = mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, IN);

    dir_fisica_destination = dir_fisica_origin;

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int mov_out_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_OUT <REGISTRO DIRECCION> <REGISTRO DATOS>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "MOV_OUT %s %s", argv[1], argv[2]);

    register_origin = string_to_register(argv[1]);
    register_destination = string_to_register(argv[2]);

    dir_logica_origin = atoi(argv[1]);
    dir_logica_destination = atoi(argv[2]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro direccion: %s - Registro datos: %s ", PCB->PID, argv[0], argv[1], argv[2]);

      // pedir tamanioo pagina a memoria
    send_2int(PCB->PID, value, CONNECTION_MEMORY.fd_connection, PAGE_SIZE_REQUEST);

    t_Package *package = package_receive(CONNECTION_MEMORY.fd_connection);
    if (package == NULL)
    {
        log_error(MODULE_LOGGER, "Error al recibir el paquete");
        exit(EXIT_FAILURE);
    }
    else
    {
        int size_pag = PAGE_SIZE_REQUEST;
    }

    dir_fisica_origin = mmu(dir_logica_origin, PCB, size_pag, register_origin, register_destination, OUT);
    dir_fisica_destination = mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, OUT);

    dir_fisica_destination = dir_fisica_origin;

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int sum_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUM <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SUM %s %s", argv[1], argv[2]);

    register_destination = string_to_register(argv[1]);
    register_origin = string_to_register(argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s ", PCB->PID, argv[0], argv[1], argv[2]);
    register_destination = register_destination + register_origin;

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int sub_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUB <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SUB %s %s", argv[1], argv[2]);

    register_destination = string_to_register(argv[0]);
    register_origin = string_to_register(argv[1]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s", PCB->PID, argv[0], argv[1], argv[2]);
    register_destination = register_destination - register_origin;

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int jnz_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: JNZ <REGISTRO> <INSTRUCCION>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "JNZ %s %s", argv[1], argv[2]);

    register_destination = string_to_register(argv[1]);
    value = atoi(argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", PCB->PID, argv[0], argv[1], argv[2]);
    if (register_destination != 0)
    {
        PCB->PC = value;
    }

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int resize_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: RESIZE <TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "RESIZE %s", argv[1]);

    value = atoi(argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s ", PCB->PID, argv[0], argv[1]);

    send_2int(PCB->PID, value, CONNECTION_MEMORY.fd_connection, RESIZE_REQUEST);

    t_Package *package = package_receive(CONNECTION_MEMORY.fd_connection);
    if (package == NULL)
    {
        log_error(MODULE_LOGGER, "Error al recibir el paquete");
        exit(EXIT_FAILURE);
    }

    if (package->header == RESIZE_REQUEST)
    {
        log_info(MODULE_LOGGER, "Se redimensiono correctamente");
    }
    else if (package->header == OUT_OF_MEMORY)
    {
        // COMUNICAR CON KERNEL QUE NO HAY MAS MEMORIA
        return EXIT_FAILURE;
    }

    package_destroy(package);

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int copy_string_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: COPY_STRING <TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "COPY_STRING %s", argv[1]);

    value = atoi(argv[1]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s", PCB->PID, argv[0], argv[1]);

    // COPY_STRING (Tamaño): Toma del string apuntado por el registro SI y copia la cantidad de bytes indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI.
    register_origin = string_to_register(argv[1]);
    register_destination = string_to_register(argv[2]);

    dir_logica_origin = atoi(argv[1]);
    dir_logica_destination = atoi(argv[2]);

    dir_fisica_origin = mmu(dir_logica_origin, PCB, size_pag, register_origin, register_destination, IN);
    dir_fisica_destination = mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, IN);

    PCB->PC++;

    SYSCALL_CALLED = 0;

    return EXIT_SUCCESS;
}

int wait_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: WAIT <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "WAIT %s", argv[1]);

    PCB->PC++;

    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = WAIT_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    return EXIT_SUCCESS;
}

int signal_cpu_operation(int argc, char **argv)
{

    if (argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: SIGNAL <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SIGNAL %s", argv[1]);

    PCB->PC++;

    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = SIGNAL_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);

    return EXIT_SUCCESS;
}

int io_gen_sleep_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

    PCB->PC++;

    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_GEN_SLEEP_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);

    return EXIT_SUCCESS;
}

int io_stdin_read_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDIN_READ <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    register_origin = string_to_register(argv[1]);
    register_destination = string_to_register(argv[2]);

    dir_logica_origin = atoi(argv[1]);
    dir_logica_destination = atoi(argv[2]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro direccion: %s - Registro tamanio: %s", PCB->PID, argv[0], argv[1], argv[2]);

    
   int direct_fisica =  mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, NULL);

    //ENVIO PAQUETE A KERNEL
    //send_2int(direct_fisica, dir_logica_destination , SERVER_CPU_DISPATCH.client.fd_client, STDIN_REQUEST);

    PCB->PC++;

    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_STDIN_READ_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    payload_enqueue(SYSCALL_INSTRUCTION, &(dir_logica_origin), sizeof(int));
    payload_enqueue(SYSCALL_INSTRUCTION, &(register_destination), sizeof(int));
 

    return EXIT_SUCCESS;
}

int io_stdout_write_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

    register_origin = string_to_register(argv[1]);
    register_destination = string_to_register(argv[2]);

    dir_logica_origin = atoi(argv[1]);
    dir_logica_destination = atoi(argv[2]);

    int direct_fisica = mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, NULL);
    
    //ENVIO PAQUETE A KERNEL
    send_2int(direct_fisica, dir_logica_destination , SERVER_CPU_DISPATCH.client.fd_client, STDOUT_REQUEST);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_STDOUT_WRITE_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    payload_enqueue(SYSCALL_INSTRUCTION, &(dir_logica_origin), sizeof(int));
    payload_enqueue(SYSCALL_INSTRUCTION, &(register_destination), sizeof(int));

    return EXIT_SUCCESS;
}

int io_fs_create_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_FS_CREATE_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_delete_cpu_operation(int argc, char **argv)
{

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_DELETE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_FS_DELETE_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_truncate_cpu_operation(int argc, char **argv)
{

    if (argc != 4)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_TRUNCATE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_FS_TRUNCATE_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    text_serialize(SYSCALL_INSTRUCTION, argv[3]);

    return EXIT_SUCCESS;
}

int io_fs_write_cpu_operation(int argc, char **argv)
{
    if (argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_FS_WRITE_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    text_serialize(SYSCALL_INSTRUCTION, argv[3]);
    text_serialize(SYSCALL_INSTRUCTION, argv[4]);
    text_serialize(SYSCALL_INSTRUCTION, argv[5]);

    return EXIT_SUCCESS;
}

int io_fs_read_cpu_operation(int argc, char **argv)
{
    if (argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = IO_FS_READ_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));
    text_serialize(SYSCALL_INSTRUCTION, argv[1]);
    text_serialize(SYSCALL_INSTRUCTION, argv[2]);
    text_serialize(SYSCALL_INSTRUCTION, argv[3]);
    text_serialize(SYSCALL_INSTRUCTION, argv[4]);
    text_serialize(SYSCALL_INSTRUCTION, argv[5]);

    return EXIT_SUCCESS;
}

int exit_cpu_operation(int argc, char **argv)
{

    if (argc != 1)
    {
        log_error(MODULE_LOGGER, "Uso: EXIT");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "EXIT");

    //PCB->current_state = EXIT_STATE;
    // Saco de la TLB
    for (int i = list_size(tlb) - 1; i >= 0; i--)
    {

        t_TLB *delete_tlb_entry = list_get(tlb, i);
        if (delete_tlb_entry->PID == PCB->PID)
        {
            list_remove(tlb, i);
        }
    }
    log_info(MODULE_LOGGER, "Proceso %i finalizado y en TLB", PCB->PID);

    PCB->PC++;
    
    SYSCALL_CALLED = 1;
    e_CPU_OpCode syscall_opcode = EXIT_CPU_OPCODE;
    cpu_opcode_serialize(SYSCALL_INSTRUCTION, &(syscall_opcode));

    return EXIT_SUCCESS;
}