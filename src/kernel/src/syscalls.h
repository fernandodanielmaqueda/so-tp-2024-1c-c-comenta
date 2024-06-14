#ifndef KERNEL_SYSCALLS_H
#define KERNEL_SYSCALLS_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "commons/collections/dictionary.h"
#include "utils/module.h"
#include "utils/socket.h"
#include "kernel.h"

#define MAX_CONSOLE_ARGC 2 // 1 para el nombre del comando + 1 para el argumento

typedef struct t_Syscall {
    char *name;
    int (*function) (int, char*[]);
} t_Syscall;

extern t_Syscall SYSCALLS[];

t_Syscall *syscall_find(char *name);
int wait_kernel_syscall(int argc, char *argv[]);
int signal_kernel_syscall(int argc, char *argv[]);
int io_gen_sleep_kernel_syscall(int argc, char *argv[]);
int io_stdin_read_kernel_syscall(int argc, char *argv[]);
int io_stdout_write_kernel_syscall(int argc, char *argv[]);
int io_fs_create_kernel_syscall(int argc, char *argv[]);
int io_fs_delete_kernel_syscall(int argc, char *argv[]);
int io_fs_truncate_kernel_syscall(int argc, char *argv[]);
int io_fs_write_kernel_syscall(int argc, char *argv[]);
int io_fs_read_kernel_syscall(int argc, char *argv[]);
int exit_kernel_syscall(int argc, char *argv[]);

#endif // KERNEL_SYSCALLS_H