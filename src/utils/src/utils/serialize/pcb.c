/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/pcb.h"

void pcb_serialize(t_Payload *payload, t_PCB source) {
  if(payload == NULL)
    return;

  payload_enqueue(payload, &(source.PID), sizeof(source.PID));
  payload_enqueue(payload, &(source.PC), sizeof(source.PC));
  payload_enqueue(payload, &(source.AX), sizeof(source.AX));
  payload_enqueue(payload, &(source.BX), sizeof(source.BX));
  payload_enqueue(payload, &(source.CX), sizeof(source.CX));
  payload_enqueue(payload, &(source.DX), sizeof(source.DX));
  payload_enqueue(payload, &(source.EAX), sizeof(source.EAX));
  payload_enqueue(payload, &(source.EBX), sizeof(source.EBX));
  payload_enqueue(payload, &(source.ECX), sizeof(source.ECX));
  payload_enqueue(payload, &(source.EDX), sizeof(source.EDX));
  payload_enqueue(payload, &(source.RAX), sizeof(source.RAX));
  payload_enqueue(payload, &(source.RBX), sizeof(source.RBX));
  payload_enqueue(payload, &(source.RCX), sizeof(source.RCX));
  payload_enqueue(payload, &(source.RDX), sizeof(source.RDX));
  payload_enqueue(payload, &(source.SI), sizeof(source.SI));
  payload_enqueue(payload, &(source.DI), sizeof(source.DI));
  payload_enqueue(payload, &(source.quantum), sizeof(source.quantum));
  payload_enqueue(payload, &(source.current_state), sizeof(source.current_state));
  payload_enqueue(payload, &(source.arrival_READY), sizeof(source.arrival_READY));
  payload_enqueue(payload, &(source.arrival_RUNNING), sizeof(source.arrival_RUNNING));

  pcb_log(source);
}

void pcb_deserialize(t_Payload *payload, t_PCB *destination) {
  if(payload == NULL || destination == NULL)
    return;

  payload_dequeue(payload, &(destination->PID), sizeof(destination->PID));
  payload_dequeue(payload, &(destination->PC), sizeof(destination->PC));
  payload_dequeue(payload, &(destination->AX), sizeof(destination->AX));
  payload_dequeue(payload, &(destination->BX), sizeof(destination->BX));
  payload_dequeue(payload, &(destination->CX), sizeof(destination->CX));
  payload_dequeue(payload, &(destination->DX), sizeof(destination->DX));
  payload_dequeue(payload, &(destination->EAX), sizeof(destination->EAX));
  payload_dequeue(payload, &(destination->EBX), sizeof(destination->EBX));
  payload_dequeue(payload, &(destination->ECX), sizeof(destination->ECX));
  payload_dequeue(payload, &(destination->EDX), sizeof(destination->EDX));
  payload_dequeue(payload, &(destination->RAX), sizeof(destination->RAX));
  payload_dequeue(payload, &(destination->RBX), sizeof(destination->RBX));
  payload_dequeue(payload, &(destination->RCX), sizeof(destination->RCX));
  payload_dequeue(payload, &(destination->RDX), sizeof(destination->RDX));
  payload_dequeue(payload, &(destination->SI), sizeof(destination->SI));
  payload_dequeue(payload, &(destination->DI), sizeof(destination->DI));
  payload_dequeue(payload, &(destination->quantum), sizeof(destination->quantum));
  payload_dequeue(payload, &(destination->current_state), sizeof(destination->current_state));
  payload_dequeue(payload, &(destination->arrival_READY), sizeof(destination->arrival_READY));
  payload_dequeue(payload, &(destination->arrival_RUNNING), sizeof(destination->arrival_RUNNING));

  pcb_log(*destination);
}

void pcb_log(t_PCB pcb) {
  log_info(SERIALIZE_LOGGER,
    "t_PCB:\n"
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
    "* quantum: %" PRIu64 "\n"
    "* current_state: %" PRIu8 "\n"
    "* arrival_READY: %g\n"
    "* arrival_RUNNING: %g"
    , pcb.PID
    , pcb.PC
    , pcb.AX
    , pcb.BX
    , pcb.CX
    , pcb.DX
    , pcb.EAX
    , pcb.EBX
    , pcb.ECX
    , pcb.EDX
    , pcb.RAX
    , pcb.RBX
    , pcb.RCX
    , pcb.RDX
    , pcb.SI
    , pcb.DI
    , pcb.quantum
    , pcb.current_state
    , pcb.arrival_READY
    , pcb.arrival_RUNNING  
    );
}