/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/interrupt.h"

void interrupt_send(e_Interrupt *interrupt, int fd_socket) {
  t_Package *package = package_create_with_header(INTERRUPT_HEADER);
  interrupt_serialize(package->payload, interrupt);
  package_send(package, fd_socket);
  package_destroy(package);
}

void interrupt_serialize(t_Payload *payload, e_Interrupt *interrupt) {
  uint8_t conversion = (uint8_t) *interrupt;
  
  payload_enqueue(payload, &(conversion), sizeof(conversion));
}

e_Interrupt *interrupt_deserialize(t_Payload *payload) {
  e_Interrupt *interrupt = malloc(sizeof(e_Interrupt));

  uint32_t offset = 0;

  uint8_t conversion;

  offset = memcpy_source_offset(&(conversion), payload->stream, offset, sizeof(uint8_t));
  *interrupt = (e_Interrupt) conversion;

  return interrupt;
}

void interrupt_print(e_Interrupt *interrupt) {
  /* 
  log_info(SERIALIZE_LOGGER,
    "t_Interrupt[%p]:\n"
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
    ,(void *) interrupt,
    interrupt->PID,
    interrupt->PC,
    interrupt->AX,
    interrupt->BX,
    interrupt->CX,
    interrupt->DX,
    interrupt->EAX,
    interrupt->EBX,
    interrupt->ECX,
    interrupt->EDX,
    interrupt->RAX,
    interrupt->RBX,
    interrupt->RCX,
    interrupt->RDX,
    interrupt->SI,
    interrupt->DI,
    interrupt->quantum,
    interrupt->current_state,
    interrupt->arrival_READY,
    interrupt->arrival_RUNNING);

    */
}