/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_opcode.h"

void cpu_opcode_serialize(t_Payload *payload, e_CPU_OpCode source) {
  if(payload == NULL)
    return;

  t_EnumValue aux;
  
    aux = (t_EnumValue) source;
  payload_append(payload, &aux, sizeof(aux));

  cpu_opcode_log(source);
}

void cpu_opcode_deserialize(t_Payload *payload, e_CPU_OpCode *destination) {
  if(payload == NULL || destination == NULL)
    return;

  t_EnumValue aux;
  
  payload_shift(payload, &aux, sizeof(aux));
    *destination = (e_CPU_OpCode) aux;

  cpu_opcode_log(*destination);
}

void cpu_opcode_log(e_CPU_OpCode cpu_opcode) {
  log_info(SERIALIZE_LOGGER,
    "e_CPU_OpCode: %d"
    , cpu_opcode
  );
}