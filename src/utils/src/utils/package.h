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

typedef enum Header {
    DISCONNECTION_HEADERCODE = -1,
    PCB_HEADERCODE,
    CPU_MEMORY_REQUEST_HEADERCODE,
    CPU_INSTRUCTION_HEADERCODE,
    //::OPERACIONES MODULOS::
    //Kernel - Memoria
    PROCESS_NEW,
    PROCESS_CREATED,
    PROCESS_FINALIZED,
    //Kernel - IO
    //Kernel - CPU
    ///////////////////
    //IO - Memoria
    //Instrucciones
} Header;

typedef uint8_t Header_Serialized;

typedef struct Package {
    enum Header header;
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