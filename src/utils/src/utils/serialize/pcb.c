/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/pcb.h"

void pcb_serialize(t_Payload *payload, t_PCB source) {
  if(payload == NULL)
    return;
  
  t_EnumValue aux;

  payload_enqueue(payload, &(source.PID), sizeof(source.PID));
  payload_enqueue(payload, &(source.PC), sizeof(source.PC));
  payload_enqueue(payload, &(source.cpu_registers.AX), sizeof(source.cpu_registers.AX));
  payload_enqueue(payload, &(source.cpu_registers.BX), sizeof(source.cpu_registers.BX));
  payload_enqueue(payload, &(source.cpu_registers.CX), sizeof(source.cpu_registers.CX));
  payload_enqueue(payload, &(source.cpu_registers.DX), sizeof(source.cpu_registers.DX));
  payload_enqueue(payload, &(source.cpu_registers.EAX), sizeof(source.cpu_registers.EAX));
  payload_enqueue(payload, &(source.cpu_registers.EBX), sizeof(source.cpu_registers.EBX));
  payload_enqueue(payload, &(source.cpu_registers.ECX), sizeof(source.cpu_registers.ECX));
  payload_enqueue(payload, &(source.cpu_registers.EDX), sizeof(source.cpu_registers.EDX));
  payload_enqueue(payload, &(source.cpu_registers.RAX), sizeof(source.cpu_registers.RAX));
  payload_enqueue(payload, &(source.cpu_registers.RBX), sizeof(source.cpu_registers.RBX));
  payload_enqueue(payload, &(source.cpu_registers.RCX), sizeof(source.cpu_registers.RCX));
  payload_enqueue(payload, &(source.cpu_registers.RDX), sizeof(source.cpu_registers.RDX));
  payload_enqueue(payload, &(source.cpu_registers.SI), sizeof(source.cpu_registers.SI));
  payload_enqueue(payload, &(source.cpu_registers.DI), sizeof(source.cpu_registers.DI));
    aux = (t_EnumValue) source.current_state;
  payload_enqueue(payload, &aux, sizeof(t_EnumValue));
  payload_enqueue(payload, &(source.quantum), sizeof(source.quantum));
    aux = (t_EnumValue) source.exit_reason;
  payload_enqueue(payload, &aux, sizeof(t_EnumValue));

  pcb_log(source);
}

void pcb_deserialize(t_Payload *payload, t_PCB *destination) {
  if(payload == NULL || destination == NULL)
    return;

  t_EnumValue aux;

  payload_dequeue(payload, &(destination->PID), sizeof(destination->PID));
  payload_dequeue(payload, &(destination->PC), sizeof(destination->PC));
  payload_dequeue(payload, &(destination->cpu_registers.AX), sizeof(destination->cpu_registers.AX));
  payload_dequeue(payload, &(destination->cpu_registers.BX), sizeof(destination->cpu_registers.BX));
  payload_dequeue(payload, &(destination->cpu_registers.CX), sizeof(destination->cpu_registers.CX));
  payload_dequeue(payload, &(destination->cpu_registers.DX), sizeof(destination->cpu_registers.DX));
  payload_dequeue(payload, &(destination->cpu_registers.EAX), sizeof(destination->cpu_registers.EAX));
  payload_dequeue(payload, &(destination->cpu_registers.EBX), sizeof(destination->cpu_registers.EBX));
  payload_dequeue(payload, &(destination->cpu_registers.ECX), sizeof(destination->cpu_registers.ECX));
  payload_dequeue(payload, &(destination->cpu_registers.EDX), sizeof(destination->cpu_registers.EDX));
  payload_dequeue(payload, &(destination->cpu_registers.RAX), sizeof(destination->cpu_registers.RAX));
  payload_dequeue(payload, &(destination->cpu_registers.RBX), sizeof(destination->cpu_registers.RBX));
  payload_dequeue(payload, &(destination->cpu_registers.RCX), sizeof(destination->cpu_registers.RCX));
  payload_dequeue(payload, &(destination->cpu_registers.RDX), sizeof(destination->cpu_registers.RDX));
  payload_dequeue(payload, &(destination->cpu_registers.SI), sizeof(destination->cpu_registers.SI));
  payload_dequeue(payload, &(destination->cpu_registers.DI), sizeof(destination->cpu_registers.DI));
  payload_dequeue(payload, &aux, sizeof(t_EnumValue));
    destination->current_state = (e_Process_State) aux;
  payload_dequeue(payload, &(destination->quantum), sizeof(destination->quantum));
  payload_dequeue(payload, &aux, sizeof(t_EnumValue));
    destination->exit_reason = (e_Exit_Reason) aux;

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
    "* current_state: %d\n"
    "* quantum: %" PRIu64
    , pcb.PID
    , pcb.PC
    , pcb.cpu_registers.AX
    , pcb.cpu_registers.BX
    , pcb.cpu_registers.CX
    , pcb.cpu_registers.DX
    , pcb.cpu_registers.EAX
    , pcb.cpu_registers.EBX
    , pcb.cpu_registers.ECX
    , pcb.cpu_registers.EDX
    , pcb.cpu_registers.RAX
    , pcb.cpu_registers.RBX
    , pcb.cpu_registers.RCX
    , pcb.cpu_registers.RDX
    , pcb.cpu_registers.SI
    , pcb.cpu_registers.DI
    , pcb.current_state
    , pcb.quantum
    );
}