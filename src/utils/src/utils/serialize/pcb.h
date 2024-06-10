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
    uint32_t quantum; //remanente del quantum
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
void pcb_serialize(t_Payload *payload, t_PCB *pcb);


/**
 * @brief Deserializacion del t_PCB para ser enviada.
 * @param Payload Payload.
 */
t_PCB *pcb_deserialize(t_Payload *payload);


void pcb_print(t_PCB *pcb);

#endif // SERIALIZE_PCB_H