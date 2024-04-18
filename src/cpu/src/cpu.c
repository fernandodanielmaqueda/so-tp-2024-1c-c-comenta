#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/memory.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/socket.h>
#include "cpu.h"


t_log* cpu_logger;
t_log* cpu_debug_logger;
t_config* cpu_config;

int fd_kernel;
int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;


char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;


int main(int argc, char* argv[]) {

    cpu();


    return 0;
}

void initialize_logger()
{
	cpu_logger = log_create("cpu.log", "cpu", true, LOG_LEVEL_INFO);
}

void initialize_config()
{
    t_config* cpu_config = config_create("cpu.config");
    if(cpu_config == NULL) {
        log_error(cpu_logger, "No se pudo abrir la config de cpu");
        exit(EXIT_FAILURE);
    }

    obtener_configuracion(cpu_config);
}

void obtener_configuracion(t_config* cpu_config)
{
    IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(cpu_config, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(cpu_config, "ALGORITMO_TLB");
}

void initialize_sockets(){


        //Inicializo memoria
    log_info(cpu_logger, "Iniciando clinte cpu para ir a memoria");
    fd_memoria = start_client(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(cpu_logger, "Conectado a memoria en %s:%s", IP_MEMORIA, PUERTO_MEMORIA);


    //Dejo a CPU en modo server incializo server cpu dispatch
    fd_cpu_dispatch = start_server(NULL,PUERTO_ESCUCHA_DISPATCH);
    log_info(cpu_logger, "CPU en modo dispatch server escuchando en puerto %s \n", PUERTO_ESCUCHA_DISPATCH);

    //Dejo a CPU en modo server incialzio server cpu interrupt
    fd_cpu_interrupt = start_server(NULL,PUERTO_ESCUCHA_INTERRUPT);
    log_info(cpu_logger, "CPU en modo interrupt server escuchando en puerto %s \n", PUERTO_ESCUCHA_INTERRUPT);

    
    //Espero conexion  dispatch kernel
    log_info(cpu_logger, "Esperando conexion de kernel en puerto disptach %s \n", PUERTO_ESCUCHA_DISPATCH);
    fd_kernel = esperar_cliente(fd_cpu_dispatch);
    log_info(cpu_logger, "Conectado a kernel en puerto dispatch ");

    //Espero a conexion interrupt kernel
    log_info(cpu_logger, "Esperando conexion de kernel en puerto interrupt %s \n", PUERTO_ESCUCHA_INTERRUPT);
    fd_kernel = esperar_cliente(fd_cpu_interrupt);
    log_info(cpu_logger, "Conectado a kernel en puerto interrupt ");


  

}

void cpu()
{
    initialize_logger();
    initialize_config();
    initialize_sockets();

    log_info(cpu_logger, "CPU inicializado correctamente");

}

