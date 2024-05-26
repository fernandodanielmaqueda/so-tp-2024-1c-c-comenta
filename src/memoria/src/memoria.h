/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/string.h"
#include "commons/memory.h"
#include "commons/bitarray.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"
#include "utils/modules.h"
#include "utils/socket.h"

typedef struct t_process {
    char* nombre;
    int pid;
    int cantidadInstrucciones;
    t_list* lista_instrucciones;
    t_list* tabla_paginas;
} t_process;

typedef struct t_page {
    int nro_pagina;
    bool bit_uso;
    bool bit_modificado;
    bool bit_presencia;
    void* marco_asignado;
} t_page;

typedef struct t_marco {
    int pid;
    int marco_id;
    t_page* pagina_asignada;
} t_marco;

int module(int, char*[]);
void read_module_config(t_config *module_config);
void initialize_sockets(void);
void finish_sockets(void);
void *memory_start_server(void *server_parameter);
void *memory_client_handler(void *fd_new_client_parameter);

/**
 * @brief Busca el archivo de pseudocodigo y crea la estructura dentro de memoria
 * @param socket Socket desde donde se va a recibir el pcb.
 */
void create_process(int socket);

/**
 * @brief Busca la lista de instruccion y devuelve la instruccion buscada
 * @param pid Program counter requerido.
 * @param pc Program counter requerido.
 */
void seek_instruccion(int socket);

#endif /* MEMORIA_H_ */