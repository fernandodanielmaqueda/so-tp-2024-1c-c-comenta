#include "entradasalida.h"


t_log* entrada_logger;
t_log* entrada_debug_logger;
t_config* entrada_config;

int fd_kernel;
int fd_memoria;

char* TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char* IP_KERNEL;
char* PUERTO_KERNEL;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char*  PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;



int main(int argc, char* argv[]) {

    entradaysalida();
   
    return 0;
}

void initialize_logger()
{
    entrada_logger = log_create("entrada.log", "entrada", true, LOG_LEVEL_INFO);
}

void initialize_config()
{
    t_config* entrada_config = config_create("entradasalida.config");
    if(entrada_config == NULL) {
        log_error(entrada_logger, "No se pudo abrir la config de entrada");
        exit(EXIT_FAILURE);
    }

    obtener_configuracion(entrada_config);
}

void obtener_configuracion(t_config* entrada_config)
{
    TIPO_INTERFAZ = config_get_string_value(entrada_config, "TIPO_INTERFAZ");
    TIEMPO_UNIDAD_TRABAJO = config_get_int_value(entrada_config, "TIEMPO_UNIDAD_TRABAJO");
    IP_KERNEL = config_get_string_value(entrada_config, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(entrada_config, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(entrada_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(entrada_config, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value(entrada_config, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(entrada_config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(entrada_config, "BLOCK_COUNT");
}


void initialize_sockets(){

    //Me conecto a memoria como cliente entardasalida
    log_info(entrada_logger, "Conectando con la memoria");
    fd_memoria = start_client(IP_MEMORIA, PUERTO_MEMORIA);

    if (fd_memoria == -1)
    {
        log_error(entrada_logger, "No se pudo conectar a memoria");
        exit(EXIT_FAILURE);
    } else
    log_info(entrada_logger, "I/O esta conectado con la memoria \n");

    if(fd_memoria == -1){
        log_error(entrada_logger, "No se pudo conectar con la memoria");
        exit(EXIT_FAILURE);
    }
    else{
    log_info(entrada_logger, "I/O esta conectado con la memoria \n");
    }


    //Me conecto a kernel como cliente entardasalida
    log_info(entrada_logger, "Inicializando cliente I/O para ir al  kernel");
    fd_kernel = start_client(IP_KERNEL, PUERTO_KERNEL);
    
    if (fd_kernel == -1)
    {
        log_error(entrada_logger, "No se pudo conectar a kernel");
        exit(EXIT_FAILURE);
    } else
    log_info(entrada_logger, "I/O esta conectado con el kernel \n");

 

}

void entradaysalida()
{
    initialize_logger();
    initialize_config();
    initialize_sockets();
    log_info(entrada_logger, "Entrada y salida iniciado correctamente\n");
   
}