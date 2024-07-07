/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/send.h"

// Handshake

void send_port_type(e_Port_Type port_type, int fd_socket) {
  t_Package *package = package_create_with_header(PORT_TYPE_HEADER);
  port_type_serialize(package->payload, port_type);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_port_type(e_Port_Type *port_type, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header == PORT_TYPE_HEADER)
    port_type_deserialize(package->payload, port_type);
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

// De uso general

void send_header(e_Header header, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_expected_header(e_Header expected_header, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header != expected_header) {
    log_error(MODULE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

void send_text_with_header(e_Header header, char *text, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  text_serialize(package->payload, text);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_text_with_expected_header(e_Header expected_header, char **text, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header == expected_header)
      text_deserialize(package->payload, text);
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

void send_return_value_with_header(e_Header header, t_Return_Value return_value, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  return_value_serialize(package->payload, return_value);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_return_value_with_expected_header(e_Header expected_header, t_Return_Value *return_value, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header == expected_header)
    return_value_deserialize(package->payload, return_value);
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

// Kernel - Memoria

void send_process_create(char *instructions_path, t_PID pid, int fd_socket) {
    t_Package *package = package_create_with_header(PROCESS_CREATE_HEADER);
    text_serialize(package->payload, instructions_path);
    payload_enqueue(package->payload, &pid, sizeof(pid));
    package_send(package, fd_socket);
    package_destroy(package);
}

void send_process_destroy(t_PID pid, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_DESTROY_HEADER);
  payload_enqueue(package->payload, &pid, sizeof(pid));
  package_send(package, fd_socket);
  package_destroy(package);
}

// Kernel - CPU

void send_process_dispatch(t_PCB pcb, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_DISPATCH_HEADER);
  pcb_serialize(package->payload, pcb);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_process_dispatch(t_PCB *pcb, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header == PROCESS_DISPATCH_HEADER) {
    pcb_deserialize(package->payload, pcb);
  } else {
    log_error(SERIALIZE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

void send_process_eviction(t_PCB pcb, e_Eviction_Reason eviction_reason, t_Payload syscall_instruction, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_EVICTION_HEADER);
  pcb_serialize(package->payload, pcb);
  eviction_reason_serialize(package->payload, eviction_reason);
  subpayload_serialize(package->payload, syscall_instruction);
  package_send(package, fd_socket);
  package_destroy(package);
}

void receive_process_eviction(t_PCB *pcb, e_Eviction_Reason *eviction_reason, t_Payload *syscall_instruction, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header == PROCESS_EVICTION_HEADER) {
    pcb_deserialize(package->payload, pcb);
    eviction_reason_deserialize(package->payload, eviction_reason);
    subpayload_deserialize(package->payload, syscall_instruction);
  } else {
    log_error(SERIALIZE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

void send_kernel_interrupt(e_Kernel_Interrupt type, t_PID pid, int fd_socket) {
	t_Package *package = package_create_with_header(KERNEL_INTERRUPT_HEADER);
	kernel_interrupt_serialize(package->payload, type);
	payload_enqueue(package->payload, &pid, sizeof(pid));
	package_send(package, fd_socket);
	package_destroy(package);
}

void receive_kernel_interrupt(e_Kernel_Interrupt *kernel_interrupt, t_PID *pid, int fd_socket) {
  t_Package *package;
  package_receive(&package, fd_socket);
  if(package->header == KERNEL_INTERRUPT_HEADER) {
    kernel_interrupt_deserialize(package->payload, kernel_interrupt);
    payload_dequeue(package->payload, pid, sizeof(*pid));
  } else {
    log_error(SERIALIZE_LOGGER, "Header invalido");
    exit(EXIT_FAILURE);
  }
  package_destroy(package);
}

// CPU - Memoria

void send_instruction_request(t_PID pid, t_PC pc, int fd_socket) {
  t_Package *package = package_create_with_header(INSTRUCTION_REQUEST);
  payload_enqueue(package->payload, &pid, sizeof(pid));
  payload_enqueue(package->payload, &pc, sizeof(pc));
  package_send(package, fd_socket);
  package_destroy(package);
}