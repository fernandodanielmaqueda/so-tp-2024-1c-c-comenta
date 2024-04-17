#ifndef KERNEL_H_
#define KERNEL_H_

#ifndef DEBUG 
#define DEBUG 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <utils/socket.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <utils/estructuras.h>

void obtener_configuracion(t_config* kernel_config);
void kernel();
void agregar_a_new(t_pcb* pcb);
void new_a_ready();
void initialize_sockets();
void initialize_logger();
void initialize_config();






#endif /* KERNEL_H_ */