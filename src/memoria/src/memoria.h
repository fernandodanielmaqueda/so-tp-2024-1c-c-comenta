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



void *memoria_principal;
pthread_t hilo_kernel;
pthread_t hilo_cpu;
pthread_t hilo_io;


typedef struct {
    char* nombre;
    int pid;
    int cantidadInstrucciones;
    t_list* lista_instrucciones;

}t_archivo_instruccion;

t_log* memoria_logger;
t_log* memoria_debug_logger;
t_config* memoria_config;

void initialize_logger();
void initialize_config();
void obtener_configuracion(t_config* memoria_config);
void initialize_sockets();
void memoria();

/**
 * @brief Busca el archivo de pseudocodigo y crea la estructura dentro de memoria
 * @param nombreArchivo Socket desde donde se va a recibir el pcb.
 */
void leer_archivo_pseudocodigo(char* nombreArchivo);

/**
 * @brief Busca la lista de instruccion y devuelve la instruccion buscada
 * @param pid Program counter requerido.
 * @param pc Program counter requerido.
 */
t_instruccion leer_instruccion(int pid,int pc);



#endif /* MEMORIA_H_ */