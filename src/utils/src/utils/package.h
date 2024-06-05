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
    FRAME_ACCESS,    //PARA EMMORIA Y REVISAR LA TLB
    FRAME_REQUEST,
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

typedef uint32_t PayloadSize;

typedef struct Payload {
    PayloadSize size; // Tamaño del payload
    // uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} Payload;

typedef uint8_t Header;

typedef struct Package {
    Header header;
    Payload *payload;
} Package;

/**
 * @brief Crear paquete.
 */
Package *package_create(void);

/**
 * @brief Crear paquete.
 * @param header Codigo de operacion que tendra el paquete.
 */
Package *package_create_with_header(Header header);

/**
 * @brief Eliminar paquete
 * @param package Package a eliminar.
 */
void package_destroy(Package *package);

/**
 * @brief Agregar dato a paquete
 * @param package Package a rellenar.
 * @param data Datos a agregar
 * @param dataSize Tamaño de los datos a agregar.
 */
void package_add(Package *package, void *data, size_t dataSize);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a enviar
 * @param fd_socket Socket destino
 */
void package_send(Package *package, int fd_socket);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a serializar
 * @param bufferSize Tamanio del paquete
 */
void *package_serialize(Package *package, size_t bufferSize);

Package *package_receive(int fd_socket);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete
 * @param fd_socket Paquete donde se creara el payload
 */
void package_receive_header(Package *package, int fd_socket);

void package_receive_payload(Package *package, int fd_socket);
void package_deserialize(Package *package);
Payload *payload_create(void);
void payload_destroy(Payload *payload);
void payload_add(Payload *payload, void *data, size_t dataSize);
size_t memcpy_offset(void *destination, size_t offset, void *source, size_t bytes);
void payload_read(Payload *payload, void *data, size_t dataSize);
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
// t_list* get_package_like_list(int fd_client);

void free_string_element(void* element);

#include "serialize.h"

#endif // PACKAGE_H