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
  if (package == NULL) return;
  payload_destroy(package->payload);
  free(package);
}

void package_send(Package *package, int fd_socket) {
  size_t bufferSize = sizeof(package->header) + sizeof(package->payload->size) + (size_t) package->payload->size;
  void *buffer = package_serialize(package, bufferSize);

  send(fd_socket, buffer, bufferSize, 0);

  free(buffer);
}

void *package_serialize(Package *package, size_t bufferSize) {
  void *buffer = malloc(bufferSize);
  size_t offset = 0;

  offset = memcpy_destination_offset(buffer, offset, &(package->header), sizeof(package->header));
  offset = memcpy_destination_offset(buffer, offset, &(package->payload->size), sizeof(package->payload->size));
  offset = memcpy_destination_offset(buffer, offset, package->payload->stream, (size_t) package->payload->size);

  return buffer;
}

Package *package_receive(int fd_socket) {
  Package *package = package_create();
  package_receive_header(package, fd_socket);
  package_receive_payload(package, fd_socket);
  return package;
}

void package_receive_header(Package *package, int fd_socket) {
  recv(fd_socket, (void *) &(package->header), sizeof(Header), 0); // MSG_WAITALL
}

void package_receive_payload(Package *package, int fd_socket) {
  recv(fd_socket, &(package->payload->size), sizeof(PayloadSize), 0); // MSG_WAITALL
  if(package->payload->size == 0) return;
  package->payload->stream = malloc(package->payload->size);
  recv(fd_socket, package->payload->stream, (size_t) package->payload->size, 0); // MSG_WAITALL
}