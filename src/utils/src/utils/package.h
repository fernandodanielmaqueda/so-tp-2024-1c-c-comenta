/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef PACKAGE_H
#define PACKAGE_H

#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "commons/config.h"
#include "commons/log.h"
#include "utils/payload.h"
#include "utils/module.h"

typedef enum e_Header {
    DISCONNECTION_HEADER = -1,
    PCB_HEADER,
    CPU_MEMORY_REQUEST_HEADER,
    CPU_INSTRUCTION_HEADER,
    INTERRUPT_HEADER,
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
} e_Header;

typedef uint8_t t_Header_Serialized;

typedef struct t_Package {
    enum e_Header header;
    t_Payload *payload;
} t_Package;

/**
 * @brief Crear paquete.
 */
t_Package *package_create(void);

/**
 * @brief Crear paquete.
 * @param header Codigo de operacion que tendra el paquete.
 */
t_Package *package_create_with_header(e_Header header);

/**
 * @brief Eliminar paquete
 * @param package t_Package a eliminar.
 */
void package_destroy(t_Package *package);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a enviar
 * @param fd_socket Socket destino
 */
void package_send(t_Package *package, int fd_socket);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete a serializar
 * @param bufferSize Tamanio del paquete
 */
void *package_serialize(t_Package *package, size_t bufferSize);

t_Package *package_receive(int fd_socket);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete
 * @param fd_socket Paquete donde se creara el payload
 */
void package_receive_header(t_Package *package, int fd_socket);

void package_receive_payload(t_Package *package, int fd_socket);

#endif // PACKAGE_H