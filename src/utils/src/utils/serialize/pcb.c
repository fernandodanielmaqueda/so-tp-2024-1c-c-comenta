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
  payload_enqueue(payload, &(pcb->PID), sizeof(pcb->PID));
  payload_enqueue(payload, &(pcb->PC), sizeof(pcb->PC));
  payload_enqueue(payload, &(pcb->AX), sizeof(pcb->AX));
  payload_enqueue(payload, &(pcb->BX), sizeof(pcb->BX));
  payload_enqueue(payload, &(pcb->CX), sizeof(pcb->CX));
  payload_enqueue(payload, &(pcb->DX), sizeof(pcb->DX));
  payload_enqueue(payload, &(pcb->EAX), sizeof(pcb->EAX));
  payload_enqueue(payload, &(pcb->EBX), sizeof(pcb->EBX));
  payload_enqueue(payload, &(pcb->ECX), sizeof(pcb->ECX));
  payload_enqueue(payload, &(pcb->EDX), sizeof(pcb->EDX));
  payload_enqueue(payload, &(pcb->RAX), sizeof(pcb->RAX));
  payload_enqueue(payload, &(pcb->RBX), sizeof(pcb->RBX));
  payload_enqueue(payload, &(pcb->RCX), sizeof(pcb->RCX));
  payload_enqueue(payload, &(pcb->RDX), sizeof(pcb->RDX));
  payload_enqueue(payload, &(pcb->SI), sizeof(pcb->SI));
  payload_enqueue(payload, &(pcb->DI), sizeof(pcb->DI));
  payload_enqueue(payload, &(pcb->quantum), sizeof(pcb->quantum));
  payload_enqueue(payload, &(pcb->current_state), sizeof(pcb->current_state));
  payload_enqueue(payload, &(pcb->arrival_READY), sizeof(pcb->arrival_READY));
  payload_enqueue(payload, &(pcb->arrival_RUNNING), sizeof(pcb->arrival_RUNNING));

  pcb_log(pcb);
}

t_PCB *pcb_deserialize(t_Payload *payload) {
  t_PCB *pcb = malloc(sizeof(t_PCB));

  payload_dequeue(payload, &(pcb->PID), sizeof(pcb->PID));
  payload_dequeue(payload, &(pcb->PC), sizeof(pcb->PC));
  payload_dequeue(payload, &(pcb->AX), sizeof(pcb->AX));
  payload_dequeue(payload, &(pcb->BX), sizeof(pcb->BX));
  payload_dequeue(payload, &(pcb->CX), sizeof(pcb->CX));
  payload_dequeue(payload, &(pcb->DX), sizeof(pcb->DX));
  payload_dequeue(payload, &(pcb->EAX), sizeof(pcb->EAX));
  payload_dequeue(payload, &(pcb->EBX), sizeof(pcb->EBX));
  payload_dequeue(payload, &(pcb->ECX), sizeof(pcb->ECX));
  payload_dequeue(payload, &(pcb->EDX), sizeof(pcb->EDX));
  payload_dequeue(payload, &(pcb->RAX), sizeof(pcb->RAX));
  payload_dequeue(payload, &(pcb->RBX), sizeof(pcb->RBX));
  payload_dequeue(payload, &(pcb->RCX), sizeof(pcb->RCX));
  payload_dequeue(payload, &(pcb->RDX), sizeof(pcb->RDX));
  payload_dequeue(payload, &(pcb->SI), sizeof(pcb->SI));
  payload_dequeue(payload, &(pcb->DI), sizeof(pcb->DI));
  payload_dequeue(payload, &(pcb->quantum), sizeof(pcb->quantum));
  payload_dequeue(payload, &(pcb->current_state), sizeof(pcb->current_state));
  payload_dequeue(payload, &(pcb->arrival_READY), sizeof(pcb->arrival_READY));
  payload_dequeue(payload, &(pcb->arrival_RUNNING), sizeof(pcb->arrival_RUNNING));

  pcb_log(pcb);
  return pcb;
}

void pcb_log(t_PCB *pcb) {
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
    , (void *) pcb
    , pcb->PID
    , pcb->PC
    , pcb->AX
    , pcb->BX
    , pcb->CX
    , pcb->DX
    , pcb->EAX
    , pcb->EBX
    , pcb->ECX
    , pcb->EDX
    , pcb->RAX
    , pcb->RBX
    , pcb->RCX
    , pcb->RDX
    , pcb->SI
    , pcb->DI
    , pcb->quantum
    , pcb->current_state
    , pcb->arrival_READY
    , pcb->arrival_RUNNING
    );
}