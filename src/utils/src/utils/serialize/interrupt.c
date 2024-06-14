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
  payload_enqueue(payload, interrupt, sizeof(uint8_t));

  interrupt_log(interrupt);
}

e_Interrupt *interrupt_deserialize(t_Payload *payload) {
  e_Interrupt *interrupt = malloc(sizeof(e_Interrupt));

  payload_dequeue(payload, interrupt, sizeof(uint8_t));

  interrupt_log(interrupt);
  return interrupt;
}

void interrupt_log(e_Interrupt *interrupt) {
  log_info(SERIALIZE_LOGGER,
    "e_Interrupt[%p]: %d"
    , (void *) interrupt
    , *interrupt
  );
}