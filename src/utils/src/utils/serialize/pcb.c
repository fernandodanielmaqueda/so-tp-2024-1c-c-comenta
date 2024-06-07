/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/pcb.h"

void pcb_send(t_PCB *pcb, int fd_socket) {
  t_Package *package = package_create_with_header(PCB_HEADER);
  pcb_serialize(package->payload, pcb);
  package_send(package, fd_socket);
  package_destroy(package);
}

void pcb_serialize(t_Payload *payload, t_PCB *pcb) {
  payload_add(payload, &(pcb->PID), sizeof(pcb->PID));
  payload_add(payload, &(pcb->PC), sizeof(pcb->PC));
  payload_add(payload, &(pcb->AX), sizeof(pcb->AX));
  payload_add(payload, &(pcb->BX), sizeof(pcb->BX));
  payload_add(payload, &(pcb->CX), sizeof(pcb->CX));
  payload_add(payload, &(pcb->DX), sizeof(pcb->DX));
  payload_add(payload, &(pcb->EAX), sizeof(pcb->EAX));
  payload_add(payload, &(pcb->EBX), sizeof(pcb->EBX));
  payload_add(payload, &(pcb->ECX), sizeof(pcb->ECX));
  payload_add(payload, &(pcb->EDX), sizeof(pcb->EDX));
  payload_add(payload, &(pcb->RAX), sizeof(pcb->RAX));
  payload_add(payload, &(pcb->RBX), sizeof(pcb->RBX));
  payload_add(payload, &(pcb->RCX), sizeof(pcb->RCX));
  payload_add(payload, &(pcb->RDX), sizeof(pcb->RDX));
  payload_add(payload, &(pcb->SI), sizeof(pcb->SI));
  payload_add(payload, &(pcb->DI), sizeof(pcb->DI));
  payload_add(payload, &(pcb->quantum), sizeof(pcb->quantum));
  payload_add(payload, &(pcb->current_state), sizeof(pcb->current_state));
  payload_add(payload, &(pcb->arrival_READY), sizeof(pcb->arrival_READY));
  payload_add(payload, &(pcb->arrival_RUNNING), sizeof(pcb->arrival_RUNNING));
}

t_PCB *pcb_deserialize(t_Payload *payload) {
  t_PCB *pcb = malloc(sizeof(t_PCB));

  uint32_t offset = 0;

  offset = memcpy_source_offset(&(pcb->PID), payload->stream, offset, sizeof(pcb->PID));
  offset = memcpy_source_offset(&(pcb->PC), payload->stream, offset, sizeof(pcb->PC));
  offset = memcpy_source_offset(&(pcb->AX), payload->stream, offset, sizeof(pcb->AX));
  offset = memcpy_source_offset(&(pcb->BX), payload->stream, offset, sizeof(pcb->BX));
  offset = memcpy_source_offset(&(pcb->CX), payload->stream, offset, sizeof(pcb->CX));
  offset = memcpy_source_offset(&(pcb->DX), payload->stream, offset, sizeof(pcb->DX));
  offset = memcpy_source_offset(&(pcb->EAX), payload->stream, offset, sizeof(pcb->EAX));
  offset = memcpy_source_offset(&(pcb->EBX), payload->stream, offset, sizeof(pcb->EBX));
  offset = memcpy_source_offset(&(pcb->ECX), payload->stream, offset, sizeof(pcb->ECX));
  offset = memcpy_source_offset(&(pcb->EDX), payload->stream, offset, sizeof(pcb->EDX));
  offset = memcpy_source_offset(&(pcb->RAX), payload->stream, offset, sizeof(pcb->RAX));
  offset = memcpy_source_offset(&(pcb->RBX), payload->stream, offset, sizeof(pcb->RBX));
  offset = memcpy_source_offset(&(pcb->RCX), payload->stream, offset, sizeof(pcb->RCX));
  offset = memcpy_source_offset(&(pcb->RDX), payload->stream, offset, sizeof(pcb->RDX));
  offset = memcpy_source_offset(&(pcb->SI), payload->stream, offset, sizeof(pcb->SI));
  offset = memcpy_source_offset(&(pcb->DI), payload->stream, offset, sizeof(pcb->DI));
  offset = memcpy_source_offset(&(pcb->quantum), payload->stream, offset, sizeof(pcb->quantum));
  offset = memcpy_source_offset(&(pcb->current_state), payload->stream, offset, sizeof(pcb->current_state));
  offset = memcpy_source_offset(&(pcb->arrival_READY), payload->stream, offset, sizeof(pcb->arrival_READY));
  offset = memcpy_source_offset(&(pcb->arrival_RUNNING), payload->stream, offset, sizeof(pcb->arrival_RUNNING));

  pcb_print(pcb);
  return pcb;
}

void pcb_print(t_PCB *pcb) {
  log_info(SERIALIZE_LOGGER,
    "t_PCB[%p]:\n"
    "* PID: %" PRIu32 "\n"
    "* PC: %" PRIu32 "\n"
    "* AX: %" PRIu8 "\n"
    "* BX: %" PRIu8 "\n"
    "* CX: %" PRIu8 "\n"
    "* DX: %" PRIu8 "\n"
    "* EAX: %" PRIu32 "\n"
    "* EBX: %" PRIu32 "\n"
    "* ECX: %" PRIu32 "\n"
    "* EDX: %" PRIu32 "\n"
    "* RAX: %" PRIu32 "\n"
    "* RBX: %" PRIu32 "\n"
    "* RCX: %" PRIu32 "\n"
    "* RDX: %" PRIu32 "\n"
    "* SI: %" PRIu32 "\n"
    "* DI: %" PRIu32 "\n"
    "* quantum: %" PRIu32 "\n"
    "* current_state: %" PRIu8 "\n"
    "* arrival_READY: %g\n"
    "* arrival_RUNNING: %g"
    ,(void *) pcb,
    pcb->PID,
    pcb->PC,
    pcb->AX,
    pcb->BX,
    pcb->CX,
    pcb->DX,
    pcb->EAX,
    pcb->EBX,
    pcb->ECX,
    pcb->EDX,
    pcb->RAX,
    pcb->RBX,
    pcb->RCX,
    pcb->RDX,
    pcb->SI,
    pcb->DI,
    pcb->quantum,
    pcb->current_state,
    pcb->arrival_READY,
    pcb->arrival_RUNNING
    );
}