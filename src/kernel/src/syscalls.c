#include "syscalls.h"

t_Syscall SYSCALLS[] = {
    [WAIT_CPU_OPCODE] = {.name = "WAIT" , .function = wait_kernel_syscall},
    [SIGNAL_CPU_OPCODE] = {.name = "SIGNAL" , .function = signal_kernel_syscall},
    [IO_GEN_SLEEP_CPU_OPCODE] = {.name = "IO_GEN_SLEEP" , .function = io_gen_sleep_kernel_syscall},
    [IO_STDIN_READ_CPU_OPCODE] = {.name = "IO_STDIN_READ" , .function = io_stdin_read_kernel_syscall},
    [IO_STDOUT_WRITE_CPU_OPCODE] = {.name = "IO_STDOUT_WRITE" , .function = io_stdout_write_kernel_syscall},
    [IO_FS_CREATE_CPU_OPCODE] = {.name = "IO_FS_CREATE" , .function = io_fs_create_kernel_syscall},
    [IO_FS_DELETE_CPU_OPCODE] = {.name = "IO_FS_DELETE" , .function = io_fs_delete_kernel_syscall},
    [IO_FS_TRUNCATE_CPU_OPCODE] = {.name = "IO_FS_TRUNCATE" , .function = io_fs_truncate_kernel_syscall},
    [IO_FS_WRITE_CPU_OPCODE] = {.name = "IO_FS_WRITE" , .function = io_fs_write_kernel_syscall},
    [IO_FS_READ_CPU_OPCODE] = {.name = "IO_FS_READ" , .function = io_fs_read_kernel_syscall},
    [EXIT_CPU_OPCODE] = {.name = "EXIT" , .function = exit_kernel_syscall}
};

t_PCB *SYSCALL_PCB;

int BLOCKING_SYSCALL;

int syscall_execute(t_Payload *syscall_instruction) {

    e_CPU_OpCode *syscall_opcode;
    payload_dequeue(syscall_instruction, &syscall_opcode, sizeof(t_EnumValue));

    int (*syscall) (t_Payload *) = syscall_find(*syscall_opcode);
    if(syscall == NULL) {
        log_error(MODULE_LOGGER, "Syscall %d no encontrada", *syscall_opcode);
        exit(EXIT_FAILURE);
    }

    cpu_opcode_free(syscall_opcode);

    return syscall(syscall_instruction);
}

int (*syscall_find(e_CPU_OpCode syscall_opcode)) (t_Payload *) {
    switch(syscall_opcode) {
        case WAIT_CPU_OPCODE:
            return wait_kernel_syscall;
        case SIGNAL_CPU_OPCODE:
            return signal_kernel_syscall;
        case IO_GEN_SLEEP_CPU_OPCODE:
            return io_gen_sleep_kernel_syscall;
        case IO_STDIN_READ_CPU_OPCODE:
            return io_stdin_read_kernel_syscall;
        case IO_STDOUT_WRITE_CPU_OPCODE:
            return io_stdout_write_kernel_syscall;
        case IO_FS_CREATE_CPU_OPCODE:
            return io_fs_create_kernel_syscall;
        case IO_FS_DELETE_CPU_OPCODE:
            return io_fs_delete_kernel_syscall;
        case IO_FS_TRUNCATE_CPU_OPCODE:
            return io_fs_truncate_kernel_syscall;
        case IO_FS_WRITE_CPU_OPCODE:
            return io_fs_write_kernel_syscall;
        case IO_FS_READ_CPU_OPCODE:
            return io_fs_read_kernel_syscall;
        case EXIT_CPU_OPCODE:
            return exit_kernel_syscall;
        default:
            return NULL;
    }
}

int wait_kernel_syscall(t_Payload *syscall_arguments) {

    if (argc != 2) {
        log_error(MODULE_LOGGER, "Uso: WAIT <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "WAIT %s", argv[1]);

    t_Resource *resource = resource_find(argv[1]);
    if(resource == NULL) {
        log_trace(MODULE_LOGGER, "WAIT %s: recurso no encontrado", argv[1]);
        return EXIT_FAILURE;
    }

    resource->available--;
    if(resource->available < 0) {
        list_add(resource->blocked_queue, SYSCALL_PCB);
        BLOCKING_SYSCALL = 1;
    } else {
        BLOCKING_SYSCALL = 0;
    }

    return EXIT_SUCCESS;
}

int signal_kernel_syscall(t_Payload *syscall_arguments) {

    if (argc != 2) {
        log_error(MODULE_LOGGER, "Uso: SIGNAL <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SIGNAL %s", argv[1]);

    t_Resource *resource = resource_find(argv[1]);
    if(resource == NULL) {
        log_trace(MODULE_LOGGER, "SIGNAL %s: recurso no encontrado", argv[1]);
        return EXIT_FAILURE;
    }

    resource->available++;
    if(resource->available <= 0)
        if(list_size(resource->blocked_queue) > 0)
            switch_process_state((t_PCB *) list_get(resource->blocked_queue, 0), READY_STATE);
    
    BLOCKING_SYSCALL = 0;
    return EXIT_SUCCESS;
}

int io_gen_sleep_kernel_syscall(t_Payload *syscall_arguments) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

    // TODO

    return EXIT_SUCCESS;
}

int io_stdin_read_kernel_syscall(t_Payload *syscall_arguments) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDIN_READ <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    // TODO

    return EXIT_SUCCESS;
}

int io_stdout_write_kernel_syscall(t_Payload *syscall_arguments) {
    
    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

    // TODO
    
    return EXIT_SUCCESS;
}

int io_fs_create_kernel_syscall(t_Payload *syscall_arguments) {
    
    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_delete_kernel_syscall(t_Payload *syscall_arguments) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_DELETE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_truncate_kernel_syscall(t_Payload *syscall_arguments) {

    if (argc != 4)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_TRUNCATE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);
    
    return EXIT_SUCCESS;
}

int io_fs_write_kernel_syscall(t_Payload *syscall_arguments) {
    if(argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return EXIT_SUCCESS;
}

int io_fs_read_kernel_syscall(t_Payload *syscall_arguments) {
    if(argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return EXIT_SUCCESS;
}

int exit_kernel_syscall(t_Payload *syscall_arguments) {

    if(argc != 1)
    {
        log_error(MODULE_LOGGER, "Uso: EXIT");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "EXIT");

    return EXIT_SUCCESS;
}