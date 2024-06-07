/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/pcb.h"

void pcb_send(t_PCB *pcb, int fd_socket) {
  Package *package = package_create_with_header(PCB_HEADERCODE);
  pcb_serialize(package->payload, pcb);
  package_send(package, fd_socket);
  package_destroy(package);
}

void pcb_serialize(Payload *payload, t_PCB *pcb) {
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

t_PCB *pcb_deserialize(Payload *payload) {
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

  return pcb;
}

void pcb_print(t_PCB *pcb) {
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: PID: %" PRIu32, (void *) pcb, pcb->PID);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: PC: %" PRIu32, (void *) pcb, pcb->PC);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: AX: %" PRIu8, (void *) pcb, pcb->AX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: BX: %" PRIu8, (void *) pcb, pcb->BX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: CX: %" PRIu8, (void *) pcb, pcb->CX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: DX: %" PRIu8, (void *) pcb, pcb->DX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: EAX: %" PRIu32, (void *) pcb, pcb->EAX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: EBX: %" PRIu32, (void *) pcb, pcb->EBX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: ECX: %" PRIu32, (void *) pcb, pcb->ECX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: EDX: %" PRIu32, (void *) pcb, pcb->EDX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: RAX: %" PRIu32, (void *) pcb, pcb->RAX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: RBX: %" PRIu32, (void *) pcb, pcb->RBX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: RCX: %" PRIu32, (void *) pcb, pcb->RCX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: RDX: %" PRIu32, (void *) pcb, pcb->RDX);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: SI: %" PRIu32, (void *) pcb, pcb->SI);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: DI: %" PRIu32, (void *) pcb, pcb->DI);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: quantum: %" PRIu32, (void *) pcb, pcb->quantum);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: current_state: %" PRIu8, (void *) pcb, pcb->current_state);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: arrival_READY: %g", (void *) pcb, pcb->arrival_READY);
  log_info(SERIALIZE_LOGGER, "t_PCB[%p]: arrival_RUNNING: %g", (void *) pcb, pcb->arrival_RUNNING);
}

void instruction_send(t_instruction_use* instruccion, int socket) {
  
  Package* package = package_create_with_header(INSTUCTION_REQUEST);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_instruccion( ) [...]\n"); }
  
  int cursor = 0;
  int cantidad_parametros= list_size(instruccion->parameters);


    payload_add(package->payload, &instruccion->operation, sizeof(enum HeaderCode));
    DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Identificador instruccion = %d\n", cursor, instruccion->operation);
    cursor++;

  //PARAMETROS
  payload_add(package->payload, &cantidad_parametros, sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Cantidad de parametros = %d\n", cursor, cantidad_parametros); }
  cursor++;

  char* parametro;
  for (int i = 0; i < cantidad_parametros; i++)
  {
    parametro = list_get(instruccion->parameters, i);

    payload_add(package->payload, parametro, strlen(parametro) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Parametro = %s\n", cursor, parametro); }
    cursor++;
  }

  DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_instruccion( ) [END]\n");

  package_send(package, socket);

  package_destroy(package);
}

t_instruction_use* instruction_receive(int socket) {
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_instruccion( ) [...]\n"); }

  t_list *propiedadesPlanas = NULL;
  //t_list *propiedadesPlanas = get_package_like_list(socket);
  t_instruction_use* instruccionRecibida = malloc(sizeof(t_instruction_use));
  int cursor = 0;

  instruccionRecibida->operation = *(enum HeaderCode*)list_get(propiedadesPlanas, cursor);
  DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: instruccionRecibida->operation  = %d\n", cursor, instruccionRecibida->operation );
  
  int cantidadParametros = *(int*)list_get(propiedadesPlanas, cursor);
  DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: instruccionRecibida->operation  = %d\n", cursor, cantidadParametros );
  
  for (size_t i = 0; i < cantidadParametros; i++)
  {
    char* parametro = string_new();
    parametro = string_duplicate(list_get(propiedadesPlanas, ++cursor));
    DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: parametro = %s \n", cursor, parametro);
    list_add(instruccionRecibida->parameters, parametro);
  }
  
  list_destroy_and_destroy_elements(propiedadesPlanas, &free);

  DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_instruccion( ) [END]\n");

  return instruccionRecibida;
}

void instruction_delete(t_instruction_use *lineaInstruccion) {
  
    if (lineaInstruccion->parameters != NULL) {
        list_iterate(lineaInstruccion->parameters, free_string_element);
        list_destroy(lineaInstruccion->parameters);
    }
  free(lineaInstruccion);
}

/*
void message_send(enum HeaderCode headerCode, char* message, int fd_socket) {
  Package *package = package_create(headerCode);
  package_add(package, message, strlen(message) + 1);
  package_send(package, fd_socket);
  package_destroy(package);
}

char* message_receive(int fd_socket) {
  int size;
  char* message = buffer_receive(&size, fd_socket);
  return message;
}
*/

/*
t_list* get_package_like_list(int fd_client) {
  int buffer_size;
  int tamanioContenido;
  int offset = 0;

  t_list *contenido = list_create();
  void *buffer = buffer_receive(&buffer_size, fd_client);

  while (offset < buffer_size)
  {
    memcpy((void*) &tamanioContenido, (void*)(((uint8_t*) buffer) + offset), sizeof(int));
    offset += sizeof(int);

    void *valor = malloc(tamanioContenido);
    memcpy(valor, (void*)(((uint8_t*) buffer) + offset), tamanioContenido);
    offset += tamanioContenido;

    list_add(contenido, valor);
  }

  free(buffer);
  return contenido;
}
*/

void free_string_element(void *element) {
    free((char *) element);
}