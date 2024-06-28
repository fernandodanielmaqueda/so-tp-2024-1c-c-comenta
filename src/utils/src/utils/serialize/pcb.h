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

typedef enum e_Exit_Reason {
    SUCCESS_EXIT_REASON,
    INVALID_RESOURCE_EXIT_REASON,
    INVALID_INTERFACE_EXIT_REASON,
    OUT_OF_MEMORY_EXIT_REASON,
    INTERRUPTED_BY_USER_EXIT_REASON
} e_Exit_Reason;

typedef enum e_Process_State {
    NEW_STATE,
    READY_STATE,
    EXECUTING_STATE,
    BLOCKED_STATE, 
	EXIT_STATE
} e_Process_State;

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
typedef uint32_t t_MemorySize;
typedef uint64_t t_Quantum;

typedef struct t_PCB {
    t_PID PID;
    t_PC PC;
    t_Quantum quantum;
    t_CPU_Registers cpu_registers;
    e_Process_State current_state;
    e_Exit_Reason exit_reason;
} t_PCB;


/**
 * @brief Serializacion de un t_PCB para ser enviado.
 * @param payload Payload a encolar.
 * @param source t_PCB fuente a serializar
 */
void pcb_serialize(t_Payload *payload, t_PCB source);


/**
 * @brief Deserializacion de un t_PCB para ser leido.
 * @param payload Payload a desencolar.
 * @param destination Destino del t_PCB deserializado
 */
void pcb_deserialize(t_Payload *payload, t_PCB *destination);


/**
 * @brief Loguea un t_PCB.
 * @param pcb t_PCB a loguear.
 */
void pcb_log(t_PCB pcb);

#endif // UTILS_SERIALIZE_PCB_H