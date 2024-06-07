/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "socket.h"

const char *PORT_NAMES[PortType_Count] = {[KERNEL_TYPE] = "Kernel", [CPU_TYPE] = "CPU", [CPU_DISPATCH_TYPE] = "CPU (Dispatch)", [CPU_INTERRUPT_TYPE] = "CPU (Interrupt)", [MEMORY_TYPE] = "Memoria", [IO_TYPE] = "Entrada/Salida", [TO_BE_DEFINED_TYPE] = "A identificar"};
// const int32_t HANDSHAKES[PortType_Count] = {[KERNEL_TYPE] = 10, [CPU_TYPE] = 20, [CPU_DISPATCH_TYPE] = 21, [CPU_INTERRUPT_TYPE] = 22, [MEMORY_TYPE] = 30, [IO_TYPE] = 40, [TO_BE_DEFINED_TYPE] = -1};

void *client_thread_connect_to_server(void *connection_parameter) {
  t_Connection *connection = (t_Connection*) connection_parameter;

  size_t bytes;

  int32_t handshake = (int32_t) connection->client_type;
  int32_t result;

  while(1) {
    while(1) {
      log_info(SOCKET_LOGGER, "Intentando conectar con [Servidor] %s en IP: %s - Puerto: %s...", PORT_NAMES[connection->server_type], connection->ip, connection->port);
      connection->fd_connection = client_start_try(connection->ip, connection->port);

      if(connection->fd_connection != -1) break;
      else {
        log_warning(SOCKET_LOGGER, "No se pudo conectar con [Servidor] %s en IP: %s - Puerto: %s. Reintentando en %d segundos...", PORT_NAMES[connection->server_type], connection->ip, connection->port, RETRY_CONNECTION_IN_SECONDS);
        sleep(RETRY_CONNECTION_IN_SECONDS);
      }
    }

    log_info(SOCKET_LOGGER, "Conectado con [Servidor] %s en IP: %s - Puerto: %s", PORT_NAMES[connection->server_type], connection->ip, connection->port);

    // Handshake

    bytes = send(connection->fd_connection, &handshake, sizeof(int32_t), 0);
    bytes = recv(connection->fd_connection, &result, sizeof(int32_t), MSG_WAITALL);

    if(result == 0) break;
    else {
      close(connection->fd_connection);
      log_warning(SOCKET_LOGGER, "Error Handshake con [Servidor] %s en IP: %s - Puerto: %s. Reintentando en %d segundos...", PORT_NAMES[connection->server_type], connection->ip, connection->port, RETRY_CONNECTION_IN_SECONDS);
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
    log_warning(SOCKET_LOGGER, "Funcion getaddrinfo: %s\n", gai_strerror(exit_code));
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
      log_warning(SOCKET_LOGGER, "Funcion socket: %s\n", strerror(errno));
      continue; /* This one failed */
    }

    if(connect(fd_client, rp->ai_addr, rp->ai_addrlen) == 0) break; /* Until one succeeds */
    else {
      log_warning(SOCKET_LOGGER, "Funcion connect: %s\n", strerror(errno));
    }

    close(fd_client);
  }
	
  freeaddrinfo(result); /* No longer needed */

  if (rp == NULL) { /* No address succeeded */
    return -1;
  }
	
  return fd_client;
}

void server_start(t_Server *server) {

  while(1) {
    log_info(SOCKET_LOGGER, "Intentando iniciar [Servidor] %s en Puerto: %s...", PORT_NAMES[server->server_type], server->port);
    server->fd_listen = server_start_try(server->port);

    if(server->fd_listen != -1) break;
    else {
      log_warning(SOCKET_LOGGER, "No se pudo iniciar [Servidor] %s en Puerto: %s. Reintentando en %d segundos...", PORT_NAMES[server->server_type], server->port, RETRY_CONNECTION_IN_SECONDS);
      sleep(RETRY_CONNECTION_IN_SECONDS);
    }
  }

  log_info(SOCKET_LOGGER, "Escuchando [Servidor] %s en Puerto: %s", PORT_NAMES[server->server_type], server->port);
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
    log_warning(SOCKET_LOGGER, "Funcion getaddrinfo: %s\n", gai_strerror(exit_code));
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
      log_warning(SOCKET_LOGGER, "Funcion socket: %s\n", strerror(errno));
      continue; /* This one failed */
    }

    if(bind(fd_server, rp->ai_addr, rp->ai_addrlen) == 0) break; /* Until one succeeds */
    else {
      log_warning(SOCKET_LOGGER, "Funcion bind: %s\n", strerror(errno));
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
		log_warning(SOCKET_LOGGER, "Funcion listen: %s\n", strerror(errno));
		return -1;
	}

	return fd_server;
}

int server_accept(int fd_server) {
  // Syscall bloqueante que se queda esperando hasta que llegue un nuevo cliente
	int fd_client = accept(fd_server, NULL, NULL);

	if(fd_client == -1) {
      log_warning(SOCKET_LOGGER, "Funcion accept: %s\n", strerror(errno));
  }

	return fd_client;
}