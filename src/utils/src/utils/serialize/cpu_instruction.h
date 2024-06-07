/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_CPU_INSTRUCTION_H
#define SERIALIZE_CPU_INSTRUCTION_H

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

typedef enum e_CPU_Opcode {
    SET_OPCODE,
    MOV_IN_OPCODE,
    MOV_OUT_OPCODE,
    SUM_OPCODE,
    SUB_OPCODE,
    JNZ_OPCODE,
    RESIZE_OPCODE,
    COPY_STRING_OPCODE,
    WAIT_OPCODE,
    SIGNAL_OPCODE,
    IO_GEN_SLEEP_OPCODE,
    EXIT_OPCODE,
    IO_STDIN_READ_OPCODE,
    IO_STDOUT_WRITE_OPCODE,
    IO_FS_CREATE_OPCODE,
    IO_FS_DELETE_OPCODE,
    IO_FS_TRUNCATE_OPCODE,
    IO_FS_WRITE_OPCODE,
    IO_FS_READ_OPCODE
} e_CPU_Opcode;

typedef struct t_CPU_Instruction {
	enum e_CPU_Opcode opcode;
	t_list *parameters;
} t_CPU_Instruction;


/**
 * @brief Enviar instruction (incluye el serializado)
 * @param instruction t_CPU_Instruction a enviar.
 * @param fd_socket Socket desde donde se va a recibir el instruction.
 */
void cpu_instruction_send(t_CPU_Instruction *instruction, int fd_socket);


/**
 * @brief Serializacion del t_CPU_Instruction para ser enviada.
 * @param package Package a rellenar.
 * @param instruction instruction a serializar
 */
void cpu_instruction_serialize(t_Payload *payload, t_CPU_Instruction *instruction);


/**
 * @brief Deserializacion del t_CPU_Instruction para ser enviada.
 * @param Payload Payload.
 */
t_CPU_Instruction *cpu_instruction_deserialize(t_Payload *payload);


void cpu_instruction_print(t_CPU_Instruction *instruction);

#endif // SERIALIZE_CPU_INSTRUCTION_H