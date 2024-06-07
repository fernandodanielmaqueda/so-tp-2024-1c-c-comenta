/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_PCB_H
#define SERIALIZE_PCB_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "estructuras.h"
#include "utils/package.h"

#include "commons/log.h"
#include "utils/module.h"

#ifndef DEBUG_SERIALIZATION
#define DEBUG_SERIALIZATION 0
#endif

#define DEBUGGING_SERIALIZATION if(DEBUG_SERIALIZATION)

typedef struct t_PCB {
    uint32_t PID;
    uint32_t PC;
    //registers_1[t_register];
    uint8_t AX;
    uint8_t BX;
    uint8_t CX; 
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t RAX;
    uint32_t RBX;
    uint32_t RCX;
    uint32_t RDX;
    uint32_t SI;
    uint32_t DI;
    uint32_t quantum;
    uint8_t current_state; //enum Process_State current_state;
    double arrival_READY;
    double arrival_RUNNING;
} t_PCB;


/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcb t_PCB a enviar.
 * @param fd_socket Socket desde donde se va a recibir el pcb.
 */
void pcb_send(t_PCB *pcb, int fd_socket);


/**
 * @brief Serializacion del t_PCB para ser enviada.
 * @param package Package a rellenar.
 * @param pcb Pcb a serializar
 */
void pcb_serialize(Payload *payload, t_PCB *pcb);


/**
 * @brief Deserializacion del t_PCB para ser enviada.
 * @param Payload Payload.
 */
t_PCB *pcb_deserialize(Payload *payload);


void pcb_print(t_PCB *pcb);

typedef struct t_instruction_use {
	enum HeaderCode operation;
	t_list* parameters;
} t_instruction_use;


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param instruccion Instruccion a enviar
 * @param socket Socket destino
 */
void instruction_send(t_instruction_use* instruccion, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param socket Socket a recibir
 */
t_instruction_use* instruction_receive(int socket);


/**
 * @brief Libera la memoria reservada para una instruccion determinada.
 * @param lineaInstruccion Instruccion a liberar
 */
void instruction_delete(t_instruction_use *lineaInstruccion);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param codigoOperacion Codigo de operacion a enviar
 * @param mensaje Mensaje a enviar
 * @param socket Socket destino
 */
//void message_send(enum HeaderCode codigoOperacion, char* mensaje, int socket);


/**
 * @brief Recibe una cadena de caracteres (char*)
 * @param socket Socket recibido
 */
//char* message_receive(int socket);


/**
 * @brief Recibe un paquete desde un socket, y transforma el contenido en una lista.
 * @param fd_client Socket desde donde se va a recibir el paquete.
 */
// t_list* get_package_like_list(int fd_client);

void free_string_element(void* element);

#endif // SERIALIZE_PCB_H