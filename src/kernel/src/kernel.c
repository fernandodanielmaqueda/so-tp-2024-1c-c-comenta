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

int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_entrada_salida;
int fd_kernel;

char* PUERTO_ESCUCHA;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char** RECURSOS;
char** INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;

size_t bytes;

int main() {
   
    //inicializamos  kernel
    kernel();

    return 0;
}


void initialize_logger()
{
	kernel_logger = log_create("kernel.log", "kernel", true, LOG_LEVEL_INFO);
}

void initialize_config()
{
	t_config* kernel_config = config_create("kernel.config");
	if(kernel_config == NULL) {
		log_error(kernel_logger, "No se pudo abrir la config de kernel");
        exit(EXIT_FAILURE);
	}

	obtener_configuracion(kernel_config);
}

void obtener_configuracion(t_config* kernel_config)
{
	PUERTO_ESCUCHA = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");
	IP_MEMORIA = config_get_string_value(kernel_config, "IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
	IP_CPU = config_get_string_value(kernel_config, "IP_CPU");
	PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	ALGORITMO_PLANIFICACION = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(kernel_config, "QUANTUM");
	RECURSOS = config_get_array_value(kernel_config, "RECURSOS");
	INSTANCIAS_RECURSOS = config_get_array_value(kernel_config, "INSTANCIAS_RECURSOS");
	GRADO_MULTIPROGRAMACION = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");
}

void initialize_sockets()
{
	
	//Dejo al kernel en modo server escuchando por el puerto de la config
	fd_kernel = start_server(NULL, PUERTO_ESCUCHA);
	log_info(kernel_logger, "Kernel escuchando en puerto... %s", PUERTO_ESCUCHA);

	//Conectar como cliente a memoria
	log_info(kernel_logger, "Conectando a memoria...");

	fd_memoria = esperar_cliente(fd_kernel);
	log_info(kernel_logger, "Kernel conectado a memoria");


	

	//Conectar como cliente a Entrada/Salida
	log_info(kernel_logger, "Conectando a Entrada/Salida...");
	fd_entrada_salida = esperar_cliente(fd_kernel);
	


	log_info(kernel_logger, "Kernel conectado a Entrada/Salida");


	//Conectar como cliente a CPU Dispatch
	log_info(kernel_logger, "Conectando a CPU Dispatch...");
	fd_cpu_dispatch = esperar_cliente(fd_kernel);
	log_info(kernel_logger, "Kernel conectado a CPU Dispatch");

	//Conectar como cliente a CPU Interrupt
	log_info(kernel_logger, "Conectando a CPU Interrupt...");
	fd_cpu_interrupt = esperar_cliente(fd_kernel);
	log_info(kernel_logger, "Kernel conectado a CPU Interrupt");

	
	
}
	
void kernel(){

    initialize_logger();
	initialize_config();

	
	initialize_sockets();


 


//ESTO ES PCB Y ESTADOS
	new_list = list_create();

	ready_list = list_create();
}

// =====///


void agregar_a_new(t_pcb* pcb){

	list_add(new_list, pcb);
}


void new_a_ready(){

	t_pcb* pcb = list_get(new_list, 0);
	list_add(ready_list, pcb);
	
}

