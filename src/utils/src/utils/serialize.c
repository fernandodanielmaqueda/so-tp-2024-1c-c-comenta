/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "serialize.h"

void message_send(enum HeaderCode headerCode, char* message, int fd_socket) {
  Package *package = package_create(headerCode);
  package_add(package, message, strlen(message) + 1);
  package_send(package, fd_socket);
  package_destroy(package);
}

enum HeaderCode receive_headerCode(int fd_socket) {

  enum HeaderCode headerCode;

  if (recv(fd_socket, &headerCode, sizeof(int), MSG_WAITALL) > 0) return headerCode;
  else {
    close(fd_socket);
    return DISCONNECTED;
  }
}

Package *package_create(uint8_t header) {

  Package *package = malloc(sizeof(Package));

  package->header = header;
  package_buffer_create(package);

  return package;
}

void package_add(Package *package, void *value, size_t size) {
  package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + size + sizeof(int));
  memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
  memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, size);
  package->buffer->size += size + sizeof(int);
}

void package_destroy(Package *package) {
  if (package != NULL) {
    if (package->buffer != NULL) {
      free(package->buffer->stream);
      free(package->buffer);
    }
    free(package);
  }
}

//

void package_buffer_create(Package *package) {
  package->buffer = malloc(sizeof(Buffer));
  package->buffer->size = 0;
  package->buffer->stream = NULL;
}

void *buffer_get(int *size, int fd_client) {
  void *buffer;

  recv(fd_client, size, sizeof(int), MSG_WAITALL);
  buffer = malloc(*size);
  recv(fd_client, buffer, *size, MSG_WAITALL);

  return buffer;
}

//

t_list* get_package_like_list(int fd_client) {
  int buffer_size;
  int tamanioContenido;
  int offset = 0;

  t_list *contenido = list_create();
  void *buffer = buffer_get(&buffer_size, fd_client);

  while (offset < buffer_size)
  {
    memcpy(&tamanioContenido, buffer + offset, sizeof(int));
    offset += sizeof(int);

    void *valor = malloc(tamanioContenido);
    memcpy(valor, buffer + offset, tamanioContenido);
    offset += tamanioContenido;

    list_add(contenido, valor);
  }

  free(buffer);
  return contenido;
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

void instruction_delete(t_instruccion_use *lineaInstruccion) {
  if (lineaInstruccion != NULL) free(lineaInstruccion);
}
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
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro AX = %s\n", cursor, pcb->AX); }
  cursor++;
  package_add(package, &(pcb->BX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro BX = %s\n", cursor, pcb->BX); }
  cursor++;
  package_add(package, &(pcb->CX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro CX = %s\n", cursor, pcb->CX); }
  cursor++;
  package_add(package, &(pcb->DX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro DX = %s\n", cursor, pcb->DX); }
  cursor++;
  package_add(package, &(pcb->EAX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EAX = %s\n", cursor, pcb->EAX); }
  cursor++;
  package_add(package, &(pcb->EBX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EBX = %s\n", cursor, pcb->EBX); }
  cursor++;
  package_add(package, &(pcb->ECX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro ECX = %s\n", cursor, pcb->ECX); }
  cursor++;
  package_add(package, &(pcb->EDX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EDX = %s\n", cursor, pcb->EDX); }
  cursor++;
  package_add(package, &(pcb->RAX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RAX = %s\n", cursor, pcb->RAX); }
  cursor++;
  package_add(package, &(pcb->RBX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RBX = %s\n", cursor, pcb->RBX); }
  cursor++;
  package_add(package, &(pcb->RCX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RCX = %s\n", cursor, pcb->RCX); }
  cursor++;
  package_add(package, &(pcb->RDX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RDX = %s\n", cursor, pcb->RDX); }
  cursor++;
  package_add(package, &(pcb->SI), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro SI = %s\n", cursor, pcb->SI); }
  cursor++;
  package_add(package, &(pcb->DI), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro DI = %s\n", cursor, pcb->DI); }
  cursor++;

  /*CASO CHAR*
  package_add(package, &(pcb->AX), strlen(&(pcb->AX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro AX = %s\n", cursor, pcb->AX); }
  cursor++;
  package_add(package, &(pcb->BX), strlen(&(pcb->BX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro BX = %s\n", cursor, pcb->BX); }
  cursor++;
  package_add(package, &(pcb->CX), strlen(&(pcb->CX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro CX = %s\n", cursor, pcb->CX); }
  cursor++;
  package_add(package, &(pcb->DX), strlen(&(pcb->DX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro DX = %s\n", cursor, pcb->DX); }
  cursor++;
  package_add(package, &(pcb->EAX), strlen(&(pcb->EAX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EAX = %s\n", cursor, pcb->EAX); }
  cursor++;
  package_add(package, &(pcb->EBX), strlen(&(pcb->EBX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EBX = %s\n", cursor, pcb->EBX); }
  cursor++;
  package_add(package, &(pcb->ECX), strlen(&(pcb->ECX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro ECX = %s\n", cursor, pcb->ECX); }
  cursor++;
  package_add(package, &(pcb->EDX), strlen(&(pcb->EDX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro EDX = %s\n", cursor, pcb->EDX); }
  cursor++;
  package_add(package, &(pcb->RAX), strlen(&(pcb->RAX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RAX = %s\n", cursor, pcb->RAX); }
  cursor++;
  package_add(package, &(pcb->RBX), strlen(&(pcb->RBX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RBX = %s\n", cursor, pcb->RBX); }
  cursor++;
  package_add(package, &(pcb->RCX), strlen(&(pcb->RCX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RCX = %s\n", cursor, pcb->RCX); }
  cursor++;
  package_add(package, &(pcb->RDX), strlen(&(pcb->RDX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: Registro RDX = %s\n", cursor, pcb->RDX); }
  cursor++;
*/

  package_add(package, &(pcb->quantum), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: quantum = %d\n", cursor, pcb->quantum); }
  cursor++;
  package_add(package, &(pcb->arrival_READY), sizeof(double));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: arrival_READY = %d\n", cursor, pcb->arrival_READY); }
  cursor++;
  package_add(package, &(pcb->arrival_RUNNING), sizeof(double));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] package[%d]: arrival_RUNNING = %d\n", cursor, pcb->arrival_RUNNING); }
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
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->arrival_READY = %d\n", cursor, pcb->arrival_READY); }
  pcb->arrival_RUNNING = *(double *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] package[%d]: pcb->arrival_RUNNING = %d\n", cursor, pcb->arrival_RUNNING); }

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

void package_send(Package* package, int fd_receiver) {
  int bytes = package->buffer->size + 2 * sizeof(int);
  void *aEnviar = package_serialize(package, bytes);

  send(fd_receiver, aEnviar, bytes, 0);
  free(aEnviar);
}

void *package_serialize(Package *package, int bytes) {
  void *package_void = malloc(bytes);
  int offset = 0;

  memcpy(package_void + offset, &(package->header), sizeof(int));
  offset += sizeof(int);
  memcpy(package_void + offset, &(package->buffer->size), sizeof(int));
  offset += sizeof(int);
  memcpy(package_void + offset, package->buffer->stream, package->buffer->size);

  return package_void;
}


void send_instruccion(t_instruction_use* instruccion, int socket) {
  
  Package* package = package_create(INSTUCTION_REQUEST);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_instruccion( ) [...]\n"); }
  
  int cursor = 0;
  int cantidad_parametros= list_size(instruccion->parameters);


    package_add(package, &instruccion->operation, sizeof(enum HeaderCode));
    //DEBUG_PRINTF("\n[Serializar] package[%d]: Identificador instruccion = %d\n", cursor, instruccion->operation);
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

  //DEBUG_PRINTF("\n[Serializar] serializar_instruccion( ) [END]\n");

  package_send(package, socket);

  package_destroy(package);
}

t_instruction_use* receive_instruccion(int socket) {
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_instruccion( ) [...]\n"); }

  t_list *propiedadesPlanas = get_package_like_list(socket);
  t_instruction_use* instruccionRecibida = malloc(sizeof(t_instruction_use));
  int cursor = 0;

  instruccionRecibida->operation = *(enum HeaderCode*)list_get(propiedadesPlanas, cursor);
  //DEBUG_PRINTF("\n[Deserializar] package[%d]: instruccionRecibida->operation  = %d\n", cursor, instruccionRecibida->operation );
  
  int cantidadParametros = *(int*)list_get(propiedadesPlanas, cursor);
  //DEBUG_PRINTF("\n[Deserializar] package[%d]: instruccionRecibida->operation  = %d\n", cursor, cantidadParametros );
  
  for (size_t i = 0; i < cantidadParametros; i++)
  {
    char* parametro = string_new();
    parametro = string_duplicate(list_get(propiedadesPlanas, ++cursor));
    //DEBUG_PRINTF("\n[Deserializar] package[%d]: parametro = %s \n", cursor, parametro);
    list_add(instruccionRecibida->parameters, parametro);
  }
  
  list_destroy_and_destroy_elements(propiedadesPlanas, &free);

  //DEBUG_PRINTF("\n[Deserializar] deserializar_instruccion( ) [END]\n");

  return instruccionRecibida;
}

// Alternativa iña
void serialize_pcb_2(Package* package, t_pcb* pcb) {
    // Calcular el tamaño total necesario para el buffer
    uint32_t buffer_size = sizeof(t_pcb);

    // Reservar memoria para el buffer
    package->buffer = malloc(sizeof(Buffer));
    package->buffer->size = buffer_size;
    package->buffer->stream = malloc(buffer_size);

    // Copiar los datos de la estructura t_pcb al buffer
    uint32_t offset = 0;
    memcpy(package->buffer->stream + offset, &pcb->pid, sizeof(pcb->pid));
    offset += sizeof(pcb->pid);

    memcpy(package->buffer->stream + offset, &pcb->pc, sizeof(pcb->pc));
    offset += sizeof(pcb->pc);

    memcpy(package->buffer->stream + offset, &pcb->AX, sizeof(pcb->AX));
    offset += sizeof(pcb->AX);

    memcpy(package->buffer->stream + offset, &pcb->BX, sizeof(pcb->BX));
    offset += sizeof(pcb->BX);

    memcpy(package->buffer->stream + offset, &pcb->CX, sizeof(pcb->CX));
    offset += sizeof(pcb->CX);

    memcpy(package->buffer->stream + offset, &pcb->DX, sizeof(pcb->DX));
    offset += sizeof(pcb->DX);

    memcpy(package->buffer->stream + offset, &pcb->EAX, sizeof(pcb->EAX));
    offset += sizeof(pcb->EAX);

    memcpy(package->buffer->stream + offset, &pcb->EBX, sizeof(pcb->EBX));
    offset += sizeof(pcb->EBX);

    memcpy(package->buffer->stream + offset, &pcb->ECX, sizeof(pcb->ECX));
    offset += sizeof(pcb->ECX);

    memcpy(package->buffer->stream + offset, &pcb->EDX, sizeof(pcb->EDX));
    offset += sizeof(pcb->EDX);

    memcpy(package->buffer->stream + offset, &pcb->RAX, sizeof(pcb->RAX));
    offset += sizeof(pcb->RAX);

    memcpy(package->buffer->stream + offset, &pcb->RBX, sizeof(pcb->RBX));
    offset += sizeof(pcb->RBX);

    memcpy(package->buffer->stream + offset, &pcb->RCX, sizeof(pcb->RCX));
    offset += sizeof(pcb->RCX);

    memcpy(package->buffer->stream + offset, &pcb->RDX, sizeof(pcb->RDX));
    offset += sizeof(pcb->RDX);

    memcpy(package->buffer->stream + offset, &pcb->SI, sizeof(pcb->SI));
    offset += sizeof(pcb->SI);

    memcpy(package->buffer->stream + offset, &pcb->DI, sizeof(pcb->DI));
    offset += sizeof(pcb->DI);

    memcpy(package->buffer->stream + offset, &pcb->quantum, sizeof(pcb->quantum));
    offset += sizeof(pcb->quantum);

    memcpy(package->buffer->stream + offset, &pcb->current_state, sizeof(pcb->current_state));
    offset += sizeof(pcb->current_state);

    //memcpy(package->buffer->stream + offset, &pcb->fd_conexion, sizeof(pcb->fd_conexion));
    //offset += sizeof(pcb->fd_conexion);

    memcpy(package->buffer->stream + offset, &pcb->arrival_READY, sizeof(pcb->arrival_READY));
    offset += sizeof(pcb->arrival_READY);

    memcpy(package->buffer->stream + offset, &pcb->arrival_RUNNING, sizeof(pcb->arrival_RUNNING));
    offset += sizeof(pcb->arrival_RUNNING);

    // Establecer el código de operación del package, si es necesario
    package->header = 1;  // O el valor que necesites
}

// Función de ejemplo para liberar la memoria asignada
void free_package(Package* package) {
    if (package->buffer != NULL) {
        if (package->buffer->stream != NULL) {
            free(package->buffer->stream);
        }
        free(package->buffer);
    }
}

void send_pcb(int socket, t_pcb* pcb) {
    Package package;
    serialize_pcb_2(&package, pcb);

    uint32_t total_size = sizeof(package.header) + sizeof(package.buffer->size) + package.buffer->size;
    void* buffer = malloc(total_size);

    uint32_t offset = 0;
    memcpy(buffer + offset, &package.header, sizeof(package.header));
    offset += sizeof(package.header);

    memcpy(buffer + offset, &package.buffer->size, sizeof(package.buffer->size));
    offset += sizeof(package.buffer->size);

    memcpy(buffer + offset, package.buffer->stream, package.buffer->size);

    send(socket, buffer, total_size, 0);

    free(buffer);
    free_package(&package);
}

// Alternativa iña
void deserialize_pcb_2(t_pcb* pcb, void* stream) {
    uint32_t offset = 0;

    memcpy(&pcb->pid, stream + offset, sizeof(pcb->pid));
    offset += sizeof(pcb->pid);

    memcpy(&pcb->pc, stream + offset, sizeof(pcb->pc));
    offset += sizeof(pcb->pc);

    memcpy(&pcb->AX, stream + offset, sizeof(pcb->AX));
    offset += sizeof(pcb->AX);

    memcpy(&pcb->BX, stream + offset, sizeof(pcb->BX));
    offset += sizeof(pcb->BX);

    memcpy(&pcb->CX, stream + offset, sizeof(pcb->CX));
    offset += sizeof(pcb->CX);

    memcpy(&pcb->DX, stream + offset, sizeof(pcb->DX));
    offset += sizeof(pcb->DX);

    memcpy(&pcb->EAX, stream + offset, sizeof(pcb->EAX));
    offset += sizeof(pcb->EAX);

    memcpy(&pcb->EBX, stream + offset, sizeof(pcb->EBX));
    offset += sizeof(pcb->EBX);

    memcpy(&pcb->ECX, stream + offset, sizeof(pcb->ECX));
    offset += sizeof(pcb->ECX);

    memcpy(&pcb->EDX, stream + offset, sizeof(pcb->EDX));
    offset += sizeof(pcb->EDX);

    memcpy(&pcb->RAX, stream + offset, sizeof(pcb->RAX));
    offset += sizeof(pcb->RAX);

    memcpy(&pcb->RBX, stream + offset, sizeof(pcb->RBX));
    offset += sizeof(pcb->RBX);

    memcpy(&pcb->RCX, stream + offset, sizeof(pcb->RCX));
    offset += sizeof(pcb->RCX);

    memcpy(&pcb->RDX, stream + offset, sizeof(pcb->RDX));
    offset += sizeof(pcb->RDX);

    memcpy(&pcb->SI, stream + offset, sizeof(pcb->SI));
    offset += sizeof(pcb->SI);

    memcpy(&pcb->DI, stream + offset, sizeof(pcb->DI));
    offset += sizeof(pcb->DI);

    memcpy(&pcb->quantum, stream + offset, sizeof(pcb->quantum));
    offset += sizeof(pcb->quantum);

    memcpy(&pcb->current_state, stream + offset, sizeof(pcb->current_state));
    offset += sizeof(pcb->current_state);

    //memcpy(&pcb->fd_conexion, stream + offset, sizeof(pcb->fd_conexion));
    //offset += sizeof(pcb->fd_conexion);

    memcpy(&pcb->arrival_READY, stream + offset, sizeof(pcb->arrival_READY));
    offset += sizeof(pcb->arrival_READY);

    memcpy(&pcb->arrival_RUNNING, stream + offset, sizeof(pcb->arrival_RUNNING));
    offset += sizeof(pcb->arrival_RUNNING);
}

void receive_pcb(int socket, t_pcb *pcb) {
    uint8_t header;
    uint32_t buffer_size;

    // Recibir el código de operación
    recv(socket, &header, sizeof(header), 0);

    // Recibir el tamaño del buffer
    recv(socket, &buffer_size, sizeof(buffer_size), 0);

    // Reservar memoria para el buffer
    void *buffer = malloc(buffer_size);

    // Recibir el buffer
    recv(socket, buffer, buffer_size, 0);

    // Deserializar el buffer en la estructura t_pcb
    deserialize_pcb_2(pcb, buffer);

    // Liberar la memoria del buffer
    free(buffer);
}