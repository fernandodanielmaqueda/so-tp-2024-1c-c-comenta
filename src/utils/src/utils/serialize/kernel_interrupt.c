/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/kernel_interrupt.h"

void kernel_interrupt_serialize(t_Payload *payload, e_Kernel_Interrupt source) {
  if(payload == NULL)
    return;

  t_EnumValue aux;
  
    aux = (t_EnumValue) source;
  payload_append(payload, &aux, sizeof(aux));

  kernel_interrupt_log(source);
}

void kernel_interrupt_deserialize(t_Payload *payload, e_Kernel_Interrupt *destination) {
  if(payload == NULL || destination == NULL)
    return;

  t_EnumValue aux;

  payload_shift(payload, &aux, sizeof(aux));
    *destination = (e_Kernel_Interrupt) aux;

  kernel_interrupt_log(*destination);
}

void kernel_interrupt_log(e_Kernel_Interrupt kernel_interrupt) {
  log_info(SERIALIZE_LOGGER,
    "e_Kernel_Interrupt: %d"
    , kernel_interrupt
  );
}