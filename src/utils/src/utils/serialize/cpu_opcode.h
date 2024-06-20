/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_CPU_INSTRUCTION_H
#define UTILS_SERIALIZE_CPU_INSTRUCTION_H

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

typedef uint32_t t_Value;

typedef enum e_CPU_OpCode {
    SET_CPU_OPCODE,
    MOV_IN_CPU_OPCODE,
    MOV_OUT_CPU_OPCODE,
    SUM_CPU_OPCODE,
    SUB_CPU_OPCODE,
    JNZ_CPU_OPCODE,
    RESIZE_CPU_OPCODE,
    COPY_STRING_CPU_OPCODE,
    WAIT_CPU_OPCODE,
    SIGNAL_CPU_OPCODE,
    IO_GEN_SLEEP_CPU_OPCODE,
    EXIT_CPU_OPCODE,
    IO_STDIN_READ_CPU_OPCODE,
    IO_STDOUT_WRITE_CPU_OPCODE,
    IO_FS_CREATE_CPU_OPCODE,
    IO_FS_DELETE_CPU_OPCODE,
    IO_FS_TRUNCATE_CPU_OPCODE,
    IO_FS_WRITE_CPU_OPCODE,
    IO_FS_READ_CPU_OPCODE
} e_CPU_OpCode;


/**
 * @brief Serializacion del e_CPU_OpCode para ser enviada.
 * @param package Package a rellenar.
 * @param cpu_opcode instruction a serializar
 */
void cpu_opcode_serialize(t_Payload *payload, e_CPU_OpCode *cpu_opcode);


/**
 * @brief Deserializacion del e_CPU_OpCode para ser enviada.
 * @param Payload Payload.
 */
e_CPU_OpCode *cpu_opcode_deserialize(t_Payload *payload);


void cpu_opcode_free(e_CPU_OpCode *cpu_opcode);
void cpu_opcode_log(e_CPU_OpCode *cpu_opcode);

#endif // UTILS_SERIALIZE_CPU_INSTRUCTION_H