/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/arguments.h"

void arguments_send(t_Arguments *arguments, int fd_socket) {
  t_Package *package = package_create_with_header(ARGUMENTS_HEADER);
  arguments_serialize(package->payload, arguments);
  package_send(package, fd_socket);
  package_destroy(package);
}

void arguments_serialize(t_Payload *payload, t_Arguments *arguments) {
  payload_enqueue(payload, &(arguments->argc), sizeof(arguments->argc));
}

t_Arguments *arguments_deserialize(t_Payload *payload) {
  t_Arguments *arguments = malloc(sizeof(t_Arguments));

  uint32_t offset = 0;

  offset = memcpy_source_offset(&(arguments->argc), payload->stream, offset, sizeof(arguments->argc));

  arguments_print(arguments);
  return arguments;
}

void arguments_print(t_Arguments *arguments) {
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
    pcb->arrival_RUNNING
    );
  */
}