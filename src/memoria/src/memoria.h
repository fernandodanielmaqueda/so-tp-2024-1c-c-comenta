#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/memory.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/socket.h>


t_log* memoria_logger;
t_log* memoria_debug_logger;
t_config* memoria_config;

void initialize_logger();
void initialize_config();
void obtener_configuracion(t_config* memoria_config);
void initialize_sockets();
void memoria();



#endif /* MEMORIA_H_ */