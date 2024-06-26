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

	t_EnumValue aux;

    payload_dequeue(syscall_instruction, &aux, sizeof(t_EnumValue));
	    e_CPU_OpCode syscall_opcode = (e_CPU_OpCode) aux;

    if(SYSCALLS[syscall_opcode].function == NULL) {
        log_error(MODULE_LOGGER, "Funcion de syscall no encontrada");
        return 1;
    }

    return SYSCALLS[syscall_opcode].function(syscall_instruction);
}

int wait_kernel_syscall(t_Payload *syscall_arguments) {

    char *resource_name;
    text_deserialize(syscall_arguments, &resource_name);

    log_trace(MODULE_LOGGER, "WAIT %s", resource_name);

    t_Resource *resource = resource_find(resource_name);
    if(resource == NULL) {
        log_trace(MODULE_LOGGER, "WAIT %s: recurso no encontrado", resource_name);
        return 1;
    }

    free(resource_name);

    resource->available--;
    if(resource->available < 0) {
        list_add(resource->blocked_queue, SYSCALL_PCB);
        BLOCKING_SYSCALL = 1;
    } else {
        BLOCKING_SYSCALL = 0;
    }

    return 0;
}

int signal_kernel_syscall(t_Payload *syscall_arguments) {

    char *resource_name;
    text_deserialize(syscall_arguments, &resource_name);

    log_trace(MODULE_LOGGER, "SIGNAL %s", resource_name);

    t_Resource *resource = resource_find(resource_name);
    if(resource == NULL) {
        log_trace(MODULE_LOGGER, "SIGNAL %s: recurso no encontrado", resource_name);
        return 1;
    }

    free(resource_name);

    resource->available++;
    if(resource->available <= 0)
        if(list_size(resource->blocked_queue) > 0)
            switch_process_state((t_PCB *) list_get(resource->blocked_queue, 0), READY_STATE);
    
    BLOCKING_SYSCALL = 0;
    return 0;
}

int io_gen_sleep_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s %s", argv[1], argv[2]);

    return 0;
}

int io_stdin_read_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    return 0;
}

int io_stdout_write_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);
    
    return 0;
}

int io_fs_create_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    return 0;
}

int io_fs_delete_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    return 0;
}

int io_fs_truncate_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);
    
    return 0;
}

int io_fs_write_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return 0;
}

int io_fs_read_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    return 0;
}

int exit_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "EXIT");

    return 0;
}