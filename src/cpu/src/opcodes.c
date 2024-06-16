/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "opcodes.h"

t_CPU_OpCode CPU_OPCODES[] = {
    {.name = "SET", .function = set_cpu_opcode},
    {.name = "MOV_IN", .function = mov_in_cpu_opcode},
    {.name = "MOV_OUT", .function = mov_out_cpu_opcode},
    {.name = "SUM", .function = sum_cpu_opcode},
    {.name = "SUB", .function = sub_cpu_opcode},
    {.name = "JNZ", .function = jnz_cpu_opcode},
    {.name = "RESIZE", .function = resize_cpu_opcode},
    {.name = "COPY_STRING", .function = copy_string_cpu_opcode},
    {.name = "WAIT", .function = wait_cpu_opcode},
    {.name = "SIGNAL", .function = signal_cpu_opcode},
    {.name = "IO_GEN_SLEEP", .function = io_gen_sleep_cpu_opcode},
    {.name = "IO_STDIN_READ", .function = io_stdin_read_cpu_opcode},
    {.name = "IO_STDOUT_WRITE", .function = io_stdout_write_cpu_opcode},
    {.name = "IO_FS_CREATE", .function = io_fs_create_cpu_opcode},
    {.name = "IO_FS_DELETE", .function = io_fs_delete_cpu_opcode},
    {.name = "IO_FS_TRUNCATE", .function = io_fs_truncate_cpu_opcode},
    {.name = "IO_FS_WRITE", .function = io_fs_write_cpu_opcode},
    {.name = "IO_FS_READ", .function = io_fs_read_cpu_opcode},
    {.name = "EXIT", .function = exit_cpu_opcode},
    {.name = NULL}};

t_CPU_OpCode *decode_instruction(char *name)
{
    for (register int i = 0; CPU_OPCODES[i].name != NULL; i++)
        if (!strcmp(CPU_OPCODES[i].name, name))
            return (&CPU_OPCODES[i]);

    return NULL;
}

int set_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SET <REGISTRO> <VALOR>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SET %s %s", IR->argv[1], IR->argv[2]);

    register_destination = string_to_register(IR->argv[1]);
    value = atoi(IR->argv[2]); // aca recibo el valor a asignarle al registro
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);
    register_destination = value;
    PCB->PC++;

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int mov_in_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_IN <REGISTRO DATOS> <REGISTRO DIRECCION>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "MOV_IN %s %s", IR->argv[1], IR->argv[2]);

    register_origin = string_to_register(IR->argv[1]);
    register_destination = string_to_register(IR->argv[2]);

    dir_logica_origin = atoi(IR->argv[1]);
    dir_logica_destination = atoi(IR->argv[2]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro datos: %s - Registro direccion: %s ", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);

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

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int mov_out_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: MOV_OUT <REGISTRO DIRECCION> <REGISTRO DATOS>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "MOV_OUT %s %s", IR->argv[1], IR->argv[2]);

    register_origin = string_to_register(IR->argv[1]);
    register_destination = string_to_register(IR->argv[2]);

    dir_logica_origin = atoi(IR->argv[1]);
    dir_logica_destination = atoi(IR->argv[2]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro direccion: %s - Registro datos: %s ", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);

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

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int sum_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUM <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SUM %s %s", IR->argv[1], IR->argv[2]);

    register_destination = string_to_register(IR->argv[1]);
    register_origin = string_to_register(IR->argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s ", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);
    register_destination = register_destination + register_origin;
    PCB->PC++;

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int sub_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: SUB <REGISTRO DESTINO> <REGISTRO ORIGEN>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SUB %s %s", IR->argv[1], IR->argv[2]);

    register_destination = string_to_register(IR->argv[0]);
    register_origin = string_to_register(IR->argv[1]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro destino: %s - Registro origen: %s", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);
    register_destination = register_destination - register_origin;

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int jnz_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: JNZ <REGISTRO> <INSTRUCCION>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "JNZ %s %s", IR->argv[1], IR->argv[2]);

    register_destination = string_to_register(IR->argv[1]);
    value = atoi(IR->argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro: %s - Valor: %s", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);
    if (register_destination != 0)
    {
        PCB->PC = value;
    }
    PCB->PC++;

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int resize_cpu_opcode(void)
{

    if (IR->argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: RESIZE <TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "RESIZE %s", IR->argv[1]);

    value = atoi(IR->argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s ", PCB->PID, IR->argv[0], IR->argv[1]);

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

int copy_string_cpu_opcode(void)
{

    if (IR->argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: COPY_STRING <TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "COPY_STRING %s", IR->argv[1]);

    value = atoi(IR->argv[1]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Tamaño: %s", PCB->PID, IR->argv[0], IR->argv[1]);

    // COPY_STRING (Tamaño): Toma del string apuntado por el registro SI y copia la cantidad de bytes indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI.
    register_origin = string_to_register(IR->argv[1]);
    register_destination = string_to_register(IR->argv[2]);

    dir_logica_origin = atoi(IR->argv[1]);
    dir_logica_destination = atoi(IR->argv[2]);

    dir_fisica_origin = mmu(dir_logica_origin, PCB, size_pag, register_origin, register_destination, IN);
    dir_fisica_destination = mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, IN);

    SYSCALL_CALLED = 0;
    return EXIT_SUCCESS;
}

int wait_cpu_opcode(void)
{

    if (IR->argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: WAIT <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "WAIT %s", IR->argv[1]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int signal_cpu_opcode(void)
{

    if (IR->argc != 2)
    {
        log_error(MODULE_LOGGER, "Uso: SIGNAL <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SIGNAL %s", IR->argv[1]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_gen_sleep_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", IR->argv[1], IR->argv[2]);

    unit_work = atoi(IR->argv[2]);
    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Interfaz: %s - Unidad de trabajo: %s", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);
    usleep(unit_work);
    PCB->PC++;

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_stdin_read_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDIN_READ <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", IR->argv[1], IR->argv[2], IR->argv[3]);

    register_origin = string_to_register(IR->argv[1]);
    register_destination = string_to_register(IR->argv[2]);

    dir_logica_origin = atoi(IR->argv[1]);
    dir_logica_destination = atoi(IR->argv[2]);

    log_info(MODULE_LOGGER, "PID: %d - Ejecutando instruccion: %s- Registro direccion: %s - Registro tamanio: %s", PCB->PID, IR->argv[0], IR->argv[1], IR->argv[2]);

    
   int direct_fisica =  mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, NULL);

    //ENVIO PAQUETE A KERNEL
    send_2int(direct_fisica, dir_logica_destination , SERVER_CPU_DISPATCH.client.fd_client, STDIN_REQUEST);

     
    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_stdout_write_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", IR->argv[1], IR->argv[2], IR->argv[3]);

    register_origin = string_to_register(IR->argv[1]);
    register_destination = string_to_register(IR->argv[2]);

    dir_logica_origin = atoi(IR->argv[1]);
    dir_logica_destination = atoi(IR->argv[2]);

    int direct_fisica = mmu(dir_logica_destination, PCB, size_pag, register_origin, register_destination, NULL);
    
    //ENVIO PAQUETE A KERNEL
    send_2int(direct_fisica, dir_logica_destination , SERVER_CPU_DISPATCH.client.fd_client, STDOUT_REQUEST);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_fs_create_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", IR->argv[1], IR->argv[2]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_fs_delete_cpu_opcode(void)
{

    if (IR->argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_DELETE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", IR->argv[1], IR->argv[2]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_fs_truncate_cpu_opcode(void)
{

    if (IR->argc != 4)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_TRUNCATE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", IR->argv[1], IR->argv[2], IR->argv[3]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_fs_write_cpu_opcode(void)
{
    if (IR->argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", IR->argv[1], IR->argv[2], IR->argv[3], IR->argv[4], IR->argv[5]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int io_fs_read_cpu_opcode(void)
{
    if (IR->argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", IR->argv[1], IR->argv[2], IR->argv[3], IR->argv[4], IR->argv[5]);

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}

int exit_cpu_opcode(void)
{

    if (IR->argc != 1)
    {
        log_error(MODULE_LOGGER, "Uso: EXIT");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "EXIT");

    PCB->current_state = EXIT_STATE;
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

    SYSCALL_CALLED = 1;
    return EXIT_SUCCESS;
}