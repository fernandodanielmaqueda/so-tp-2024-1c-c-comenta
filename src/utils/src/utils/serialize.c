/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "serialize.h"

void message_send(enum HeaderCode headerCode, char* message, int fd_socket) {
  Package *package = package_create(headerCode);
  package_add(package, message, strlen(message) + 1);
  package_send(package, fd_socket);
  package_destroy(package);
}

/*  LIBERA ESPACIO
void kill_pcb(t_pcb *pcbObjetivo)
{
  if ( != NULL)
  {
    if ( != NULL)
      list_destroy_and_destroy_elements(, (void *)delete_instruction);

    free();
  }
}
//TODO: BRAI CONTINUAR  ESTA FUNCION  ====================================//////////////
*/

void serialize_pcb(Package *package, t_pcb *pcb) {
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_pcb( ) [...]\n"); }
  
  int cursor = 0;
  
  package_add(package, &(pcb->pid), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: pid = %d\n", cursor, pcb->pid); }
  cursor++;
  package_add(package, &(pcb->pc), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: pc = %d\n", cursor, pcb->pc); }
  cursor++;

  //REGISTROS
  package_add(package, &(pcb->AX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro AX = %d\n", cursor, pcb->AX); }
  cursor++;
  package_add(package, &(pcb->BX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro BX = %d\n", cursor, pcb->BX); }
  cursor++;
  package_add(package, &(pcb->CX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro CX = %d\n", cursor, pcb->CX); }
  cursor++;
  package_add(package, &(pcb->DX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro DX = %d\n", cursor, pcb->DX); }
  cursor++;
  package_add(package, &(pcb->EAX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EAX = %d\n", cursor, pcb->EAX); }
  cursor++;
  package_add(package, &(pcb->EBX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EBX = %d\n", cursor, pcb->EBX); }
  cursor++;
  package_add(package, &(pcb->ECX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro ECX = %d\n", cursor, pcb->ECX); }
  cursor++;
  package_add(package, &(pcb->EDX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EDX = %d\n", cursor, pcb->EDX); }
  cursor++;
  package_add(package, &(pcb->RAX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RAX = %d\n", cursor, pcb->RAX); }
  cursor++;
  package_add(package, &(pcb->RBX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RBX = %d\n", cursor, pcb->RBX); }
  cursor++;
  package_add(package, &(pcb->RCX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RCX = %d\n", cursor, pcb->RCX); }
  cursor++;
  package_add(package, &(pcb->RDX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RDX = %d\n", cursor, pcb->RDX); }
  cursor++;
  package_add(package, &(pcb->SI), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro SI = %d\n", cursor, pcb->SI); }
  cursor++;
  package_add(package, &(pcb->DI), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro DI = %d\n", cursor, pcb->DI); }
  cursor++;

  package_add(package, &(pcb->quantum), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: quantum = %d\n", cursor, pcb->quantum); }
  cursor++;
  package_add(package, &(pcb->arrival_READY), sizeof(double));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: arrival_READY = %g\n", cursor, pcb->arrival_READY); }
  cursor++;
  package_add(package, &(pcb->arrival_RUNNING), sizeof(double));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: arrival_RUNNING = %g\n", cursor, pcb->arrival_RUNNING); }
  cursor++;
  package_add(package, &(pcb->current_state), sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: current_state = %d\n", cursor, pcb->current_state); }
  cursor++;
  //package_add(package, &(pcb->fd_conexion), sizeof(int));
  //{ DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: fd_conexion = %d\n", cursor, pcb->fd_conexion); }
  //cursor++;

/*
  //INSTRUCCIONES
  package_add(package, &cantidadInstrucciones, sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Cantidad de Instrucciones = %d\n", cursor, cantidadInstrucciones); }
  cursor++;

  t_instruccion *lineaInstruccion;
  for (int i = 0; i < cantidadInstrucciones; i++)
  {
    lineaInstruccion = list_get(pcb->instrucciones, i);

    package_add(package, lineaInstruccion->id, strlen(lineaInstruccion->id) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Identificador instruccion = %s\n", cursor, lineaInstruccion->id); }
    cursor++;
    package_add(package, lineaInstruccion->param1, strlen(lineaInstruccion->param1) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Param1 = %d\n", cursor, lineaInstruccion->param1); }
    cursor++;
    package_add(package, lineaInstruccion->param2, strlen(lineaInstruccion->param2) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Param2 = %d\n", cursor, lineaInstruccion->param2); }
    cursor++;
    package_add(package, lineaInstruccion->param3, strlen(lineaInstruccion->param3) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Param3 = %d\n", cursor, lineaInstruccion->param3); }
    cursor++;
    package_add(package, lineaInstruccion->param4, strlen(lineaInstruccion->param4) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Param4 = %d\n", cursor, lineaInstruccion->param4); }
    cursor++;
    package_add(package, lineaInstruccion->param5, strlen(lineaInstruccion->param5) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Param5 = %d\n", cursor, lineaInstruccion->param5); }
    cursor++;
  }
*/
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_pcb( ) [END]\n"); }
}


t_pcb *deserialize_pcb(int socketCliente) {
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_pcb( ) [...]\n"); }

  t_list *lista_elememtos = get_package_like_list(socketCliente);
  t_pcb *pcb = malloc(sizeof(t_pcb));
  int cursor = 0;

  pcb->pid = *(uint32_t *)list_get(lista_elememtos, cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->pid = %d\n", cursor, pcb->pid); }
  pcb->pc = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->pc = %d\n", cursor, pcb->pc); }

  //REGISTROS
  pcb->AX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->AX = %d\n", cursor, pcb->AX); }
  pcb->BX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->BX = %d\n", cursor, pcb->BX); }
  pcb->CX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->CX = %d\n", cursor, pcb->CX); }
  pcb->DX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->DX = %d\n", cursor, pcb->DX); }
  pcb->EAX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->EAX = %d\n", cursor, pcb->EAX); }
  pcb->EBX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->EBX = %d\n", cursor, pcb->EBX); }
  pcb->ECX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->ECX = %d\n", cursor, pcb->ECX); }
  pcb->EDX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->EDX = %d\n", cursor, pcb->EDX); }
  pcb->RAX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->RAX = %d\n", cursor, pcb->RAX); }
  pcb->RBX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->RBX = %d\n", cursor, pcb->RBX); }
  pcb->RCX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->RCX = %d\n", cursor, pcb->RCX); }
  pcb->RDX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->RDX = %d\n", cursor, pcb->RDX); }
  pcb->SI = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->SI = %d\n", cursor, pcb->SI); }
  pcb->DI = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->DI = %d\n", cursor, pcb->DI); }

  pcb->quantum = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->quantum = %d\n", cursor, pcb->quantum); }
  pcb->arrival_READY = *(double *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->arrival_READY = %g\n", cursor, pcb->arrival_READY); }
  pcb->arrival_RUNNING = *(double *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->arrival_RUNNING = %g\n", cursor, pcb->arrival_RUNNING); }

  pcb->current_state = *(int *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->current_state = %d\n", cursor, pcb->current_state); }
  //pcb->fd_conexion = *(int *)list_get(lista_elememtos, ++cursor);
  //{ DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->fd_conexion = %d\n", cursor, pcb->fd_conexion); }

/*
  //INSTRUCCIONES
  int cantidadInstrucciones =*(int *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: cantidadInstrucciones = %d\n", cursor, cantidadInstrucciones); }
  int offset = cursor;

  while (cursor < cantidadInstrucciones * 6 + offset)
  {
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    instruccion->id = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: id = %s \n", cursor, instruccion->id); }
    instruccion->param1 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: param1 = %d \n", cursor, instruccion->param1); }
    instruccion->param2 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: param2 = %d \n", cursor, instruccion->param2); }
    instruccion->param3 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: param3 = %d \n", cursor, instruccion->param3); }
    instruccion->param4 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: param4 = %d \n", cursor, instruccion->param4); }
    instruccion->param5 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: param5 = %d \n", cursor, instruccion->param5); }

    list_add(pcb->instrucciones, instruccion);
  }
*/

  list_destroy_and_destroy_elements(lista_elememtos, &free);

  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_pcb( ) [END]\n"); }

  return pcb;
}

void send_pcb_to(t_pcb* pcbEnviado, int socket){
  Package* pack = package_create(PCB);
  serialize_pcb(pack, pcbEnviado);
  package_send(pack, socket);
  package_destroy(pack);
}


void instruction_delete(t_instruction_use *lineaInstruccion) {
  
    if (lineaInstruccion->parameters != NULL) {
        list_iterate(lineaInstruccion->parameters, free_string_element);
        list_destroy(lineaInstruccion->parameters);
    }
  free(lineaInstruccion);
}

void send_instruccion(t_instruction_use* instruccion, int socket) {
  
  Package* package = package_create(INSTUCTION_REQUEST);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_instruccion( ) [...]\n"); }
  
  int cursor = 0;
  int cantidad_parametros= list_size(instruccion->parameters);


    package_add(package, &instruccion->operation, sizeof(enum HeaderCode));
    DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Identificador instruccion = %d\n", cursor, instruccion->operation);
    cursor++;

  //PARAMETROS
  package_add(package, &cantidad_parametros, sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Cantidad de parametros = %d\n", cursor, cantidad_parametros); }
  cursor++;

  char* parametro;
  for (int i = 0; i < cantidad_parametros; i++)
  {
    parametro = list_get(instruccion->parameters, i);

    package_add(package, parametro, strlen(parametro) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Parametro = %s\n", cursor, parametro); }
    cursor++;
  }

  DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_instruccion( ) [END]\n");

  package_send(package, socket);

  package_destroy(package);
}

t_instruction_use* receive_instruccion(int socket) {
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_instruccion( ) [...]\n"); }

  t_list *propiedadesPlanas = get_package_like_list(socket);
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

// Alternativa iña
void serialize_pcb_2(Package* package, t_pcb* pcb) {
    // Calcular el tamaño total necesario para el payload
    uint32_t payload_size = sizeof(t_pcb);

    // Reservar memoria para el payload
    package->payload = malloc(sizeof(Payload));
    package->payload->size = payload_size;
    package->payload->stream = malloc(payload_size);

    // Copiar los datos de la estructura t_pcb al payload
    uint32_t offset = 0;
    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->pid, sizeof(pcb->pid));
    offset += sizeof(pcb->pid);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->pc, sizeof(pcb->pc));
    offset += sizeof(pcb->pc);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->AX, sizeof(pcb->AX));
    offset += sizeof(pcb->AX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->BX, sizeof(pcb->BX));
    offset += sizeof(pcb->BX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->CX, sizeof(pcb->CX));
    offset += sizeof(pcb->CX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->DX, sizeof(pcb->DX));
    offset += sizeof(pcb->DX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->EAX, sizeof(pcb->EAX));
    offset += sizeof(pcb->EAX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->EBX, sizeof(pcb->EBX));
    offset += sizeof(pcb->EBX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->ECX, sizeof(pcb->ECX));
    offset += sizeof(pcb->ECX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->EDX, sizeof(pcb->EDX));
    offset += sizeof(pcb->EDX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->RAX, sizeof(pcb->RAX));
    offset += sizeof(pcb->RAX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->RBX, sizeof(pcb->RBX));
    offset += sizeof(pcb->RBX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->RCX, sizeof(pcb->RCX));
    offset += sizeof(pcb->RCX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->RDX, sizeof(pcb->RDX));
    offset += sizeof(pcb->RDX);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->SI, sizeof(pcb->SI));
    offset += sizeof(pcb->SI);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->DI, sizeof(pcb->DI));
    offset += sizeof(pcb->DI);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->quantum, sizeof(pcb->quantum));
    offset += sizeof(pcb->quantum);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->current_state, sizeof(pcb->current_state));
    offset += sizeof(pcb->current_state);

    //memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->fd_conexion, sizeof(pcb->fd_conexion));
    //offset += sizeof(pcb->fd_conexion);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->arrival_READY, sizeof(pcb->arrival_READY));
    offset += sizeof(pcb->arrival_READY);

    memcpy((void*)(((uint8_t*) package->payload->stream) + offset), &pcb->arrival_RUNNING, sizeof(pcb->arrival_RUNNING));
    offset += sizeof(pcb->arrival_RUNNING);

    // Establecer el código de operación del package, si es necesario
    package->header = 1;  // O el valor que necesites
}

void send_pcb(int socket, t_pcb* pcb) {
    Package package;
    serialize_pcb_2(&package, pcb);

    uint32_t total_size = sizeof(package.header) + sizeof(package.payload->size) + package.payload->size;
    void* payload = malloc(total_size);

    uint32_t offset = 0;
    memcpy((void*)(((uint8_t*) payload) + offset), &package.header, sizeof(package.header));
    offset += sizeof(package.header);

    memcpy((void*)(((uint8_t*) payload) + offset), &package.payload->size, sizeof(package.payload->size));
    offset += sizeof(package.payload->size);

    memcpy((void*)(((uint8_t*) payload) + offset), package.payload->stream, package.payload->size);

    send(socket, payload, total_size, 0);

    free(payload);
    //free_package(&package);
}

// Alternativa iña
void deserialize_pcb_2(t_pcb* pcb, void* stream) {
    uint32_t offset = 0;

    memcpy((void*) &pcb->pid, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->pid));
    offset += sizeof(pcb->pid);

    memcpy((void*) &pcb->pc, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->pc));
    offset += sizeof(pcb->pc);

    memcpy((void*) &pcb->AX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->AX));
    offset += sizeof(pcb->AX);

    memcpy((void*) &pcb->BX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->BX));
    offset += sizeof(pcb->BX);

    memcpy((void*) &pcb->CX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->CX));
    offset += sizeof(pcb->CX);

    memcpy((void*) &pcb->DX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->DX));
    offset += sizeof(pcb->DX);

    memcpy((void*) &pcb->EAX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->EAX));
    offset += sizeof(pcb->EAX);

    memcpy((void*) &pcb->EBX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->EBX));
    offset += sizeof(pcb->EBX);

    memcpy((void*) &pcb->ECX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->ECX));
    offset += sizeof(pcb->ECX);

    memcpy((void*) &pcb->EDX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->EDX));
    offset += sizeof(pcb->EDX);

    memcpy((void*) &pcb->RAX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->RAX));
    offset += sizeof(pcb->RAX);

    memcpy((void*) &pcb->RBX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->RBX));
    offset += sizeof(pcb->RBX);

    memcpy((void*) &pcb->RCX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->RCX));
    offset += sizeof(pcb->RCX);

    memcpy((void*) &pcb->RDX, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->RDX));
    offset += sizeof(pcb->RDX);

    memcpy((void*) &pcb->SI, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->SI));
    offset += sizeof(pcb->SI);

    memcpy((void*) &pcb->DI, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->DI));
    offset += sizeof(pcb->DI);

    memcpy((void*) &pcb->quantum, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->quantum));
    offset += sizeof(pcb->quantum);

    memcpy((void*) &pcb->current_state, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->current_state));
    offset += sizeof(pcb->current_state);

    //memcpy((void*) &pcb->fd_conexion, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->fd_conexion));
    //offset += sizeof(pcb->fd_conexion);

    memcpy((void*) &pcb->arrival_READY, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->arrival_READY));
    offset += sizeof(pcb->arrival_READY);

    memcpy((void*) &pcb->arrival_RUNNING, (void*)(((uint8_t*) stream) + offset), sizeof(pcb->arrival_RUNNING));
    offset += sizeof(pcb->arrival_RUNNING);
}

void receive_pcb(int socket, t_pcb *pcb) {
    uint8_t header;
    uint32_t payload_size;

    // Recibir el código de operación
    recv(socket, &header, sizeof(header), 0);

    // Recibir el tamaño del payload
    recv(socket, &payload_size, sizeof(payload_size), 0);

    // Reservar memoria para el payload
    void *payload = malloc(payload_size);

    // Recibir el payload
    recv(socket, payload, payload_size, 0);

    // Deserializar el payload en la estructura t_pcb
    deserialize_pcb_2(pcb, payload);

    // Liberar la memoria del payload
    free(payload);
}