/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_memory_request.h"

void cpu_memory_request_serialize(t_Payload *payload, e_CPU_Memory_Request *memory_request) {
  payload_enqueue(payload, memory_request, sizeof(uint8_t));

  cpu_memory_request_log(memory_request);
}

e_CPU_Memory_Request *cpu_memory_request_deserialize(t_Payload *payload) {
  e_CPU_Memory_Request *memory_request = malloc(sizeof(e_CPU_Memory_Request));

  payload_dequeue(payload, memory_request, sizeof(uint8_t));

  cpu_memory_request_log(memory_request);
  return memory_request;
}

void cpu_memory_request_free(e_CPU_Memory_Request *memory_request) {
  free(memory_request);
}

void cpu_memory_request_log(e_CPU_Memory_Request *memory_request) {
  log_info(SERIALIZE_LOGGER,
    "e_CPU_Memory_Request[%p]: %d",
    (void *) memory_request,
    *memory_request
  );
}