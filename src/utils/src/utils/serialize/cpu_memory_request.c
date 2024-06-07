/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_memory_request.h"

void cpu_memory_request_send(enum t_CPU_Memory_Request *memory_request, int fd_socket) {
  Package *package = package_create_with_header(CPU_MEMORY_REQUEST_HEADERCODE);
  cpu_memory_request_serialize(package->payload, memory_request);
  package_send(package, fd_socket);
  package_destroy(package);
}

void cpu_memory_request_serialize(Payload *payload, enum t_CPU_Memory_Request *memory_request) {
  uint8_t conversion_uint8_t = (uint8_t) *memory_request;
  
  payload_add(payload, &(conversion_uint8_t), sizeof(conversion_uint8_t));
}

enum t_CPU_Memory_Request *cpu_memory_request_deserialize(Payload *payload) {

  enum t_CPU_Memory_Request *memory_request = malloc(sizeof(enum t_CPU_Memory_Request));
  uint32_t offset = 0;

  uint8_t conversion_uint8_t;

  offset = memcpy_source_offset(&(conversion_uint8_t), payload->stream, offset, sizeof(uint8_t));
  *memory_request = (enum t_CPU_Memory_Request) conversion_uint8_t;

  return memory_request;
}

void cpu_memory_request_print(enum t_CPU_Memory_Request *memory_request) {
  log_info(SERIALIZE_LOGGER,
    "t_CPU_Memory_Request[%p]: %d",
    (void *) memory_request,
    *memory_request
  );
}