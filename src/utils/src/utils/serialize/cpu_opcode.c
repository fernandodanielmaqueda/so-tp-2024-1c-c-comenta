/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_opcode.h"

void cpu_opcode_serialize(t_Payload *payload, e_CPU_OpCode *cpu_opcode) {
  payload_enqueue(payload, cpu_opcode, sizeof(t_EnumValue));

  cpu_opcode_log(cpu_opcode);
}

e_CPU_OpCode *cpu_opcode_deserialize(t_Payload *payload) {
  e_CPU_OpCode *cpu_opcode = malloc(sizeof(e_CPU_OpCode));

  payload_dequeue(payload, cpu_opcode, sizeof(t_EnumValue));

  cpu_opcode_log(cpu_opcode);
  return cpu_opcode;
}

void cpu_opcode_free(e_CPU_OpCode* cpu_opcode){
  free(cpu_opcode);
}

void cpu_opcode_log(e_CPU_OpCode *cpu_opcode) {
  log_info(SERIALIZE_LOGGER,
    "e_CPU_OpCode[%p]: %d"
    , (void *) cpu_opcode
    , *cpu_opcode
  );
}