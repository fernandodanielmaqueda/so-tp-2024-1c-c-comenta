/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/send.h"

void send_header(e_Header header, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  package_send(package, fd_socket);
  package_destroy(package);
}

void send_return_value_with_header(e_Header header, t_Return_Value return_value, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  return_value_serialize(package->payload, return_value);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_return_value_with_header(e_Header expected_header, t_Return_Value *return_value, int fd_socket) {
  t_Package *package = package_receive(fd_socket);
  if(package->header == expected_header)
    return_value_deserialize(package->payload, return_value);
  else {
      log_error(MODULE_LOGGER, "Header invalido");
      exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

/*
void send_process_create(t_Return_Value return_value, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_CREATE_HEADER);
  return_value_serialize(package->payload, &return_value);
  package_send(package, fd_socket);
  package_destroy(package);
}
*/

void send_cpu_memory_request(e_CPU_Memory_Request memory_request, int fd_socket) {
  t_Package *package = package_create_with_header(CPU_MEMORY_REQUEST_HEADER);
  cpu_memory_request_serialize(package->payload, memory_request);
  package_send(package, fd_socket);
  package_destroy(package);
}

void send_cpu_opcode(e_CPU_OpCode cpu_opcode, int fd_socket) {
  t_Package *package = package_create_with_header(CPU_INSTRUCTION_HEADER);
  cpu_opcode_serialize(package->payload, cpu_opcode);
  package_send(package, fd_socket);
  package_destroy(package);
}

void send_eviction_reason(e_Eviction_Reason eviction_reason, int fd_socket) {
  t_Package *package = package_create_with_header(EVICTION_REASON_HEADER);
  eviction_reason_serialize(package->payload, eviction_reason);
  package_send(package, fd_socket);
  package_destroy(package);
}

void send_kernel_interrupt(e_Kernel_Interrupt kernel_interrupt, int fd_socket) {
  t_Package *package = package_create_with_header(KERNEL_INTERRUPT_HEADER);
  kernel_interrupt_serialize(package->payload, kernel_interrupt);
  package_send(package, fd_socket);
  package_destroy(package);
}

void send_pcb(t_PCB pcb, int fd_socket) {
  t_Package *package = package_create_with_header(PCB_HEADER);
  pcb_serialize(package->payload, pcb);
  package_send(package, fd_socket);
  package_destroy(package);
}

void send_text_with_header(e_Header header, char *text, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  text_serialize(package->payload, text);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_text_with_header(e_Header expected_header, char **text, int fd_socket) {
  t_Package *package = package_receive(fd_socket);
  if(package->header == expected_header)
      text_deserialize(package->payload, text);
  else {
      log_error(MODULE_LOGGER, "Header invalido");
      exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

//

/*
void send_string(char* mensaje, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, mensaje, strlen(mensaje)+1 );
  package_send(package, socket);
  package_destroy(package);
}


void send_int(int nro, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, &nro, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}

void send_2int(int nro, int nro2, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, &nro, sizeof(int) );
    payload_enqueue(package->payload, &nro2, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}

void receive_2int(int* nro1, int* nro2, t_Payload* payload){ //receive_2int(&n1,&n2,payload);
  payload_dequeue(payload, nro1, sizeof(int) );
  payload_dequeue(payload, nro2, sizeof(int) );
}

void receive_int(int* nro, t_Payload* payload){ //receive_int(&n1,payload);
  payload_dequeue(payload, nro, sizeof(int) );
}

void send_2int_1uint32(int nro, int nro2, uint32_t contenido, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, &nro, sizeof(int) );
    payload_enqueue(package->payload, &nro2, sizeof(int) );
    payload_enqueue(package->payload, &contenido, sizeof(uint32_t) );
  package_send(package, socket);
  package_destroy(package);
}

void receive_2int_1uint32(int* nro1, int* nro2, uint32_t* contenido, t_Payload* payload){
  payload_dequeue(payload, nro1, sizeof(int) );
  payload_dequeue(payload, nro2, sizeof(int) );
  payload_dequeue(payload, contenido, sizeof(uint32_t) );
}

//receive_write_request(&n1,&n2,&content,&bytes,payload);
void receive_write_request(int* pid, int* dir_fis, int* bytes, char* contenido, t_Payload* payload){
  payload_dequeue(payload, pid, sizeof(int) );
  payload_dequeue(payload, dir_fis, sizeof(int) );
  payload_dequeue(payload, bytes, sizeof(int) );
  payload_dequeue(payload, contenido, (size_t) bytes );
}

void send_write_request(int pid, int dir_fis, char* mensaje, int socket, int opcod){
    t_Package* package = package_create_with_header(opcod);
    payload_enqueue(package->payload, &pid, sizeof(int) );
    payload_enqueue(package->payload, &dir_fis, sizeof(int) );
    text_serialize(package->payload, mensaje );
  package_send(package, socket);
  package_destroy(package);
}

void receive_read_request(int* pid, int* dir_fis, int* bytes, t_Payload* payload){ 
  payload_dequeue(payload, pid, sizeof(int) );
  payload_dequeue(payload, dir_fis, sizeof(int) );
  payload_dequeue(payload, bytes, sizeof(int) );
}

void send_read_request(int pid, int dir_fis, int bytes, int socket, int opcod){
  t_Package* package = package_create_with_header(opcod);
  payload_enqueue(package->payload, &pid, sizeof(int) );
  payload_enqueue(package->payload, &dir_fis, sizeof(int) );
  payload_enqueue(package->payload, &bytes, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}


void send_String_1int(int nro, char* mensaje, int socket, int opcod){
  t_Package* package = package_create_with_header(opcod);
  text_serialize(package->payload, mensaje);
  payload_enqueue(package->payload, &nro, sizeof(int) );
  package_send(package, socket);
  package_destroy(package);
}

void receive_String_1int(int* pid, char** mensaje, t_Payload* payload){

  payload_dequeue(payload, pid, sizeof(int));
  text_deserialize(payload, mensaje);

}
*/