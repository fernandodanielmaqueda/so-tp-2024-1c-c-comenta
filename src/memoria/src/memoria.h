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
#include "utils/module.h"
#include "utils/serialize/cpu_instruction.h"
#include "utils/serialize/cpu_memory_request.h"
#include "utils/serialize/pcb.h"
#include "utils/socket.h"
#include "socket.h"

typedef struct t_Process {
    char* name;
    int PID;
    int number_of_instructions;
    t_list* instructions_list;
    t_list* pages_table;
} t_Process;

typedef struct t_Page {
    int pagid;
    bool bit_uso;
    bool bit_modificado;
    bool bit_presencia;
    int assigned_frame;
    time_t last_use;
} t_Page;

typedef struct t_Frame {
    int PID;
    int id;
    t_Page* assigned_page;
} t_Frame;

int module(int, char*[]);
void read_module_config(t_config *module_config);

void listen_kernel(int fd_kernel);

/**
 * @brief Busca el archivo de pseudocodigo y crea la estructura dentro de memoria
 * @param socketRecibido Socket desde donde se va a recibir el pcb.
 */
void create_process(t_Payload* socketRecibido);

/**
 * @brief Elimina el proceso, marca el marco como disponible y libera la pagina
 * @param socketRecibido Socket desde donde se va a recibir el pcb.
 */
void kill_process (t_Payload* socketRecibido);


void create_instruction(FILE* file, t_list* list_instruction);
void parser_file(char* path, t_list* list_instruction);
void listen_cpu(int fd_cpu);
void listen_io(int fd_io);
t_Process* seek_process_by_pid(int pidBuscado);

/**
 * @brief Busca la lista de instruccion y devuelve la instruccion buscada
 * @param pid Program counter requerido.
 * @param pc Program counter requerido.
 */
void seek_instruccion(t_Payload* socketRecibido);



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
 * @brief Busca el proceso asociado al PID de interes.
 * @param pidBuscado Pid de la lista de instrucciones asociada
 */
t_Process* seek_process_by_pid(int pidBuscado);



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
void respond_frame_request(t_Payload* socketRecibido);

/**
 * @brief Busca el marco asociado a una pagina en especial de una tabla de paginas.
 * @param tablaPaginas Tanla de paginas del proceso donde buscar la pagina.
 * @param pagina Pagina buscada.
 */
int seek_marco_with_page_on_TDP (t_list* tablaPaginas, int pagina);

void resize_process(t_Payload* payload);
void write_memory(t_Payload* socketRecibido, int socket);
void read_memory(t_Payload* socketRecibido, int socket);
void update_page(int current_frame);
int get_next_dir_fis(int current_frame, int pid);
int seek_oldest_page_updated(t_list* page_list);

#endif /* MEMORIA_H */