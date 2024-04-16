#include "kernel.h"
t_log* kernel_logger;
t_log* kernel_debug_logger;
t_config* kernel_config;
t_list* new_list;
t_list* ready_list;

// Connections
int socket_cpu_interrupt;
int socket_cpu_dispatch;
int socket_memoria;
int socket_entradasalida;

size_t bytes;

int main(int argc, char* argv[]) {
   
    //inicializamos  kernel
    kernel(argc, argv);

    return 0;
}


void initialize_logger(char **argv)
{
	kernel_logger = log_create("kernel.log", "kernel", true, LOG_LEVEL_INFO);
}

void initialize_config(char **argv)
{
	t_config* kernel_config = config_create(argv[1]);
	if(kernel_config == NULL) {
		log_error(kernel_logger, "No se pudo abrir la config de kernel");
        exit(EXIT_FAILURE);
	}

}

void initialize_sockets()
{
	socket_cpu_interrupt = start_client_module("CPU_INTERRUPT", kernel_config);
	//usleep(10000); 
	socket_cpu_dispatch = start_client_module("CPU_DISPATCH", kernel_config);
	//usleep(10000);
	socket_memoria = start_client_module("MEMORIA_KERNEL" , kernel_config); 
	//usleep(10000);
    socket_entradasalida = start_client_module("ENTRADASALIDA", kernel_config);
    //usleep(10000);

}

void kernel(int argc, char* argv[]){

    initialize_logger(argv);
	initialize_config(argv);
	initialize_sockets();

    int socket_kernel = start_server_module("KERNEL", kernel_config);

	new_list = list_create();

	ready_list = list_create();
}

void agregar_a_new(t_pcb* pcb){

	list_add(new_list, pcb);
}


void new_a_ready(){

	t_pcb* pcb = list_get(new_list, 0);
	list_add(ready_list, pcb);
	
}

