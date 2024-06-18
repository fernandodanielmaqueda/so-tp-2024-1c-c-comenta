/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/kernel_interrupt.h"

void kernel_interrupt_serialize(t_Payload *payload, e_Kernel_Interrupt *kernel_interrupt) {
  payload_enqueue(payload, kernel_interrupt, sizeof(uint8_t));

  kernel_interrupt_log(kernel_interrupt);
}

e_Kernel_Interrupt *kernel_interrupt_deserialize(t_Payload *payload) {
  e_Kernel_Interrupt *kernel_interrupt = malloc(sizeof(e_Kernel_Interrupt));

  payload_dequeue(payload, kernel_interrupt, sizeof(uint8_t));

  kernel_interrupt_log(kernel_interrupt);
  return kernel_interrupt;
}

void kernel_interrupt_free(e_Kernel_Interrupt *kernel_interrupt) {
  free(kernel_interrupt);
}

void kernel_interrupt_log(e_Kernel_Interrupt *kernel_interrupt) {
  log_info(SERIALIZE_LOGGER,
    "e_Kernel_Interrupt[%p]: %d"
    , (void *) kernel_interrupt
    , *kernel_interrupt
  );
}