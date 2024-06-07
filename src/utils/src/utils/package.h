/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef PACKAGE_H
#define PACKAGE_H

#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "utils/payload.h"

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

#endif // PACKAGE_H