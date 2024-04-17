#ifndef CPU_H_
#define CPU_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <utils/socket.h>


void initialize_logger();
void initialize_config();
void obtener_configuracion(t_config* cpu_config);
void cpu();
void initialize_sockets();


#endif /* CPU_H_ */