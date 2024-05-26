/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

const char *PORT_NAMES[PortType_Count] = {[KERNEL_TYPE] = "Kernel", [CPU_TYPE] = "CPU", [CPU_DISPATCH_TYPE] = "CPU (Dispatch)", [CPU_INTERRUPT_TYPE] = "CPU (Interrupt)", [MEMORY_TYPE] = "Memoria", [IO_TYPE] = "Entrada/Salida", [TO_BE_DEFINED_TYPE] = "A identificar"};
//
const int32_t HANDSHAKES[PortType_Count] = {[KERNEL_TYPE] = 10, [CPU_TYPE] = 20, [CPU_DISPATCH_TYPE] = 21, [CPU_INTERRUPT_TYPE] = 22, [MEMORY_TYPE] = 30, [IO_TYPE] = 40, [UNIDENTIFIED_TYPE] = -1};
void *client_thread_connect_to_server(void *connection_parameter) {
  Connection *connection = (Connection*) connection_parameter;

  size_t bytes;

  int32_t handshake = (int32_t) connection->client_type;
  int32_t result;

  while(1) {
    while(1) {
      log_info(connections_logger, "Intentando conectar con [Servidor] %s en IP: %s - Puerto: %s...", PORT_NAMES[connection->server_type], connection->ip, connection->port);
      connection->fd_connection = client_start_try(connection->ip, connection->port);

      if(connection->fd_connection != -1) break;
      else {
        log_warning(connections_logger, "No se pudo conectar con [Servidor] %s en IP: %s - Puerto: %s. Reintentando en %d segundos...", PORT_NAMES[connection->server_type], connection->ip, connection->port, RETRY_CONNECTION_IN_SECONDS);
        sleep(RETRY_CONNECTION_IN_SECONDS);
      }
    }

    log_info(connections_logger, "Conectado con [Servidor] %s en IP: %s - Puerto: %s", PORT_NAMES[connection->server_type], connection->ip, connection->port);

    // Handshake

    bytes = send(connection->fd_connection, &handshake, sizeof(int32_t), 0);
    bytes = recv(connection->fd_connection, &result, sizeof(int32_t), MSG_WAITALL);

    if(result == 0) break;
    else {
      close(connection->fd_connection);
      log_warning(connections_logger, "Error Handshake con [Servidor] %s en IP: %s - Puerto: %s. Reintentando en %d segundos...", PORT_NAMES[connection->server_type], connection->ip, connection->port, RETRY_CONNECTION_IN_SECONDS);
      sleep(RETRY_CONNECTION_IN_SECONDS);
    }
  }

  return NULL;
}

int client_start_try(char *ip, char *port) {

	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET para IPv4 unicamente
	hints.ai_socktype = SOCK_STREAM;

	int exit_code = getaddrinfo(ip, port, &hints, &result);
  if (exit_code != 0) {
    log_warning(connections_logger, "Funcion getaddrinfo: %s\n", gai_strerror(exit_code));
    return -1;
  }

	// Ahora vamos a crear el socket.
	int fd_client;

  for(rp = result ; rp != NULL ; rp = rp->ai_next) {
    fd_client = socket(
      rp->ai_family,
      rp->ai_socktype,
      rp->ai_protocol
    );

    if(fd_client == -1) {
      log_warning(connections_logger, "Funcion socket: %s\n", strerror(errno));
      continue; /* This one failed */
    }

    if(connect(fd_client, rp->ai_addr, rp->ai_addrlen) == 0) break; /* Until one succeeds */
    else {
      log_warning(connections_logger, "Funcion connect: %s\n", strerror(errno));
    }

    close(fd_client);
  }
	
  freeaddrinfo(result); /* No longer needed */

  if (rp == NULL) { /* No address succeeded */
    return -1;
  }
	
  return fd_client;
}

void server_start(Server *server) {

  while(1) {
    log_info(connections_logger, "Intentando iniciar [Servidor] %s en Puerto: %s...", PORT_NAMES[server->server_type], server->port);
    server->fd_listen = server_start_try(server->port);

    if(server->fd_listen != -1) break;
    else {
      log_warning(connections_logger, "No se pudo iniciar [Servidor] %s en Puerto: %s. Reintentando en %d segundos...", PORT_NAMES[server->server_type], server->port, RETRY_CONNECTION_IN_SECONDS);
      sleep(RETRY_CONNECTION_IN_SECONDS);
    }
  }

  log_info(connections_logger, "Escuchando [Servidor] %s en Puerto: %s", PORT_NAMES[server->server_type], server->port);

}

int server_start_try(char* port) {

	struct addrinfo hints;
  struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET para IPv4 unicamente
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int exit_code = getaddrinfo(NULL, port, &hints, &result);
  if (exit_code != 0) {
    log_warning(connections_logger, "Funcion getaddrinfo: %s\n", gai_strerror(exit_code));
    return -1;
  }

  int fd_server;

  for(rp = result ; rp != NULL ; rp = rp->ai_next) {
    fd_server = socket(
      rp->ai_family,
      rp->ai_socktype,
      rp->ai_protocol
    );

    if(fd_server == -1) {
      log_warning(connections_logger, "Funcion socket: %s\n", strerror(errno));
      continue; /* This one failed */
    }

    if(bind(fd_server, rp->ai_addr, rp->ai_addrlen) == 0) break; /* Until one succeeds */
    else {
      log_warning(connections_logger, "Funcion bind: %s\n", strerror(errno));
      continue;
    }

    return fd_server;
  }
	
  freeaddrinfo(result); /* No longer needed */

  if (rp == NULL) { /* No address succeeded */
    return -1;
  }

	// Escuchamos las conexiones entrantes
	if (listen(fd_server, SOMAXCONN) == -1) {
		log_warning(connections_logger, "Funcion listen: %s\n", strerror(errno));
		return -1;
	}

	return fd_server;
}

int server_accept(int fd_server) {
  // Syscall bloqueante que se queda esperando hasta que llegue un nuevo cliente
	int fd_client = accept(fd_server, NULL, NULL);

	if(fd_client == -1) {
      log_warning(connections_logger, "Funcion accept: %s\n", strerror(errno));
  }

	return fd_client;
}

t_paquete *create_package(uint8_t codigoOperacion) {
  t_paquete *paquete = malloc(sizeof(t_paquete));

  paquete->codigo_operacion = codigoOperacion;
  create_buffer(paquete);

  return paquete;
}


void add_to_package(t_paquete *paquete, void *valor, int tamanio) {
  paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));
  memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
  memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);
  paquete->buffer->size += tamanio + sizeof(int);
}


void kill_package(t_paquete *paquete) {
  if (paquete != NULL)
  {
    if (paquete->buffer != NULL)
    {
      free(paquete->buffer->stream);
      free(paquete->buffer);
    }
    free(paquete);
  }
}


void *get_buffer(int *size, int socketCliente) {
  void *buffer;

  recv(socketCliente, size, sizeof(int), MSG_WAITALL);
  buffer = malloc(*size);
  recv(socketCliente, buffer, *size, MSG_WAITALL);

  return buffer;
}


void create_buffer(t_paquete *paquete) {
  paquete->buffer = malloc(sizeof(t_buffer));
  paquete->buffer->size = 0;
  paquete->buffer->stream = NULL;
}


t_list* get_package_like_list(int socketCliente) {
  int sizeBuffer;
  int tamanioContenido;
  int desplazamiento = 0;

  t_list *contenido = list_create();
  void *buffer = get_buffer(&sizeBuffer, socketCliente);

  while (desplazamiento < sizeBuffer)
  {
    memcpy(&tamanioContenido, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    void *valor = malloc(tamanioContenido);
    memcpy(valor, buffer + desplazamiento, tamanioContenido);
    desplazamiento += tamanioContenido;

    list_add(contenido, valor);
  }

  free(buffer);
  return contenido;
}


void kill_pcb(t_pcb *pcbObjetivo) {
  if (pcbObjetivo != NULL)
  {
    if (pcbObjetivo->instrucciones != NULL)
      list_destroy_and_destroy_elements(pcbObjetivo->instrucciones, (void *)delete_instruction);

    free(pcbObjetivo);
  }
}


void delete_instruction(t_instruccion *lineaInstruccion) {
  if (lineaInstruccion != NULL)
    free(lineaInstruccion);
}

void serialize_pcb(t_paquete *paquete, t_pcb *pcb) {
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_pcb( ) [...]\n"); }

  int cantidadInstrucciones = list_size(pcb->instrucciones);

  int cursor = 0;
  
  add_to_package(paquete, &(pcb->pid), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: pid = %d\n", cursor, pcb->pid); }
  cursor++;
  add_to_package(paquete, &(pcb->pc), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: pc = %d\n", cursor, pcb->pc); }
  cursor++;

  //REGISTROS
  add_to_package(paquete, &(pcb->AX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro AX = %s\n", cursor, pcb->AX); }
  cursor++;
  add_to_package(paquete, &(pcb->BX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro BX = %s\n", cursor, pcb->BX); }
  cursor++;
  add_to_package(paquete, &(pcb->CX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro CX = %s\n", cursor, pcb->CX); }
  cursor++;
  add_to_package(paquete, &(pcb->DX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro DX = %s\n", cursor, pcb->DX); }
  cursor++;
  add_to_package(paquete, &(pcb->EAX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro EAX = %s\n", cursor, pcb->EAX); }
  cursor++;
  add_to_package(paquete, &(pcb->EBX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro EBX = %s\n", cursor, pcb->EBX); }
  cursor++;
  add_to_package(paquete, &(pcb->ECX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro ECX = %s\n", cursor, pcb->ECX); }
  cursor++;
  add_to_package(paquete, &(pcb->EDX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro EDX = %s\n", cursor, pcb->EDX); }
  cursor++;
  add_to_package(paquete, &(pcb->RAX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RAX = %s\n", cursor, pcb->RAX); }
  cursor++;
  add_to_package(paquete, &(pcb->RBX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RBX = %s\n", cursor, pcb->RBX); }
  cursor++;
  add_to_package(paquete, &(pcb->RCX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RCX = %s\n", cursor, pcb->RCX); }
  cursor++;
  add_to_package(paquete, &(pcb->RDX), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RDX = %s\n", cursor, pcb->RDX); }
  cursor++;
  add_to_package(paquete, &(pcb->SI), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro SI = %s\n", cursor, pcb->SI); }
  cursor++;
  add_to_package(paquete, &(pcb->DI), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro DI = %s\n", cursor, pcb->DI); }
  cursor++;

  /*CASO CHAR*
  add_to_package(paquete, &(pcb->AX), strlen(&(pcb->AX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro AX = %s\n", cursor, pcb->AX); }
  cursor++;
  add_to_package(paquete, &(pcb->BX), strlen(&(pcb->BX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro BX = %s\n", cursor, pcb->BX); }
  cursor++;
  add_to_package(paquete, &(pcb->CX), strlen(&(pcb->CX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro CX = %s\n", cursor, pcb->CX); }
  cursor++;
  add_to_package(paquete, &(pcb->DX), strlen(&(pcb->DX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro DX = %s\n", cursor, pcb->DX); }
  cursor++;
  add_to_package(paquete, &(pcb->EAX), strlen(&(pcb->EAX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro EAX = %s\n", cursor, pcb->EAX); }
  cursor++;
  add_to_package(paquete, &(pcb->EBX), strlen(&(pcb->EBX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro EBX = %s\n", cursor, pcb->EBX); }
  cursor++;
  add_to_package(paquete, &(pcb->ECX), strlen(&(pcb->ECX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro ECX = %s\n", cursor, pcb->ECX); }
  cursor++;
  add_to_package(paquete, &(pcb->EDX), strlen(&(pcb->EDX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro EDX = %s\n", cursor, pcb->EDX); }
  cursor++;
  add_to_package(paquete, &(pcb->RAX), strlen(&(pcb->RAX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RAX = %s\n", cursor, pcb->RAX); }
  cursor++;
  add_to_package(paquete, &(pcb->RBX), strlen(&(pcb->RBX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RBX = %s\n", cursor, pcb->RBX); }
  cursor++;
  add_to_package(paquete, &(pcb->RCX), strlen(&(pcb->RCX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RCX = %s\n", cursor, pcb->RCX); }
  cursor++;
  add_to_package(paquete, &(pcb->RDX), strlen(&(pcb->RDX)) + 1);
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Registro RDX = %s\n", cursor, pcb->RDX); }
  cursor++;
*/

  add_to_package(paquete, &(pcb->quantum), sizeof(uint32_t));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: quantum = %d\n", cursor, pcb->quantum); }
  cursor++;
  add_to_package(paquete, &(pcb->llegada_ready), sizeof(double));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: llegada_ready = %d\n", cursor, pcb->llegada_ready); }
  cursor++;
  add_to_package(paquete, &(pcb->llegada_running), sizeof(double));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: llegada_running = %d\n", cursor, pcb->llegada_running); }
  cursor++;
  add_to_package(paquete, &(pcb->estado_actual), sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: estado_actual = %d\n", cursor, pcb->estado_actual); }
  cursor++;
  add_to_package(paquete, &(pcb->fd_conexion), sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: fd_conexion = %d\n", cursor, pcb->fd_conexion); }
  cursor++;


  //INSTRUCCIONES
  add_to_package(paquete, &cantidadInstrucciones, sizeof(int));
  { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Cantidad de Instrucciones = %d\n", cursor, cantidadInstrucciones); }
  cursor++;

  t_instruccion *lineaInstruccion;
  for (int i = 0; i < cantidadInstrucciones; i++)
  {
    lineaInstruccion = list_get(pcb->instrucciones, i);

    add_to_package(paquete, lineaInstruccion->id, strlen(lineaInstruccion->id) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Identificador instruccion = %s\n", cursor, lineaInstruccion->id); }
    cursor++;
    add_to_package(paquete, lineaInstruccion->param1, strlen(lineaInstruccion->param1) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Param1 = %d\n", cursor, lineaInstruccion->param1); }
    cursor++;
    add_to_package(paquete, lineaInstruccion->param2, strlen(lineaInstruccion->param2) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Param2 = %d\n", cursor, lineaInstruccion->param2); }
    cursor++;
    add_to_package(paquete, lineaInstruccion->param3, strlen(lineaInstruccion->param3) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Param3 = %d\n", cursor, lineaInstruccion->param3); }
    cursor++;
    add_to_package(paquete, lineaInstruccion->param4, strlen(lineaInstruccion->param4) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Param4 = %d\n", cursor, lineaInstruccion->param4); }
    cursor++;
    add_to_package(paquete, lineaInstruccion->param5, strlen(lineaInstruccion->param5) + 1);
    { DEBUGGING_SERIALIZATION printf("\n[Serializar] paquete[%d]: Param5 = %d\n", cursor, lineaInstruccion->param5); }
    cursor++;
  }

  { DEBUGGING_SERIALIZATION printf("\n[Serializar] serializar_pcb( ) [END]\n"); }

}


t_pcb *deserializar_pcb(int socketCliente)
{
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_pcb( ) [...]\n"); }

  t_list *lista_elememtos = get_package_like_list(socketCliente);
  t_pcb *pcb = malloc(sizeof(t_pcb));
  pcb->instrucciones = list_create();
  int cursor = 0;

  pcb->pid = *(uint32_t *)list_get(lista_elememtos, cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->pid = %d\n", cursor, pcb->pid); }
  pcb->pc = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->pc = %d\n", cursor, pcb->pc); }

  //REGISTROS
  pcb->AX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->AX = %d\n", cursor, pcb->AX); }
  pcb->BX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->BX = %d\n", cursor, pcb->BX); }
  pcb->CX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->CX = %d\n", cursor, pcb->CX); }
  pcb->DX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->DX = %d\n", cursor, pcb->DX); }
  pcb->EAX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->EAX = %d\n", cursor, pcb->EAX); }
  pcb->EBX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->EBX = %d\n", cursor, pcb->EBX); }
  pcb->ECX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->ECX = %d\n", cursor, pcb->ECX); }
  pcb->EDX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->EDX = %d\n", cursor, pcb->EDX); }
  pcb->RAX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->RAX = %d\n", cursor, pcb->RAX); }
  pcb->RBX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->RBX = %d\n", cursor, pcb->RBX); }
  pcb->RCX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->RCX = %d\n", cursor, pcb->RCX); }
  pcb->RDX = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->RDX = %d\n", cursor, pcb->RDX); }
  pcb->SI = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->SI = %d\n", cursor, pcb->SI); }
  pcb->DI = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->DI = %d\n", cursor, pcb->DI); }

  pcb->quantum = *(uint32_t *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->quantum = %d\n", cursor, pcb->quantum); }
  pcb->llegada_ready = *(double *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->llegada_ready = %d\n", cursor, pcb->llegada_ready); }
  pcb->llegada_running = *(double *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->llegada_running = %d\n", cursor, pcb->llegada_running); }

  pcb->estado_actual = *(int *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->estado_actual = %d\n", cursor, pcb->estado_actual); }
  pcb->fd_conexion = *(int *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: pcb->fd_conexion = %d\n", cursor, pcb->fd_conexion); }


  //INSTRUCCIONES
  int cantidadInstrucciones =*(int *)list_get(lista_elememtos, ++cursor);
  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: cantidadInstrucciones = %d\n", cursor, cantidadInstrucciones); }
  int offset = cursor;

  while (cursor < cantidadInstrucciones * 6 + offset)
  {
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    instruccion->id = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: id = %s \n", cursor, instruccion->id); }
    instruccion->param1 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: param1 = %d \n", cursor, instruccion->param1); }
    instruccion->param2 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: param2 = %d \n", cursor, instruccion->param2); }
    instruccion->param3 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: param3 = %d \n", cursor, instruccion->param3); }
    instruccion->param4 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: param4 = %d \n", cursor, instruccion->param4); }
    instruccion->param5 = string_duplicate(list_get(lista_elememtos, ++cursor));
    { DEBUGGING_SERIALIZATION printf("\n[Deserializar] paquete[%d]: param5 = %d \n", cursor, instruccion->param5); }

    list_add(pcb->instrucciones, instruccion);
  }


  list_destroy_and_destroy_elements(lista_elememtos, &free);

  { DEBUGGING_SERIALIZATION printf("\n[Deserializar] deserializar_pcb( ) [END]\n"); }

  return pcb;

}