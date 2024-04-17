
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



