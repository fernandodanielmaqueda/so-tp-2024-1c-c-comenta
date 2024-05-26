#ifndef CONSOLA_H_
#define CONSOLA_H_


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
#include "utils/modules.h"
#include "utils/socket.h"
#include "kernel.h"
#include "scheduler.h"



void initalize_console_interactive();
bool validate_command_console(char* leido);
void attend_command_console(char* leido);
int pedir_enum_funcion(char** sublinea);
#endif /* CONSOLA_H_ */