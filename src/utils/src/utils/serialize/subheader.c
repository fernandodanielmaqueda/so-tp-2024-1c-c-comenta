/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/subheader.h"

void header_send(e_Header *header, int fd_socket) {
  t_Package *package = package_create_with_header(SUBHEADER_HEADER);
  header_serialize(package->payload, header);
  package_send(package, fd_socket);
  package_destroy(package);
}

void header_serialize(t_Payload *payload, e_Header *header) {
  payload_enqueue(payload, header, sizeof(t_Header_Serialized));

  header_log(header);
}

e_Header *header_deserialize(t_Payload *payload) {
  e_Header *header = malloc(sizeof(e_Header));

  payload_dequeue(payload, header, sizeof(t_Header_Serialized));

  header_log(header);
  return header;
}

void header_log(e_Header *header) {
  log_info(SERIALIZE_LOGGER,
    "e_Header[%p]: %d"
    , (void *) header
    , *header
  );
}