/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/exit.h"

void exit_send(e_Exit *exit, int fd_socket) {
  t_Package *package = package_create_with_header(INTERRUPT_HEADER);
  exit_serialize(package->payload, exit);
  package_send(package, fd_socket);
  package_destroy(package);
}

void exit_serialize(t_Payload *payload, e_Exit *exit) {
  payload_enqueue(payload, exit, sizeof(uint8_t));

  exit_log(exit);
}

e_Exit *exit_deserialize(t_Payload *payload) {
  e_Exit *exit = malloc(sizeof(e_Exit));

  payload_dequeue(payload, exit, sizeof(uint8_t));

  exit_log(exit);
  return exit;
}

void exit_log(e_Exit *exit) {
  log_info(SERIALIZE_LOGGER,
    "e_Exit[%p]: %d"
    , (void *) exit
    , *exit
  );
}