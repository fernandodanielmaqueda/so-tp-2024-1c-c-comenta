/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_PCB_H
#define UTILS_SERIALIZE_PCB_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "utils/package.h"

#include "commons/log.h"
#include "utils/module.h"

typedef struct t_CPU_Registers {
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
} t_CPU_Registers;

typedef uint32_t t_PID;
typedef uint32_t t_PC;

typedef struct t_PCB {
    t_PID PID;
    t_PC PC;
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
    uint64_t quantum; //remanente del quantum
    uint8_t current_state; //enum Process_State current_state;
    double arrival_READY; // che llego en t=5s
    double arrival_RUNNING; // che corrio en runnign t=20seg 
} t_PCB;

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


void pcb_free(t_PCB *pcb);
void pcb_log(t_PCB *pcb);

#endif // UTILS_SERIALIZE_PCB_H