
#include "socket.h"

extern t_log* logger;



int start_client(char* ip, char* port)
{

	int err;
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	

	err = getaddrinfo(ip, port, &hints, &server_info);


	int fd_conexion   = socket(server_info->ai_family,
                            server_info->ai_socktype,
						    server_info->ai_protocol);

	
	if(connect(fd_conexion  , server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		return -1;
	}

	freeaddrinfo(server_info);

	return fd_conexion  ;
}


int start_server(char* ip, char* port) {


	int fd_escucha ; //Guarda el File Descriptor(IDs) representado por un entero.

	int err;
	struct addrinfo hints, *servinfo; // Estruc q Contendra información sobre la dirección de un proveedor de servicios.

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(ip, port, &hints, &servinfo); //Traduce el nombre de una ubicación de servicio a un conjunto de direcciones de socket.

	// Creamos el socket de escucha del servidor
	fd_escucha  = socket(servinfo->ai_family,
							 servinfo->ai_socktype,
							 servinfo->ai_protocol);

	/* bind() y listen() son las llamadas al sistema que realiza
	 * la preparacion por parte del proceso servidor */

	int activado = 1;
	setsockopt(fd_escucha , SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	// 1) Asociamos el socket creado a un puerto
	bind(fd_escucha ,servinfo->ai_addr, servinfo->ai_addrlen);

	// 2) Escuchamos las conexiones entrantes a ese socket, cuya unica responsabilidad
	// es notificar cuando un nuevo cliente este intentando conectarse.

	listen(fd_escucha ,SOMAXCONN); // El servidor esta listo para recibir a los clientes (ESTA ESCUCHANDO).
	freeaddrinfo(servinfo);
	return fd_escucha ;
}

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