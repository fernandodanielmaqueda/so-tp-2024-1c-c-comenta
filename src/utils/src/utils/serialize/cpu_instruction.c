/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_instruction.h"

void cpu_instruction_send(t_CPU_Instruction *instruction, int fd_socket) {
  t_Package *package = package_create_with_header(CPU_INSTRUCTION_HEADER);
  cpu_instruction_serialize(package->payload, instruction);
  package_send(package, fd_socket);
  package_destroy(package);
}

void cpu_instruction_serialize(t_Payload *payload, t_CPU_Instruction *instruction) {
    /*
    payload_add(payload, &(instruction->opcode), sizeof(instruction->opcode));
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    payload_add(payload, &(), sizeof());
    */
}

t_CPU_Instruction *cpu_instruction_deserialize(t_Payload *payload) {
  t_CPU_Instruction *instruction = malloc(sizeof(t_CPU_Instruction));

  uint32_t offset = 0;

    /*
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  offset = memcpy_source_offset(&(), payload->stream, offset, sizeof());
  
  */

  cpu_instruction_print(instruction);
  return instruction;
}

void cpu_instruction_print(t_CPU_Instruction *instruction) {
    /*
  log_info(SERIALIZE_LOGGER,
    "t_CPU_Instruction[%p]:\n"
    "* PID: %" PRIu32 "\n"
    "* PC: %" PRIu32 "\n"
    "* AX: %" PRIu8 "\n"
    "* BX: %" PRIu8 "\n"
    "* CX: %" PRIu8 "\n"
    "* DX: %" PRIu8 "\n"
    "* EAX: %" PRIu32 "\n"
    "* EBX: %" PRIu32 "\n"
    "* ECX: %" PRIu32 "\n"
    "* EDX: %" PRIu32 "\n"
    "* RAX: %" PRIu32 "\n"
    "* RBX: %" PRIu32 "\n"
    "* RCX: %" PRIu32 "\n"
    "* RDX: %" PRIu32 "\n"
    "* SI: %" PRIu32 "\n"
    "* DI: %" PRIu32 "\n"
    "* quantum: %" PRIu32 "\n"
    "* current_state: %" PRIu8 "\n"
    "* arrival_READY: %g\n"
    "* arrival_RUNNING: %g"
    ,(void *) instruction,
    instruction->PID,
    instruction->PC,
    instruction->AX,
    instruction->BX,
    instruction->CX,
    instruction->DX,
    instruction->EAX,
    instruction->EBX,
    instruction->ECX,
    instruction->EDX,
    instruction->RAX,
    instruction->RBX,
    instruction->RCX,
    instruction->RDX,
    instruction->SI,
    instruction->DI,
    instruction->quantum,
    instruction->current_state,
 
    */
}