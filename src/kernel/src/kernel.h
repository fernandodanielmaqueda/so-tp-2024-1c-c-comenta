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
void initialize_kernel();
void agregar_a_new(t_pcb* pcb);
void new_a_ready();
void initialize_sockets();
void initialize_logger();
void initialize_config();
void cambiar_estado(t_pcb* pcb, int estado_nuevo);
t_pcb *crear_pcb(char *instrucciones, int socket_consola);
void iniciar_planificador_largo_plazo();
void iniciar_planificador_corto_plazo();
void iniciar_receptor_mensajes_cpu();
void planificador_largo_plazo();
void planificador_corto_plazo();
t_pcb *algoritmo_FIFO();
void receptor_mensajes_cpu();
int timenow();

//listas globales de estados
t_list* LISTA_NEW;
t_list* LISTA_READY;
t_list* LISTA_EXEC;
t_list* LISTA_BLOCKED;
t_list* LISTA_EXIT;

pthread_mutex_t mutex_PID;
pthread_mutex_t mutex_LISTA_NEW;
pthread_mutex_t mutex_LISTA_READY;
pthread_mutex_t mutex_LISTA_BLOCKED;
pthread_mutex_t mutex_LISTA_EXEC;
pthread_mutex_t mutex_LISTA_EXIT;

pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;
pthread_t hilo_mensajes_cpu;

sem_t sem_planificador_largo_plazo;
sem_t sem_planificador_corto_plazo;
sem_t contador_multiprogramacion;



#endif /* KERNEL_H_ */