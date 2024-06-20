/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/kernel_interrupt.h"

void kernel_interrupt_serialize(t_Payload *payload, e_Kernel_Interrupt source) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, &source, sizeof(t_EnumValue));

  kernel_interrupt_log(source);
}

void kernel_interrupt_deserialize(t_Payload *payload, e_Kernel_Interrupt *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_dequeue(payload, destination, sizeof(t_EnumValue));

  kernel_interrupt_log(*destination);
}

void kernel_interrupt_log(e_Kernel_Interrupt kernel_interrupt) {
  log_info(SERIALIZE_LOGGER,
    "e_Kernel_Interrupt: %d"
    , kernel_interrupt
  );
}