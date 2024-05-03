
#include "memoria.h"


t_log* memoria_logger;
t_log* memoria_debug_logger;
t_config* memoria_config;

int fd_memoria;
int fd_io;
int fd_cpu;
int fd_kernel;

char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;


int main() {

    memoria();
      
    return 0;
}


void initialize_logger()
{
    memoria_logger = log_create("memoria.log", "memoria", true, LOG_LEVEL_INFO);
}

void initialize_config()
{
    t_config* memoria_config = config_create("memoria.config");
    if(memoria_config == NULL) {
        log_error(memoria_logger, "No se pudo abrir la config de memoria");
        exit(EXIT_FAILURE);
    }

    obtener_configuracion(memoria_config);

}

void obtener_configuracion(t_config* memoria_config)
{
    PUERTO_ESCUCHA = config_get_string_value(memoria_config, "PUERTO_ESCUCHA");
    TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(memoria_config, "TAM_PAGINA");
    PATH_INSTRUCCIONES = config_get_string_value(memoria_config, "PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(memoria_config, "RETARDO_RESPUESTA");
}

void initialize_sockets()
{
  
     //Incializo a memoria en modo server
    fd_memoria = start_server(NULL,PUERTO_ESCUCHA);
    log_info(memoria_logger, "Servidor Memoria iniciado en el puerto %s\n", PUERTO_ESCUCHA);

    log_info(memoria_logger, "Esperando conexion de CPU");
    fd_cpu = esperar_cliente(fd_memoria);

    if(fd_cpu == -1){
        log_error(memoria_logger, "No se pudo conectar a cpu");
		

    }else{
        log_info(memoria_logger, "Se conecto el modulo CPU");
        }   



    log_info(memoria_logger, "Esperando conexion de Kernel");
    fd_kernel = esperar_cliente(fd_memoria);

    if(fd_kernel == -1){
        log_error(memoria_logger, "No se pudo conectar a kernel");

    }else{
    log_info(memoria_logger, "Se conecto el modulo Kernel\n"); 
    }
    
  
    //espero conexion entrada y salida
    log_info(memoria_logger, "Esperando conexion de EntradaSalida");
    fd_io = esperar_cliente(fd_memoria);

    if(fd_io == -1){
        log_error(memoria_logger, "No se pudo conectar a entrada y salida");

    }else{
    log_info(memoria_logger, "Se conecto el modulo Entrada salida");

    }

 

}

void memoria()
{
    initialize_logger();
    initialize_config();
    initialize_sockets();
    log_info(memoria_logger, "Memoria inicializada correctamente");


}