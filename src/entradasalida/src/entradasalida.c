/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "entradasalida.h"

char *module_name = "entradasalida";
char *module_log_pathname = "entradasalida.log";
char *module_config_pathname = "entradasalida.config";

t_log* module_logger;
t_config* module_config;

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

int module(int argc, char* argv[]) {

    initialize_module_logger();
    initialize_module_config();
    initialize_sockets();
    log_info(module_logger, "Entrada y salida iniciado correctamente\n");
   
    return EXIT_SUCCESS;
}

void read_module_config(t_config* module_config)
{
    TIPO_INTERFAZ = config_get_string_value(module_config, "TIPO_INTERFAZ");
    TIEMPO_UNIDAD_TRABAJO = config_get_int_value(module_config, "TIEMPO_UNIDAD_TRABAJO");
    IP_KERNEL = config_get_string_value(module_config, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(module_config, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(module_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(module_config, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value(module_config, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(module_config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(module_config, "BLOCK_COUNT");
}


void initialize_sockets(){

    //Me conecto a memoria como cliente entardasalida
    log_info(module_logger, "Conectando con la memoria");
    fd_memoria = client_connect(IP_MEMORIA, PUERTO_MEMORIA);

    if (fd_memoria == -1)
    {
        log_error(module_logger, "No se pudo conectar a memoria");
        exit(EXIT_FAILURE);
    } else
    log_info(module_logger, "I/O esta conectado con la memoria \n");

    if(fd_memoria == -1){
        log_error(module_logger, "No se pudo conectar con la memoria");
        exit(EXIT_FAILURE);
    }
    else{
    log_info(module_logger, "I/O esta conectado con la memoria \n");
    }


    //Me conecto a kernel como cliente entardasalida
    log_info(module_logger, "Inicializando cliente I/O para ir al  kernel");
    fd_kernel = client_connect(IP_KERNEL, PUERTO_KERNEL);
    
    if (fd_kernel == -1)
    {
        log_error(module_logger, "No se pudo conectar a kernel");
        exit(EXIT_FAILURE);
    } else
    log_info(module_logger, "I/O esta conectado con el kernel \n");

 

}