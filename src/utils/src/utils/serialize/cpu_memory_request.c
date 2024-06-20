/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_memory_request.h"

void cpu_memory_request_serialize(t_Payload *payload, e_CPU_Memory_Request source) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, &source, sizeof(t_EnumValue));

  cpu_memory_request_log(source);
}

void cpu_memory_request_deserialize(t_Payload *payload, e_CPU_Memory_Request *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_dequeue(payload, destination, sizeof(t_EnumValue));

  cpu_memory_request_log(*destination);
}

void cpu_memory_request_log(e_CPU_Memory_Request memory_request) {
  log_info(SERIALIZE_LOGGER,
    "e_CPU_Memory_Request: %d",
    memory_request
  );
}