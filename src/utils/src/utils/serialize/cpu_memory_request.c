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

  offset = memcpy_deserialize(&(conversion_uint8_t), payload->stream, offset, sizeof(uint8_t));
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

void receive_2int(int* nro1, int* nro2, t_Payload* payload){ //receive_2int(&n1,&n2,payload);
    // Extrae el primer entero del payload
    int offset = 0;
    memcpy(nro1, payload->stream + offset, sizeof(int));
    offset += sizeof(int);

    // Extrae el segundo entero del payload
    memcpy(nro2, payload->stream + offset, sizeof(int));
    offset += sizeof(int);
}

void send_2int_1uint32(int nro, int nro2, u_int32_t contenido, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, nro, sizeof(int) );
    payload_enqueue(package->payload, nro2, sizeof(int) );
    payload_enqueue(package->payload, contenido, sizeof(u_int32_t) );
  package_send(package, socket);
  package_destroy(package);
}

void receive_2int_1uint32(int* nro1, int* nro2, u_int32_t* contenido, t_Payload* payload){ //receive_2int(&n1,&n2,payload);
    // Extrae el primer entero del payload
    int offset = 0;
    memcpy(nro1, payload->stream + offset, sizeof(int));
    offset += sizeof(int);

    // Extrae el segundo entero del payload
    memcpy(nro2, payload->stream + offset, sizeof(int));
    offset += sizeof(int);
    
    // Extrae el tercer u_int32_t del payload
    memcpy(contenido, payload->stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);
}

void send_String_1int(int nro, char* mensaje, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, (void*)(strlen(mensaje)+1), sizeof(int) );
    payload_enqueue(package->payload, mensaje, strlen(mensaje)+1 );
    payload_enqueue(package->payload, (void*) nro, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}
