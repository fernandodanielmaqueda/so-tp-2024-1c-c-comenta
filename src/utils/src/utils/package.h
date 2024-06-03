/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef PACKAGE_H
#define PACKAGE_H

//#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"

typedef enum HeaderCode {
    DISCONNECTION_HEADERCODE = -1,
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
    PCB_HEADERCODE,
    ///////////////
    //CPU - Memoria////
    INSTUCTION_REQUEST,
    READ_REQUEST, //utilizado en MEMORIA-IO
    WRITE_REQUEST, //utilizado en MEMORIA-IO
    RESIZE_REQUEST,
    ///////////////////
    //IO - Memoria
    //Instrucciones
    SET,
    MOVE_IN,
    MOVE_OUT,
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

typedef uint32_t Size;

typedef struct Payload {
    Size size; // Tamaño del payload
    // uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} Payload;

typedef uint8_t Header;

typedef struct Package {
    Header header;
    Payload *payload;
} Package;


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
 * @param socket Package donde se creara el payload
 */
enum HeaderCode receive_headerCode(int fd_socket);


/**
 * @brief Crear paquete.
 */
Package *package_create(void);

/**
 * @brief Crear paquete.
 * @param headerCode Codigo de operacion que tendra el paquete.
 */
Package *package_create_with_headerCode(uint8_t headerCode);


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

Payload *payload_create(void);
void payload_destroy(Payload *payload);
void payload_add(Payload *payload, void *data, uint32_t dataSize);
void payload_read(Payload *payload, void *data, uint32_t size);
void payload_add_uint32(Payload *payload, uint32_t data);
uint32_t payload_read_uint32(Payload *payload);
void payload_add_uint8(Payload *payload, uint8_t data);
uint8_t payload_read_uint8(Payload *payload);
void payload_add_string(Payload *payload, uint32_t length, char *string);
char *payload_read_string(Payload *payload, uint32_t *length);

/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param fd_client Socket desde donde se va a recibir el paquete.
 */
t_list* get_package_like_list(int fd_client);

void free_string_element(void* element);

#endif // PACKAGE_H