/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/send.h"

// Handshake

int send_port_type(e_Port_Type port_type, int fd_socket) {
  t_Package *package = package_create_with_header(PORT_TYPE_HEADER);
  port_type_serialize(&(package->payload), port_type);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_port_type(e_Port_Type *port_type, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == PORT_TYPE_HEADER)
    port_type_deserialize(&(package->payload), port_type);
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

// De uso general

int send_header(e_Header header, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_expected_header(e_Header expected_header, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header != expected_header) {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

int send_text_with_header(e_Header header, char *text, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  text_serialize(&(package->payload), text);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_text_with_expected_header(e_Header expected_header, char **text, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == expected_header)
      text_deserialize(&(package->payload), text);
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

int send_return_value_with_header(e_Header header, t_Return_Value return_value, int fd_socket) {
  t_Package *package = package_create_with_header(header);
  return_value_serialize(&(package->payload), return_value);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_return_value_with_expected_header(e_Header expected_header, t_Return_Value *return_value, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == expected_header)
    return_value_deserialize(&(package->payload), return_value);
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

// Kernel - Memoria

int send_process_create(t_PID pid, char *instructions_path, t_Return_Value flag_relative_path, int fd_socket) {
    t_Package *package = package_create_with_header(PROCESS_CREATE_HEADER);
    payload_append(&(package->payload), &pid, sizeof(pid));
    text_serialize(&(package->payload), instructions_path);
    return_value_serialize(&(package->payload), flag_relative_path);
    if(package_send(package, fd_socket))
      return 1;
    package_destroy(package);
    return 0;
}

int send_process_destroy(t_PID pid, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_DESTROY_HEADER);
  payload_append(&(package->payload), &pid, sizeof(pid));
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

// Kernel - CPU

int send_process_dispatch(t_Exec_Context exec_context, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_DISPATCH_HEADER);
  exec_context_serialize(&(package->payload), exec_context);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_process_dispatch(t_Exec_Context *exec_context, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == PROCESS_DISPATCH_HEADER) {
    exec_context_deserialize(&(package->payload), exec_context);
  } else {
    log_error(SERIALIZE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

int send_process_eviction(t_Exec_Context exec_context, e_Eviction_Reason eviction_reason, t_Payload syscall_instruction, int fd_socket) {
  t_Package *package = package_create_with_header(PROCESS_EVICTION_HEADER);
  exec_context_serialize(&(package->payload), exec_context);
  eviction_reason_serialize(&(package->payload), eviction_reason);
  subpayload_serialize(&(package->payload), syscall_instruction);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_process_eviction(t_Exec_Context *exec_context, e_Eviction_Reason *eviction_reason, t_Payload *syscall_instruction, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == PROCESS_EVICTION_HEADER) {
    exec_context_deserialize(&(package->payload), exec_context);
    eviction_reason_deserialize(&(package->payload), eviction_reason);
    subpayload_deserialize(&(package->payload), syscall_instruction);
  } else {
    log_error(SERIALIZE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

int send_kernel_interrupt(e_Kernel_Interrupt type, t_PID pid, int fd_socket) {
	t_Package *package = package_create_with_header(KERNEL_INTERRUPT_HEADER);
	kernel_interrupt_serialize(&(package->payload), type);
	payload_append(&(package->payload), &pid, sizeof(pid));
  if(package_send(package, fd_socket))
    return 1;
	package_destroy(package);
  return 0;
}

int receive_kernel_interrupt(e_Kernel_Interrupt *kernel_interrupt, t_PID *pid, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == KERNEL_INTERRUPT_HEADER) {
    kernel_interrupt_deserialize(&(package->payload), kernel_interrupt);
    payload_shift(&(package->payload), pid, sizeof(*pid));
  } else {
    log_error(SERIALIZE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

// Kernel - Entrada/Salida

int send_interface_data(char *interface_name, e_IO_Type io_type, int fd_socket) {
	t_Package *package = package_create_with_header(INTERFACE_DATA_REQUEST_HEADER);
	text_serialize(&(package->payload), interface_name);
	io_type_serialize(&(package->payload), io_type);
  if(package_send(package, fd_socket))
    return 1;
	package_destroy(package);
  return 0;
}

int receive_interface_data(char **interface_name, e_IO_Type *io_type, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == INTERFACE_DATA_REQUEST_HEADER) {
    text_deserialize(&(package->payload), interface_name);
    io_type_deserialize(&(package->payload), io_type);
  }
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

int send_io_operation_dispatch(t_PID pid, t_Payload io_operation, int fd_socket) {
  t_Package *package = package_create_with_header(IO_OPERATION_DISPATCH_HEADER);
  payload_append(&(package->payload), &pid, sizeof(pid));
  subpayload_serialize(&(package->payload), io_operation);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_io_operation_dispatch(t_PID *pid, t_Payload *io_operation, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == IO_OPERATION_DISPATCH_HEADER) {
    payload_shift(&(package->payload), pid, sizeof(*pid));
    subpayload_deserialize(&(package->payload), io_operation);
  }
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

int send_io_operation_finished(t_PID pid, t_Return_Value return_value, int fd_socket) {
  t_Package *package = package_create_with_header(IO_OPERATION_FINISHED_HEADER);
  payload_append(&(package->payload), &pid, sizeof(pid));
  return_value_serialize(&(package->payload), return_value);
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}

int receive_io_operation_finished(t_PID *pid, t_Return_Value *return_value, int fd_socket) {
  t_Package *package;
  if(package_receive(&package, fd_socket))
    return 1;
  if(package->header == IO_OPERATION_FINISHED_HEADER) {
    payload_shift(&(package->payload), pid, sizeof(*pid));
    return_value_deserialize(&(package->payload), return_value);
  }
  else {
    log_error(MODULE_LOGGER, "Header invalido");
    return 1;
  }
  package_destroy(package);
  return 0;
}

// CPU - Memoria

int send_instruction_request(t_PID pid, t_PC pc, int fd_socket) {
  t_Package *package = package_create_with_header(INSTRUCTION_REQUEST);
  payload_append(&(package->payload), &pid, sizeof(pid));
  payload_append(&(package->payload), &pc, sizeof(pc));
  if(package_send(package, fd_socket))
    return 1;
  package_destroy(package);
  return 0;
}