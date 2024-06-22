/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_PACKAGE_H
#define UTILS_PACKAGE_H

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
    // USO GENERAL
    DISCONNECTING_HEADER,
    SUBHEADER_HEADER,
    ARGUMENTS_HEADER,
    PCB_HEADER,
    TEXT_HEADER,
    // CPU - Memoria
    INSTRUCTION_REQUEST,
    READ_REQUEST, //utilizado en MEMORIA-IO
    WRITE_REQUEST, //utilizado en MEMORIA-IO
    RESIZE_REQUEST,
    OUT_OF_MEMORY,
    FRAME_ACCESS,    //PARA MEMORIA Y REVISAR LA TLB
    FRAME_REQUEST,
    PAGE_SIZE_REQUEST,
    // Kernel - CPU
    EVICTION_REASON_HEADER,
    KERNEL_INTERRUPT_HEADER,
    EXIT_STATUS_HEADER,
    //::OPERACIONES MODULOS::
    //Kernel - Memoria
    PROCESS_CREATE_HEADER,
    PROCESS_DESTROY_HEADER,
    //Kernel - IO
    //Kernel - CPU
    STDIN_REQUEST,
    STDOUT_REQUEST,
    IO_STDIN_WRITE_MEMORY,
    IO_STDOUT_READ_MEMORY
    ///////////////////
    //IO - Memoria
    //Instrucciones
} e_Header;

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

t_Package *package_receive(int fd_socket);

/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param package Paquete
 * @param fd_socket Paquete donde se creara el payload
 */
void package_receive_header(t_Package *package, int fd_socket);

void package_receive_payload(t_Package *package, int fd_socket);

#endif // UTILS_PACKAGE_H