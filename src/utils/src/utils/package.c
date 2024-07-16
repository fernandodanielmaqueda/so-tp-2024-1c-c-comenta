/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "package.h"

const char *HEADER_NAMES[] = {
    // Uso general
    [DISCONNECTING_HEADER] = "DISCONNECTING_HEADER",
    // Handshake
    [PORT_TYPE_HEADER] = "PORT_TYPE_HEADER",
    // Kernel <---> CPU
    [PROCESS_DISPATCH_HEADER] = "PROCESS_DISPATCH_HEADER",
    [PROCESS_EVICTION_HEADER] = "PROCESS_EVICTION_HEADER",
    [KERNEL_INTERRUPT_HEADER] = "KERNEL_INTERRUPT_HEADER",
    // Kernel <---> Memoria
    [PROCESS_CREATE_HEADER] = "PROCESS_CREATE_HEADER",
    [PROCESS_DESTROY_HEADER] = "PROCESS_DESTROY_HEADER",
    //Kernel <---> Entrada/Salida
    [INTERFACE_DATA_REQUEST_HEADER] = "INTERFACE_DATA_REQUEST_HEADER",
    [IO_OPERATION_DISPATCH_HEADER] = "IO_OPERATION_DISPATCH_HEADER",
    [IO_OPERATION_FINISHED_HEADER] = "IO_OPERATION_FINISHED_HEADER",

    // CPU <---> Memoria
    [INSTRUCTION_REQUEST] = "INSTRUCTION_REQUEST",
    [READ_REQUEST] = "READ_REQUEST", //utilizado en MEMORIA-IO
    [WRITE_REQUEST] = "WRITE_REQUEST", //utilizado en MEMORIA-IO
    [RESIZE_REQUEST] = "RESIZE_REQUEST",
    [FRAME_ACCESS] = "FRAME_ACCESS",    //PARA MEMORIA Y REVISAR LA TLB
    [FRAME_REQUEST] = "FRAME_REQUEST",
    [PAGE_SIZE_REQUEST] = "PAGE_SIZE_REQUEST",
    //IO <---> Memoria
    [IO_STDIN_WRITE_MEMORY] = "IO_STDIN_WRITE_MEMORY",
    [IO_STDOUT_READ_MEMORY] = "IO_STDOUT_READ_MEMORY"
};

t_Package *package_create(void) {

  t_Package *package = malloc(sizeof(t_Package));
  if(package == NULL) {
    log_error(SOCKET_LOGGER, "No se pudo crear el package con malloc");
    exit(EXIT_FAILURE);
  }

  package->payload = payload_create();

  return package;
}

t_Package *package_create_with_header(e_Header header) {
  t_Package *package = package_create();
  package->header = header;
  return package;
}

void package_destroy(t_Package *package) {
  if (package == NULL)
    return;
  payload_destroy(package->payload);
  free(package);
}

int package_send(t_Package *package, int fd_socket) {
  
  // Si el paquete es NULL, no se envia nada
  if(package == NULL)
    return 1;

  size_t bufferSize = sizeof(t_EnumValue) + sizeof(package->payload->size) + (size_t) package->payload->size;

  void *buffer = malloc(bufferSize);
  if(buffer == NULL) {
    log_error(SOCKET_LOGGER, "malloc: No se pudieron reservar %zu bytes de memoria\n", (size_t) bufferSize);
    return 1;
  }

  size_t offset = 0;

  t_EnumValue aux;

  aux = (t_EnumValue) package->header;
  offset = memcpy_serialize(buffer, offset, &(aux), sizeof(aux));
  offset = memcpy_serialize(buffer, offset, &(package->payload->size), sizeof(package->payload->size));
  offset = memcpy_serialize(buffer, offset, package->payload->stream, (size_t) package->payload->size);

  ssize_t bytes = send(fd_socket, buffer, bufferSize, 0);
  if (bytes == -1) {
      log_error(SOCKET_LOGGER, "Funcion send: %s\n", strerror(errno));
      return 1;
  }
  if (bytes != bufferSize) {
      log_error(SOCKET_LOGGER, "Funcion send: No coinciden los bytes enviados (%zd) con los que se esperaban enviar (%zd)\n", bufferSize, bytes);
      return 1;
  }

  free(buffer);
  return 0;
}

int package_receive(t_Package **destination, int fd_socket) {
  if(destination == NULL)
    return 1;

  *destination = package_create();

  if(package_receive_header(*destination, fd_socket))
    return 1;

  if(package_receive_payload(*destination, fd_socket))
    return 1;

  return 0;
}

int package_receive_header(t_Package *package, int fd_socket) {

  if(package == NULL)
    return 1;

  t_EnumValue aux;

  if(receive(fd_socket, (void *) &(aux), sizeof(aux)))
    return 1;

  package->header = (e_Header) aux;

  return 0;
}

int package_receive_payload(t_Package *package, int fd_socket) {

  if(package == NULL)
    return 1;

  if(receive(fd_socket, (void *) &(package->payload->size), sizeof(package->payload->size)))
    return 1;

  if(package->payload->size == 0)
    return 0;

  package->payload->stream = malloc((size_t) package->payload->size);
  if(package->payload->stream == NULL) {
    log_error(SOCKET_LOGGER, "malloc: No se pudo reservar %zu bytes de memoria\n", (size_t) package->payload->size);
    return 1;
  }

  return receive(fd_socket, (void *) package->payload->stream, (size_t) package->payload->size);
}

int receive(int fd_socket, void *destination, size_t expected_bytes) {

  ssize_t bytes = recv(fd_socket, destination, expected_bytes, 0); // MSG_WAITALL
  if (bytes == 0) {
      log_error(SOCKET_LOGGER, "Emisor Desconectado\n");
      return 1;
  }
  if (bytes == -1) {
      log_error(SOCKET_LOGGER, "Funcion recv: %s\n", strerror(errno));
      return 1;
  }
  if (bytes != expected_bytes) {
      log_error(SOCKET_LOGGER, "Funcion recv: No coinciden los bytes recibidos (%zu) con los que se esperaban recibir (%zd)\n", expected_bytes, bytes);
      return 1;
  }

  return 0;
}