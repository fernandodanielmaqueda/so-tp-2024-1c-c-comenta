
#include "socket.h"

extern t_log* logger;



int start_client(char* ip, char* port)
{

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	

	getaddrinfo(ip, port, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;

	// Ahora que tenemos el socket, vamos a conectarlo
	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	
	int devuelve_connect = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

    if( devuelve_connect!= 0)
	{
		printf("Error u conectar el socket\n");
       	return -1;
	}
	freeaddrinfo(server_info);
    return socket_cliente;
}


int start_server(char* ip, char* port) {

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, port, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_servidor == -1) {
		return -1;
		
	}

	// Asociamos el socket a un puerto
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {

		close(socket_servidor);
		return -1;
	}
	


	// Escuchamos las conexiones entrantes
	if (listen(socket_servidor, SOMAXCONN) == -1) {
		
		freeaddrinfo(servinfo);
		return -1;
	}
	
		
	freeaddrinfo(servinfo);
	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept (socket_servidor, NULL, NULL);
	if(socket_cliente == -1)
    {
        printf("Error al aceptar cliente\n");
        return -1;
    }

	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}



void add_to_package(t_paquete *paquete, void *valor, int tamanio)
{
  paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));
  memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
  memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);
  paquete->buffer->size += tamanio + sizeof(int);

}


void serialize_pcb(t_paquete *paquete, t_pcb *pcb)
{
  DEBUG_PRINTF("\n[Serializar] serializar_pcb( ) [...]\n");

  int cantidadInstrucciones = list_size(pcb->instrucciones);

  int cursor = 0;
  
  add_to_package(paquete, &(pcb->pid), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: pid = %d\n", cursor, pcb->pid);
  cursor++;
  add_to_package(paquete, &(pcb->pc), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: pc = %d\n", cursor, pcb->pc);
  cursor++;

  //REGISTROS
  add_to_package(paquete, &(pcb->AX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro AX = %s\n", cursor, pcb->AX);
  cursor++;
  add_to_package(paquete, &(pcb->BX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro BX = %s\n", cursor, pcb->BX);
  cursor++;
  add_to_package(paquete, &(pcb->CX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro CX = %s\n", cursor, pcb->CX);
  cursor++;
  add_to_package(paquete, &(pcb->DX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro DX = %s\n", cursor, pcb->DX);
  cursor++;
  add_to_package(paquete, &(pcb->EAX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro EAX = %s\n", cursor, pcb->EAX);
  cursor++;
  add_to_package(paquete, &(pcb->EBX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro EBX = %s\n", cursor, pcb->EBX);
  cursor++;
  add_to_package(paquete, &(pcb->ECX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro ECX = %s\n", cursor, pcb->ECX);
  cursor++;
  add_to_package(paquete, &(pcb->EDX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro EDX = %s\n", cursor, pcb->EDX);
  cursor++;
  add_to_package(paquete, &(pcb->RAX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RAX = %s\n", cursor, pcb->RAX);
  cursor++;
  add_to_package(paquete, &(pcb->RBX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RBX = %s\n", cursor, pcb->RBX);
  cursor++;
  add_to_package(paquete, &(pcb->RCX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RCX = %s\n", cursor, pcb->RCX);
  cursor++;
  add_to_package(paquete, &(pcb->RDX), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RDX = %s\n", cursor, pcb->RDX);
  cursor++;
  add_to_package(paquete, &(pcb->SI), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro SI = %s\n", cursor, pcb->SI);
  cursor++;
  add_to_package(paquete, &(pcb->DI), sizeof(uint32_t));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro DI = %s\n", cursor, pcb->DI);
  cursor++;

  /*CASO CHAR*
  add_to_package(paquete, &(pcb->AX), strlen(&(pcb->AX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro AX = %s\n", cursor, pcb->AX);
  cursor++;
  add_to_package(paquete, &(pcb->BX), strlen(&(pcb->BX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro BX = %s\n", cursor, pcb->BX);
  cursor++;
  add_to_package(paquete, &(pcb->CX), strlen(&(pcb->CX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro CX = %s\n", cursor, pcb->CX);
  cursor++;
  add_to_package(paquete, &(pcb->DX), strlen(&(pcb->DX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro DX = %s\n", cursor, pcb->DX);
  cursor++;
  add_to_package(paquete, &(pcb->EAX), strlen(&(pcb->EAX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro EAX = %s\n", cursor, pcb->EAX);
  cursor++;
  add_to_package(paquete, &(pcb->EBX), strlen(&(pcb->EBX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro EBX = %s\n", cursor, pcb->EBX);
  cursor++;
  add_to_package(paquete, &(pcb->ECX), strlen(&(pcb->ECX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro ECX = %s\n", cursor, pcb->ECX);
  cursor++;
  add_to_package(paquete, &(pcb->EDX), strlen(&(pcb->EDX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro EDX = %s\n", cursor, pcb->EDX);
  cursor++;
  add_to_package(paquete, &(pcb->RAX), strlen(&(pcb->RAX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RAX = %s\n", cursor, pcb->RAX);
  cursor++;
  add_to_package(paquete, &(pcb->RBX), strlen(&(pcb->RBX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RBX = %s\n", cursor, pcb->RBX);
  cursor++;
  add_to_package(paquete, &(pcb->RCX), strlen(&(pcb->RCX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RCX = %s\n", cursor, pcb->RCX);
  cursor++;
  add_to_package(paquete, &(pcb->RDX), strlen(&(pcb->RDX)) + 1);
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Registro RDX = %s\n", cursor, pcb->RDX);
  cursor++;
*/

  add_to_package(paquete, &(pcb->estado_actual), sizeof(int));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: estado_actual = %d\n", cursor, pcb->fd_conexion);
  cursor++;
  agregar_a_paquete(paquete, &(pcb->fd_conexion), sizeof(int));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: fd_conexion = %d\n", cursor, pcb->fd_conexion);
  cursor++;

  //INSTRUCCIONES
  add_to_package(paquete, &cantidadInstrucciones, sizeof(int));
  DEBUG_PRINTF("\n[Serializar] paquete[%d]: Cantidad de Instrucciones = %d\n", cursor, cantidadInstrucciones);
  cursor++;

  t_instruccion *lineaInstruccion;
  for (int i = 0; i < cantidadInstrucciones; i++)
  {
    lineaInstruccion = list_get(pcb->instrucciones, i);

    add_to_package(paquete, lineaInstruccion->id, strlen(lineaInstruccion->id) + 1);
    DEBUG_PRINTF("\n[Serializar] paquete[%d]: Identificador instruccion = %s\n", cursor, lineaInstruccion->id);
    cursor++;
    add_to_package(paquete, lineaInstruccion->param1, strlen(lineaInstruccion->param1) + 1);
    DEBUG_PRINTF("\n[Serializar] paquete[%d]: Param1 = %d\n", cursor, lineaInstruccion->param1);
    cursor++;
    add_to_package(paquete, lineaInstruccion->param2, strlen(lineaInstruccion->param2) + 1);
    DEBUG_PRINTF("\n[Serializar] paquete[%d]: Param2 = %d\n", cursor, lineaInstruccion->param2);
    cursor++;
    add_to_package(paquete, lineaInstruccion->param3, strlen(lineaInstruccion->param3) + 1);
    DEBUG_PRINTF("\n[Serializar] paquete[%d]: Param3 = %d\n", cursor, lineaInstruccion->param3);
    cursor++;
    add_to_package(paquete, lineaInstruccion->param4, strlen(lineaInstruccion->param4) + 1);
    DEBUG_PRINTF("\n[Serializar] paquete[%d]: Param4 = %d\n", cursor, lineaInstruccion->param4);
    cursor++;
    add_to_package(paquete, lineaInstruccion->param5, strlen(lineaInstruccion->param5) + 1);
    DEBUG_PRINTF("\n[Serializar] paquete[%d]: Param5 = %d\n", cursor, lineaInstruccion->param5);
    cursor++;
  }

  DEBUG_PRINTF("\n[Serializar] serializar_pcb( ) [END]\n");

}