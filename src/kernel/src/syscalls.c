#include "syscalls.h"

t_Syscall SYSCALLS[] = {
    {.name = "WAIT", .function = wait_kernel_syscall},
    {.name = "SIGNAL", .function = signal_kernel_syscall},
    {.name = "IO_GEN_SLEEP", .function = io_gen_sleep_kernel_syscall},
    {.name = "IO_STDIN_READ", .function = io_stdin_read_kernel_syscall},
    {.name = "IO_STDOUT_WRITE", .function = io_stdout_write_kernel_syscall},
    {.name = "IO_FS_CREATE", .function = io_fs_create_kernel_syscall},
    {.name = "IO_FS_DELETE", .function = io_fs_delete_kernel_syscall},
    {.name = "IO_FS_TRUNCATE", .function = io_fs_truncate_kernel_syscall},
    {.name = "IO_FS_WRITE", .function = io_fs_write_kernel_syscall},
    {.name = "IO_FS_READ", .function = io_fs_read_kernel_syscall},
    {.name = "EXIT", .function = exit_kernel_syscall},
    {.name = NULL}
};

t_Syscall *syscall_find(char *name) {
    for(register int i = 0; SYSCALLS[i].name != NULL; i++)
        if(!strcmp(SYSCALLS[i].name, name))
            return (&SYSCALLS[i]);

    return NULL;
}

int wait_kernel_syscall(int argc, char *argv[]) {

    if (argc != 2) {
        log_error(MODULE_LOGGER, "Uso: WAIT <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "WAIT %s", argv[1]);    

    return EXIT_SUCCESS;
}

int signal_kernel_syscall(int argc, char *argv[]) {

    if (argc != 2) {
        log_error(MODULE_LOGGER, "Uso: SIGNAL <RECURSO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "SIGNAL %s", argv[1]);

    return EXIT_SUCCESS;
}

int io_gen_sleep_kernel_syscall(int argc, char *argv[]) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_GEN_SLEEP <INTERFAZ> <UNIDADES DE TRABAJO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_stdin_read_kernel_syscall(int argc, char *argv[]) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDIN_READ <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    return EXIT_SUCCESS;
}

int io_stdout_write_kernel_syscall(int argc, char *argv[]) {
    
    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_STDOUT_WRITE <INTERFAZ> <REGISTRO DIRECCION> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);
    
    return EXIT_SUCCESS;
}

int io_fs_create_kernel_syscall(int argc, char *argv[]) {
    
    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_CREATE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_delete_kernel_syscall(int argc, char *argv[]) {

    if (argc != 3)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_DELETE <INTERFAZ> <NOMBRE ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    return EXIT_SUCCESS;
}

int io_fs_truncate_kernel_syscall(int argc, char *argv[]) {

    if (argc != 4)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_TRUNCATE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO TAMANIO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);
    
    return EXIT_SUCCESS;
}

int io_fs_write_kernel_syscall(int argc, char *argv[]) {
    if(argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_WRITE <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return EXIT_SUCCESS;
}

int io_fs_read_kernel_syscall(int argc, char *argv[]) {
    if(argc != 6)
    {
        log_error(MODULE_LOGGER, "Uso: IO_FS_READ <INTERFAZ> <NOMBRE ARCHIVO> <REGISTRO DIRECCION> <REGISTRO TAMANIO> <REGISTRO PUNTERO ARCHIVO>");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return EXIT_SUCCESS;
}

int exit_kernel_syscall(int argc, char *argv[]) {

    if(argc != 1)
    {
        log_error(MODULE_LOGGER, "Uso: EXIT");
        exit(EXIT_FAILURE);
    }

    log_trace(MODULE_LOGGER, "EXIT");

    return EXIT_SUCCESS;
}