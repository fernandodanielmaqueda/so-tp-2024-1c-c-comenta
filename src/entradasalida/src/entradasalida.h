#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

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




t_log* entrada_logger;
t_log* entrada_debug_logger;
t_config* entrada_config;

#endif /* ENTRADASALIDA_H_ */