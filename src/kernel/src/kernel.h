/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef KERNEL_H_
#define KERNEL_H_

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
#include "utils/socket.h"

int kernel(int, char*[]);
void obtener_configuracion(t_config* kernel_config);
void initialize_kernel();
void initialize_sockets();
void initialize_logger();
void initialize_config();
void cambiar_estado(t_pcb* pcb, int estado_nuevo);
t_pcb *create_pcb(char *instrucciones);
void iniciar_planificador_largo_plazo();
void iniciar_planificador_corto_plazo();
void iniciar_receptor_mensajes_cpu();
void planificador_largo_plazo();
void planificador_corto_plazo();
t_pcb *algoritmo_FIFO();
void receptor_mensajes_cpu();
int timenow();

#endif /* KERNEL_H_ */