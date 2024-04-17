
#include "socket.h"

extern t_log* logger;



int start_client(char* ip, char* port)
{

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	

	getaddrinfo(ip, port, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;

	// Ahora que tenemos el socket, vamos a conectarlo
	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	

    if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		
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
		
		return -1;
	}
	else{
		bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	}


	// Escuchamos las conexiones entrantes
	if (listen(socket_servidor, SOMAXCONN) == -1) {
		
		freeaddrinfo(servinfo);
		return -1;
	}
	else{
		listen(socket_servidor, SOMAXCONN);
		
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
        
        return -1;
    }

	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}




/*
void get_ip_port_from_module(const char* module,  char * path_config, char* ip, char* port)
{
	t_config* config = config_create(path_config);

	if(config == NULL) {
		perror("No se pudo abrir la config");
		return;
	}

	char* config_ip_key = string_from_format("%s_IP", module);
	char* config_port_key = string_from_format("%s_PUERTO", module);

	strcpy(ip, config_get_string_value(config, config_ip_key));
	strcpy(port, config_get_string_value(config, config_port_key));

	free(config_ip_key);
	free(config_port_key);

	config_destroy(config);
}

int start_client_module(char* module, char* pathconfig)
{
	char* ip = malloc(sizeof(char) * 20);
	char* port = malloc(sizeof(char) * 20);

	get_ip_port_from_module(module, pathconfig,ip, port);

	log_rect(logger, "Creo socket cliente", 
		"MODULO [%s]\n"
		"IP [%s]\n"
		"PUERTO [%s]"
	, module, ip, port);

	int fd_conexion   = start_client(ip, port);
	if(fd_conexion   == -1)
	{
		log_error(logger, "No se puede establecer conexion con el modulo %s, ¿No sera que falta levantarlo?", module);
		exit(EXIT_FAILURE);
	}

	free(ip);
	free(port);

	return fd_conexion  ;
}


int start_server_module(char* module, char * pathconfig)
{
	char* ip = malloc(sizeof(char) * 20);
	char* port = malloc(sizeof(char) * 20);

	get_ip_port_from_module(module,pathconfig, ip, port);

	log_rect(logger, "Creo socket servidor", 
		"MODULO [%s]\n"
		"IP [%s]\n"
		"PUERTO [%s]"
	, module, ip, port);

	int socket_server = start_server(ip, port);

	free(ip);
	free(port);

	return socket_server;
}

*/