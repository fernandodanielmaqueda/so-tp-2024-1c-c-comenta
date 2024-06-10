/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_memory_request.h"

void cpu_memory_request_send(e_CPU_Memory_Request *memory_request, int fd_socket) {
  t_Package *package = package_create_with_header(CPU_MEMORY_REQUEST_HEADER);
  cpu_memory_request_serialize(package->payload, memory_request);
  package_send(package, fd_socket);
  package_destroy(package);
}

void cpu_memory_request_serialize(t_Payload *payload, e_CPU_Memory_Request *memory_request) {
  uint8_t conversion_uint8_t;
  
  conversion_uint8_t = (uint8_t) *memory_request;
  payload_enqueue(payload, &(conversion_uint8_t), sizeof(conversion_uint8_t));
}

e_CPU_Memory_Request *cpu_memory_request_deserialize(t_Payload *payload) {

  e_CPU_Memory_Request *memory_request = malloc(sizeof(e_CPU_Memory_Request));
  uint32_t offset = 0;

  uint8_t conversion_uint8_t;

  offset = memcpy_source_offset(&(conversion_uint8_t), payload->stream, offset, sizeof(uint8_t));
  *memory_request = (e_CPU_Memory_Request) conversion_uint8_t;

  return memory_request;
}

void cpu_memory_request_print(e_CPU_Memory_Request *memory_request) {
  log_info(SERIALIZE_LOGGER,
    "e_CPU_Memory_Request[%p]: %d",
    (void *) memory_request,
    *memory_request
  );
}


void send_string(char* mensaje, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, mensaje, strlen(mensaje)+1 );
  package_send(package, socket);
  package_destroy(package);
}


void send_int(int nro, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, nro, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}

void send_2int(int nro, int nro2, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, nro, sizeof(int) );
    payload_enqueue(package->payload, nro2, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}

void send_String_1int(int nro, char* mensaje, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, strlen(mensaje)+1, sizeof(int) );
    payload_enqueue(package->payload, mensaje, strlen(mensaje)+1 );
    payload_enqueue(package->payload, nro, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}
