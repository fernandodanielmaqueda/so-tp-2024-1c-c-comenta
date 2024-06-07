/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_memory_request.h"

void cpu_memory_request_send(enum t_CPU_Memory_Request *instruction, int fd_socket) {
  Package *package = package_create_with_header(CPU_MEMORY_REQUEST_HEADERCODE);
  cpu_memory_request_serialize(package->payload, instruction);
  package_send(package, fd_socket);
  package_destroy(package);
}

void cpu_memory_request_serialize(Payload *payload, enum t_CPU_Memory_Request *instruction) {
  uint8_t conversion = (uint8_t) *instruction;
  
  payload_add(payload, &(conversion), sizeof(conversion));
}

enum t_CPU_Memory_Request *cpu_memory_request_deserialize(Payload *payload) {
  enum t_CPU_Memory_Request *instruction = malloc(sizeof(enum t_CPU_Memory_Request));

  uint32_t offset = 0;

  uint8_t conversion;

  offset = memcpy_source_offset(&(conversion), payload->stream, offset, sizeof(uint8_t));
  *instruction = (enum t_CPU_Memory_Request) conversion;

  return instruction;
}

void cpu_memory_request_print(enum t_CPU_Memory_Request *instruction) {
  /*
  log_info(SERIALIZE_LOGGER,
    "t_PCB[%p]:\n"
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
    ,(void *) pcb,
    pcb->PID,
    pcb->PC,
    pcb->AX,
    pcb->BX,
    pcb->CX,
    pcb->DX,
    pcb->EAX,
    pcb->EBX,
    pcb->ECX,
    pcb->EDX,
    pcb->RAX,
    pcb->RBX,
    pcb->RCX,
    pcb->RDX,
    pcb->SI,
    pcb->DI,
    pcb->quantum,
    pcb->current_state,
    pcb->arrival_READY,
    pcb->arrival_RUNNING);
    */
}