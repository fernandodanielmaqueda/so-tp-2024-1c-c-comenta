/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef MEMORIA_H
#define MEMORIA_H

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
#include "utils/serialize.h"
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
    int marco_asignado;
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

void listen_kernel(int fd_kernel);

/**
 * @brief Busca el archivo de pseudocodigo y crea la estructura dentro de memoria
 * @param socketRecibido Socket desde donde se va a recibir el pcb.
 */
void create_process(int socketRecibido);

void create_instruction(FILE* file, t_list* list_instruction);
void parser_file(char* path, t_list* list_instruction);
void listen_cpu(int fd_cpu);
t_process* seek_process_by_pid(int pidBuscado);

/**
 * @brief Busca la lista de instruccion y devuelve la instruccion buscada
 * @param pid Program counter requerido.
 * @param pc Program counter requerido.
 */
void seek_instruccion(int socketRecibido);



/**
 * @brief Crea la lista de instrucciones asociada al archivo pasado por parametro
 * @param file Archivo a leer
 * @param list_instruction Lista a llenarse con las instrucciones del archivo.
 */
void create_instruction(FILE* file, t_list* list_instruction);


/**
 * @brief Busca un archivo, lo lee y crea una lista de instrucciones
 * @param path Path donde se encuentra el archivo.
 * @param list_instruction Lista a llenarse con las instrucciones del archivo.
 */
void parser_file(char* path, t_list* list_instruction);


/**
 * @brief Busca el proceso asociado al pid de interes.
 * @param pidBuscado Pid de la lista de instrucciones asociada
 */
t_process* seek_process_by_pid(int pidBuscado);



/**
 * @brief Funcion que encapsula al hilo escucha cpu
 * @param socket Socket escuchado
 */
void listen_cpu(int socket);


/**
 * @brief Funcion que encapsula al hilo escucha kernel
 * @param socket Socket escuchado
 */
void listen_kernel(int socket);

/**
 * @brief Crea los marcos e inicializa la lista de los mismos
 */
void create_marcos();

/**
 * @brief Libera el espacio reservado para los marcos
 */
void free_marcos();


/**
 * @brief Recibe el pedido de busqueda de marco y responde el mismo
 * @param socketRecibido Socket escuchado
 */
void respond_frame_request(int socketRecibido);

/**
 * @brief Busca el marco asociado a una pagina en especial de una tabla de paginas.
 * @param tablaPaginas Tanla de paginas del proceso donde buscar la pagina.
 * @param pagina Pagina buscada.
 */
int seek_marco_with_page_on_TDP (t_list* tablaPaginas, int pagina);

#endif /* MEMORIA_H */