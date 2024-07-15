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

int syscall_execute(t_Payload *syscall_instruction) {

	t_EnumValue aux;

    payload_dequeue(syscall_instruction, &aux, sizeof(t_EnumValue));
	    e_CPU_OpCode syscall_opcode = (e_CPU_OpCode) aux;

    if(SYSCALLS[syscall_opcode].function == NULL) {
        log_error(MODULE_LOGGER, "Funcion de syscall no encontrada");
        SYSCALL_PCB->exec_context.exit_reason = UNEXPECTED_ERROR_EXIT_REASON;
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
        free(resource_name);
        SYSCALL_PCB->exec_context.exit_reason = INVALID_RESOURCE_EXIT_REASON;
        return 1;
    }

    free(resource_name);

    resource->available--;
    if(resource->available < 0) {
        wait_draining_requests(&SCHEDULING_SYNC);
            pthread_mutex_lock(&(resource->shared_list_blocked.mutex));
                list_add(resource->shared_list_blocked.list, SYSCALL_PCB);
            pthread_mutex_unlock(&(resource->shared_list_blocked.mutex));
        signal_draining_requests(&SCHEDULING_SYNC);
        PCB_EXEC = 0;
    } else {
        PCB_EXEC = 1;
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
        free(resource_name);
        SYSCALL_PCB->exec_context.exit_reason = INVALID_RESOURCE_EXIT_REASON;
        return 1;
    }

    free(resource_name);

    PCB_EXEC = 1;

    resource->available++;
    if(resource->available <= 0) {
        wait_draining_requests(&SCHEDULING_SYNC);
            pthread_mutex_lock(&(resource->shared_list_blocked.mutex));

                if((resource->shared_list_blocked.list)->head == NULL) {
                    pthread_mutex_unlock(&(resource->shared_list_blocked.mutex));
                    signal_draining_requests(&SCHEDULING_SYNC);
                    return 0;
                }

                t_PCB *pcb = (t_PCB *) list_remove(resource->shared_list_blocked.list, 0);

            pthread_mutex_unlock(&(resource->shared_list_blocked.mutex));
            
            switch_process_state(pcb, READY_STATE);
        signal_draining_requests(&SCHEDULING_SYNC);
    }

    return 0;
}

int io_gen_sleep_kernel_syscall(t_Payload *syscall_arguments) {

    char *interface_name;
    text_deserialize(syscall_arguments, &interface_name);

    log_trace(MODULE_LOGGER, "IO_GEN_SLEEP %s", interface_name);

    PCB_EXEC = 0;

    t_Interface *interface = (t_Interface *) list_find_by_condition_with_comparation(LIST_INTERFACES, (bool (*)(void *, void *)) interface_name_matches, interface_name);
    if(interface == NULL) {
        log_warning(MODULE_LOGGER, "%s: la interfaz solicitada no existe y/o no esta conectada", interface_name);
        free(interface_name);
        SYSCALL_PCB->exec_context.exit_reason = INVALID_INTERFACE_EXIT_REASON;
        return 1;
    }

    free(interface_name);

    if(interface->io_type != GENERIC_IO_TYPE) {
        log_warning(MODULE_LOGGER, "%s: la interfaz no admite la operacion solicitada", interface->name);
        SYSCALL_PCB->exec_context.exit_reason = INVALID_INTERFACE_EXIT_REASON;
        return 1;
    }

    // TODO

    return 0;
}

int io_stdin_read_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_STDIN_READ %s %s %s", argv[1], argv[2], argv[3]);

    PCB_EXEC = 0;

    return 0;
}

int io_stdout_write_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_STDOUT_WRITE %s %s %s", argv[1], argv[2], argv[3]);

    PCB_EXEC = 0;
    
    return 0;
}

int io_fs_create_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_CREATE %s %s", argv[1], argv[2]);

    PCB_EXEC = 0;

    return 0;
}

int io_fs_delete_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_DELETE %s %s", argv[1], argv[2]);

    PCB_EXEC = 0;

    return 0;
}

int io_fs_truncate_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_TRUNCATE %s %s %s", argv[1], argv[2], argv[3]);

    PCB_EXEC = 0;
    
    return 0;
}

int io_fs_write_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_WRITE %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    PCB_EXEC = 0;

    return 0;
}

int io_fs_read_kernel_syscall(t_Payload *syscall_arguments) {

    // log_trace(MODULE_LOGGER, "IO_FS_READ %s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    PCB_EXEC = 0;

    return 0;
}

int exit_kernel_syscall(t_Payload *syscall_arguments) {

    log_trace(MODULE_LOGGER, "EXIT");

    PCB_EXEC = 0;

    return 0;
}