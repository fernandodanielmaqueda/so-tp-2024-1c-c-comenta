/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "package.h"

Package *package_create(void) {
  Package *package = malloc(sizeof(Package));
  package->payload = payload_create();
  return package;
}

Package *package_create_with_header(Header header) {
  Package *package = package_create();
  package->header = header;
  return package;
}

void package_destroy(Package *package) {
  if (package != NULL) {
    payload_destroy(package->payload);
    free(package);
  }
}

void package_add(Package *package, void *data, Size dataSize) {
  payload_add(package->payload, data, dataSize);
}

void package_send(Package* package, int fd_socket) {
  size_t bufferSize = sizeof(package->header) + sizeof(package->payload->size) + (size_t) package->payload->size;
  void *buffer = package_serialize(package, bufferSize);

  send(fd_socket, buffer, bufferSize, 0);

  free(buffer);
  package_destroy(package);
}

void *package_serialize(Package *package, size_t bufferSize) {
  void *buffer = malloc(bufferSize);
  Size offset = 0;

  memcpy((void*)(((uint8_t*) buffer) + offset), &(package->header), sizeof(package->header));
  offset += sizeof(package->header);

  memcpy((void*)(((uint8_t*) buffer) + offset), &(package->payload->size), sizeof(package->payload->size));
  offset += sizeof(package->payload->size);

  memcpy((void*)(((uint8_t*) buffer) + offset), package->payload->stream, package->payload->size);

  return buffer;
}

Package *package_receive(int fd_socket) {
  Package *package = package_create();
  package_receive_header(package, fd_socket);
  package_receive_payload(package, fd_socket);
}

void package_receive_header(Package *package, int fd_socket) {
  recv(fd_socket, (void *) &(package->header), sizeof(Header), 0); // MSG_WAITALL
}

void package_receive_payload(Package *package, int fd_socket) {
  recv(fd_socket, (void *) &(package->payload->size), sizeof(Size), 0); // MSG_WAITALL
  if(package->payload->size == 0) return;
  package->payload->stream = malloc(package->payload->size);
  recv(fd_socket, package->payload->stream, (size_t) package->payload->size, 0); // MSG_WAITALL
}

void package_deserialize(Package *package) {
  switch((enum HeaderCode) package->header) {
    case DISCONNECTION_HEADERCODE:
      break;
    case PCB_HEADERCODE:
      pcb_deserialize(package->payload);
      break;
    default:
      break;
  }
}

// Crea un payload vacío de tamaño size y offset 0
Payload *payload_create(void) {
  Payload *payload = malloc(sizeof(Payload));
  payload->size = 0;
  payload->stream = NULL;
  return payload;
}

// Libera la memoria asociada al payload
void payload_destroy(Payload *payload) {
  if (payload != NULL) {
    // TODO: Verificar si es necesario liberar el stream
    free(payload->stream);
    free(payload);
  }
}

// Agrega un stream al payload en la posición actual y avanza el offset
void payload_add(Payload *payload, void *data, Size dataSize) {
  payload->stream = realloc(payload->stream, (size_t) (payload->size + dataSize));
  memcpy((void*)(((uint8_t*) payload->stream) + payload->size), data, dataSize);
  payload->size += dataSize;
}

// Guarda size bytes del principio del payload en la dirección data y avanza el offset
void payload_read(Payload *payload, void *data, uint32_t size) {

}

// Agrega un uint32_t al payload
void payload_add_uint32(Payload *payload, uint32_t data) {
  payload_add(payload, (void*) &data, (uint32_t) sizeof(uint32_t));
}

// Lee un uint32_t del payload y avanza el offset
uint32_t payload_read_uint32(Payload *payload) {

}

// Agrega un uint8_t al payload
void payload_add_uint8(Payload *payload, uint8_t data) {
  payload_add(payload, (void*) &data, (Size) sizeof(uint8_t));
}

// Lee un uint8_t del payload y avanza el offset
uint8_t payload_read_uint8(Payload *payload) {

}

// Agrega string al payload con un uint32_t adelante indicando su longitud
void payload_add_string(Payload *payload, uint32_t length, char *string) {
  payload_add(payload, (void*) &length, (uint32_t) sizeof(uint32_t));
  payload_add(payload, (void*) string, length);
}

// Lee un string y su longitud del payload y avanza el offset
char *payload_read_string(Payload *payload, uint32_t *length) {

}

//

t_list* get_package_like_list(int fd_client) {
  int payload_size;
  int tamanioContenido;
  int offset = 0;

  t_list *contenido = list_create();
  void *payload = payload_receive(&payload_size, fd_client);

  while (offset < payload_size)
  {
    memcpy((void*) &tamanioContenido, (void*)(((uint8_t*) payload) + offset), sizeof(int));
    offset += sizeof(int);

    void *valor = malloc(tamanioContenido);
    memcpy(valor, (void*)(((uint8_t*) payload) + offset), tamanioContenido);
    offset += tamanioContenido;

    list_add(contenido, valor);
  }

  free(payload);
  return contenido;
}

void free_string_element(void* element) {
    free((char*) element);
}