/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/socket.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/collections/list.h"
#include "utils/socket.h"

extern t_log* entrada_logger;
extern t_log* entrada_debug_logger;
extern t_config* entrada_config;

int entradasalida(int, char*[]);
void initialize_logger();
void initialize_config();
void obtener_configuracion(t_config* entrada_config);
void initialize_sockets();

#endif /* ENTRADASALIDA_H_ */