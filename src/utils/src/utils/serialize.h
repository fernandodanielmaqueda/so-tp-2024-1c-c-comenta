
/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "estructuras.h"

#ifndef DEBUG_SERIALIZATION
#define DEBUG_SERIALIZATION 0
#endif

#define DEBUGGING_SERIALIZATION if(DEBUG_SERIALIZATION)

typedef enum HeaderCode {
    DISCONNECTED = -1,
    //::OPERACIONES MODULOS::
    //Kernel - Memoria
    PROCESS_NEW,
    PROCESS_CREATED,
    PROCESS_FINALIZED,
    //Kernel - IO
    //Kernel - CPU
    TYPE_INTERRUPT_SIN_INT,
    TYPE_INTERRUPT_FIN_PROCESO,
    TYPE_INTERRUPT_FIN_QUANTUM,
    PCB,
    ///////////////
    //CPU - Memoria////
    INSTUCTION_REQUEST,
    READ_REQUEST, //utilizado en MEMORIA-IO
    WRITE_REQUEST, //utilizado en MEMORIA-IO
    RESIZE_REQUEST,
    FRAME_ACCESS,    //PARA EMMORIA Y REVISAR LA TLB
    FRAME_REQUEST,
    PAGE_SIZE_REQUEST,
    ///////////////////
    //IO - Memoria
    //Instrucciones
    SET,
    MOV_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
} HeaderCode;

typedef struct Buffer {
    uint32_t size; // Tamaño del payload
    // uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} Buffer;

typedef struct Package {
    uint8_t header;
    Buffer* buffer;
} Package;

typedef struct t_instruction_use {
	enum HeaderCode operation;
	t_list* parameters;
} t_instruction_use;

/*
int start_server_module(char* module, char * pathconfig);
int start_client_module(char* module, char* pathconfig);
void get_ip_port_from_module(const char* module, char* path_config,char* ip, char* port);
*/

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param socket Package donde se creara el buffer
 */
enum HeaderCode receive_headerCode(int fd_socket);


/**
 * @brief Crear paquete.
 * @param headerCode Codigo de operacion que tendra el paquete.
 */
Package *package_create(uint8_t headerCode);


/**
 * @brief Agregar dato a paquete
 * @param package Package a rellenar.
 * @param value Dato a agregar
 * @param size Tamaño del dato a agregar.
 */
void package_add(Package *package, void *value, uint32_t size);


/**
 * @brief Eliminar paquete
 * @param paquete Package a eliminar.
 */
void package_destroy(Package *package);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param size Tamaño del buffer.
 * @param fd_client Socket desde donde se va a recibir el paquete.
 */
void *buffer_receive(int *size, int fd_client);


/**
 * @brief Crear Buffer
 * @param package Package donde se creara el buffer
 */
void package_buffer_create(Package *package);


Buffer *buffer_create(uint32_t size);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param fd_client Socket desde donde se va a recibir el paquete.
 */
t_list* get_package_like_list(int fd_client);

/**
 * @brief Eliminar pcb.
 * @param pcb_target t_pcb a eliminar.
 */
//void kill_pcb(t_pcb *pcb_target);


/**
 * @brief Eliminar instruccion.
 * @param lineInstruccion t_instruccion a eliminar.
 */
//void instruction_delete(t_instruccion *lineInstruccion);


/**
 * @brief Serializacion del t_pcb para ser enviada.
 * @param package Package a rellenar.
 * @param pcb Pcb a serializar
 */
void serialize_pcb(Package *package, t_pcb *pcb);


/**
 * @brief Deserializacion del t_pcb para ser enviada.
 * @param socketClient Socket desde donde se va a recibir el pcb.
 */
t_pcb* deserialize_pcb(int socketClient);


/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcbEnviado t_pcb a enviar.
 * @param socket Socket desde donde se va a recibir el pcb.
 */
void send_pcb_to(t_pcb* pcbEnviado, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param codigoOperacion Codigo de operacion a enviar
 * @param mensaje Mensaje a enviar
 * @param socket Socket destino
 */
void message_send(enum HeaderCode codigoOperacion, char* mensaje, int socket);


/**
 * @brief Recibe una cadena de caracteres (char*)
 * @param socket Socket recibido
 */
char* message_receive(int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a enviar
 * @param socket Socket destino
 */
void package_send(Package* package, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a serializar
 * @param bytes Tamanio del paquete
 */
void* package_serialize(Package* package, int bytes);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param instruccion Instruccion a enviar
 * @param socket Socket destino
 */
void send_instruccion(t_instruction_use* instruccion, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param socket Socket a recibir
 */
t_instruction_use* receive_instruccion(int socket);


/**
 * @brief Libera la memoria reservada para una instruccion determinada.
 * @param lineaInstruccion Instruccion a liberar
 */
void instruction_delete(t_instruction_use *lineaInstruccion);

void serialize_pcb_2(Package* paquete, t_pcb* pcb);
void free_package(Package* paquete);
void send_pcb(int socket, t_pcb* pcb);
void deserialize_pcb_2(t_pcb* pcb, void* stream);
void receive_pcb(int socket, t_pcb *pcb);


#endif // SERIALIZE_H