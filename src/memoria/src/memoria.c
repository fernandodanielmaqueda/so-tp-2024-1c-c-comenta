
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "memoria.h"

t_log *memoria_logger;
t_log *memoria_debug_logger;
t_config *memoria_config;

void *memoria_principal;
pthread_t hilo_kernel;
pthread_t hilo_cpu;
pthread_t hilo_io;

int fd_memoria;
int fd_io;
int fd_cpu;
int fd_kernel;

char *PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

int memoria(int argc, char* argv[])
{
    initialize_logger();
    initialize_config();

    memoria_principal = (void *)malloc(TAM_MEMORIA);
    memset(memoria_principal, (u_int32_t)'0', TAM_MEMORIA); //Llena de 0's el espacio de memoria

    initialize_sockets();
    log_info(memoria_logger, "Memoria inicializada correctamente");
 
 /*   
    pthread_create(&hilo_cpu, NULL, (void *)manejar_conexion_cpu, (void *)fd_cpu);
    pthread_create(&hilo_kernel, NULL, (void *)manejar_conexion_kernel, (void *)fd_kernel);
    pthread_create(&hilo_io, NULL, (void *)manejar_conexion_io, (void *)fd_io);

    pthread_join(hilo_cpu, NULL);
    pthread_join(hilo_kernel, NULL);
    pthread_join(hilo_io, NULL);

    */
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

    if (fd_cpu == -1)
    {
        log_error(memoria_logger, "No se pudo conectar a CPU");
        exit(EXIT_FAILURE);
    } else log_info(memoria_logger, "Se conecto el modulo CPU");
    

    log_info(memoria_logger, "Esperando conexion de Kernel");
    fd_kernel = esperar_cliente(fd_memoria);

    if (fd_kernel == -1)
    {
        log_error(memoria_logger, "No se pudo conectar a Kernel");
        exit(EXIT_FAILURE);
    } else
    log_info(memoria_logger, "Se conecto el modulo Kernel\n"); 
    
    //espero conexion entrada y salida
    log_info(memoria_logger, "Esperando conexion de EntradaSalida");
    fd_io = esperar_cliente(fd_memoria);

    if (fd_io == -1)
    {
        log_error(memoria_logger, "No se pudo conectar a IO");
        exit(EXIT_FAILURE);
    } else
    log_info(memoria_logger, "Se conecto el modulo IO");

    if(fd_io == -1){
        log_error(memoria_logger, "No se pudo conectar a entrada y salida");

    }else{
    log_info(memoria_logger, "Se conecto el modulo Entrada salida");

    }


}


void leer_archivo_pseudocodigo(char* nombreArchivo){

    //char* path_buscado = PATH_INSTRUCCIONES;


}