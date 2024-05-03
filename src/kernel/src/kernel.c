#include "kernel.h"
t_log* kernel_logger;
t_log* kernel_debug_logger;
t_config* kernel_config;

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
    initialize_kernel();

	//UN HILO PARA CADA PROCESO
	iniciar_planificador_largo_plazo();
	iniciar_planificador_corto_plazo();
	iniciar_receptor_mensajes_cpu();

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
	
//*********************************************************************************************************************************************************///

	//Incio cliente kernel para ir a memoria
	log_info(kernel_logger, "Iniciando cliente kernel para ir a memoria...");
	fd_kernel = start_client(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(kernel_logger, "Kernel esta conectado a memoria en el puerto %s \n", PUERTO_MEMORIA);

	//Incio cliente kernel para ir a CPU Dispatch
	log_info(kernel_logger, "Iniciando cliente kernel para ir a CPU Dispatch...");
	fd_kernel = start_client(IP_CPU, PUERTO_CPU_DISPATCH);
	log_info(kernel_logger, "Kernel esta conectado a CPU Dispatch en el puerto %s\n", PUERTO_CPU_DISPATCH);

	//Incio cliente kernel para ir a CPU Interrupt
	log_info(kernel_logger, "Iniciando cliente kernel para ir a CPU Interrupt... ");
	fd_kernel = start_client(IP_CPU, PUERTO_CPU_INTERRUPT);
	log_info(kernel_logger, "Kernel esta conectado a CPU Interrupt en el puerto %s\n", PUERTO_CPU_INTERRUPT);

	//Despues de las conexiones como cliente abro la conexion a servidor

		//Dejo al kernel en modo server escuchando por el puerto de la config
	fd_kernel = start_server(NULL, PUERTO_ESCUCHA);
	log_info(kernel_logger, "Kernel escuchando en puerto... %s \n", PUERTO_ESCUCHA);


	//=============DISCALIMER: SI ENTRADA SALIDA LO DEJO ACA HACE BIEN EL ESPERA DELMODLUO ENTRADA SALIDA... SI LO PONGO ABAJO DE TODO NO LO CONECTA======//////
	//Espero conexion entrada/salida
	log_info(kernel_logger, "Esperando conexion a Entrada/Salida...");
	fd_entrada_salida = esperar_cliente(fd_kernel);
	log_info(kernel_logger, "Kernel conectado a Entrada/Salida\n");


	
}
	
void initialize_kernel(){

    initialize_logger();
	initialize_config();

	
	initialize_sockets();
	log_info(kernel_logger, "Kernel inicializado\n");

	sem_init(&sem_planificador_largo_plazo, 0, 0);
	sem_init(&sem_planificador_corto_plazo, 0, 0);
	sem_init(&contador_multiprogramacion, 0, GRADO_MULTIPROGRAMACION);

	LISTA_NEW = list_create();
	LISTA_READY = list_create();
	LISTA_EXEC = list_create();
	LISTA_BLOCKED = list_create();
	LISTA_EXIT = list_create();
}

void iniciar_planificador_largo_plazo()
{
	pthread_create(&hilo_largo_plazo, NULL, (void *)planificador_largo_plazo, NULL);
	//log_info(logger_kernel, "Inicio planificador largo plazo");
	pthread_detach(hilo_largo_plazo);
}

void iniciar_planificador_corto_plazo() //ESTADO RUNNIG - MULTIPROCESAMIENTO 
{
	pthread_create(&hilo_corto_plazo, NULL, (void *)planificador_corto_plazo, NULL);
	//log_info(logger_kernel, "Inicio planificador corto plazo");
	pthread_detach(hilo_corto_plazo);
}

void iniciar_receptor_mensajes_cpu()
{
	pthread_create(&hilo_mensajes_cpu, NULL, (void *)receptor_mensajes_cpu, NULL);
	//log_info(logger_kernel, "Inicio mensajes cpu");
	pthread_detach(hilo_mensajes_cpu);
}


void planificador_largo_plazo()
{ 
	while(1){
		sem_wait(&sem_planificador_largo_plazo);
		sem_wait(&contador_multiprogramacion);
		
		//ACA VAN OTRAS COSAS QUE HACE EL PLANIFICADOR DE LARGO PLAZO (MENSAJES CON OTROS MODULOS, ETC)

		// cambiar_estado(pcb, READY);
		
	}
}

void planificador_corto_plazo()
{

	while(1){
		sem_wait(&sem_planificador_corto_plazo);	

		t_pcb* pcb;

		if(!strcmp(ALGORITMO_PLANIFICACION, "VRR")) {
			//pcb = algoritmo_VRR();
		} else if (!strcmp(ALGORITMO_PLANIFICACION, "FIFO")){
			pcb = algoritmo_FIFO();
		} else if (!strcmp(ALGORITMO_PLANIFICACION, "RR")){
			//pcb = algoritmo_RR();
		} else {
			// log_error(logger_kernel, "El algoritmo de planificacion ingresado no existe\n");
		}

		cambiar_estado(pcb, EXEC);

		//FALTA SERIALIZAR PCB
		//FALTA ENVIAR PAQUETE A CPU
	}
}

t_pcb *algoritmo_FIFO()
{
	pthread_mutex_lock(&mutex_LISTA_READY);
	t_pcb *pcb = (t_pcb *)list_remove(LISTA_READY, 0);
	pthread_mutex_unlock(&mutex_LISTA_READY);
	return pcb;
}

void receptor_mensajes_cpu()
{
	// t_paquete *paquete;

	while (1)
	{
		
		//FALTA RECIBIR PAQUETE DE LA CONEXION CON CPU
		//FALTA DESERIALIZAR PCB
		
		/*
		switch (paquete->codigo_operacion)
		{
			case EXIT:
			{
				cambiar_estado(pcb, EXITED);

				break;
			}
			case SET:
			{

			}
			case MOVE_IN:
			{

			}
			case MOVE_OUT:
			{

			}
			case SUM:
			{

			}
			case SUB:
			{

			}
			case JNZ:
			{

			}
			case RESIZE:
			{
				//mando mensaje a memoria
				//Recibo mensaje de memorai
				//mando mensaje a fs
				//recibo mensaje de fs
				//mando a cpu de vuelta
			}
			case COPY_STRING:
			{

			}
			case WAIT:
			{

			}
			case SIGNAL:
			{

			}
			case IO_GEN_SLEEP:
			{

			}
			case IO_STDIN_READ:
			{

			}
			case IO_STDOUT_WRITE:
			{

			}
			case IO_FS_CREATE:
			{

			}
			case IO_FS_DELETE:
			{

			}
			case IO_FS_TRUNCATE:
			{

			}
			case IO_FS_WRITE:
			{

			}
			case IO_FS_READ:
			{

			}
			case EXIT:
			{

			}
			// ACA SIGO AGREGANDO CASES SEGUN LA INSTRUCCION
		}
		*/
	}
}

void cambiar_estado(t_pcb* pcb, int estado_nuevo) 
{
	int estado_anterior = pcb->estado_actual;
	pcb->estado_actual = estado_nuevo;
	char* global_estado_anterior;
	
	// t_paquete* paquete;
	
	bool _remover_por_pid(void* elemento) {
			return (((t_pcb*)elemento)->pid == pcb->pid);
	}

	switch (estado_anterior){ //! ESTADO ANTERIOR
		case NEW:
		{
			global_estado_anterior="NEW";
			pthread_mutex_lock(&mutex_LISTA_NEW);
			list_remove_by_condition(LISTA_NEW, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LISTA_NEW);
			break;
		}
		case READY:{
			global_estado_anterior="READY";
			pthread_mutex_lock(&mutex_LISTA_READY);
			list_remove_by_condition(LISTA_READY, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LISTA_READY);
			break;
		}
		case EXEC:
		{
			global_estado_anterior="EXEC";
			pthread_mutex_lock(&mutex_LISTA_EXEC);
			list_remove_by_condition(LISTA_EXEC, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LISTA_EXEC);
			break;
		}
		case BLOCKED:
		{
			global_estado_anterior="BLOCKED";
			pthread_mutex_lock(&mutex_LISTA_BLOCKED);
			list_remove_by_condition(LISTA_BLOCKED, _remover_por_pid);
			pthread_mutex_unlock(&mutex_LISTA_BLOCKED);		
			break;
		}
		}
	switch(estado_nuevo){ // ! ESTADO NUEVO
		case NEW:
		{
			pthread_mutex_lock(&mutex_LISTA_NEW);
			list_add(LISTA_NEW, pcb);
			// log_info(logger_kernel, "Se crea el proceso <%d> en NEW" ,pcb->pid);
			pthread_mutex_unlock(&mutex_LISTA_NEW);
	
			sem_post(&sem_planificador_largo_plazo);
			break;
		}
		case READY:
		{
			pcb->llegada_ready=timenow();

			pthread_mutex_lock(&mutex_LISTA_READY);
			// log_info(logger_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <READY>", pcb->pid, global_estado_anterior);
			list_add(LISTA_READY, pcb);
			pthread_mutex_unlock(&mutex_LISTA_READY);
			sem_post(&sem_planificador_corto_plazo);
			
			break;
		}
		case EXEC:
		{
			pcb->llegada_running=timenow();
			
			pthread_mutex_lock(&mutex_LISTA_EXEC);
			list_add(LISTA_EXEC, pcb);
			pthread_mutex_unlock(&mutex_LISTA_EXEC);
			// log_info(logger_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXEC>",pcb->pid, global_estado_anterior);
	
			break;
		}
		case BLOCKED:
		{
			pthread_mutex_lock(&mutex_LISTA_BLOCKED);
			list_add(LISTA_BLOCKED, pcb);
			pthread_mutex_unlock(&mutex_LISTA_BLOCKED);

			// log_info(logger_kernel, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <BLOCKED>",pcb->pid, global_estado_anterior);

			break;
		}
		//Todos los casos de salida de un proceso.
		case EXITED:{
			
			// log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", pcb->pid);

			sem_post(&contador_multiprogramacion);

			pthread_mutex_lock(&mutex_LISTA_EXIT);
			list_add(LISTA_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LISTA_EXIT);
			
		
			break;
		}
		/*
		case INVALID_RESOURCE:{
			
			log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", pcb->pid);
			

			sem_post(&contador_multiprogramacion);

			pthread_mutex_lock(&mutex_LISTA_EXIT);
			list_add(LISTA_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LISTA_EXIT);
			
			break;
		}
		case INVALID_WRITE:{
			
			log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>", pcb->pid);
		
			sem_post(&contador_multiprogramacion);

			pthread_mutex_lock(&mutex_LISTA_EXIT);
			list_add(LISTA_EXIT, pcb);
			pthread_mutex_unlock(&mutex_LISTA_EXIT);
			
			break;
		}
		*/
	}
}

//POR REVISAR
t_pcb *create_pcb(char *instrucciones)
{
	//FALTA AGREGAR ATRIBUTOS AL PCB

	t_pcb *nuevoPCB = malloc(sizeof(t_pcb));

	nuevoPCB->instrucciones = string_new();
	nuevoPCB->instrucciones = string_duplicate(instrucciones);

	nuevoPCB->pc = 0;
	// nuevoPCB->recurso_solicitado = string_new();

	// nuevoPCB->primera_aparicion = true;


	pthread_mutex_lock(&mutex_PID);
	// nuevoPCB->pid = PID;
	// PID++;
	pthread_mutex_unlock(&mutex_PID);

	nuevoPCB->AX = string_new();
	nuevoPCB->BX = string_new();
	nuevoPCB->CX = string_new();
	nuevoPCB->DX = string_new();
	
	nuevoPCB->EAX = string_new();
	nuevoPCB->EBX = string_new();
	nuevoPCB->ECX = string_new();
	nuevoPCB->EDX = string_new();

	nuevoPCB->RAX = string_new();
	nuevoPCB->RBX = string_new();
	nuevoPCB->RCX = string_new();
	nuevoPCB->RDX = string_new();

	return nuevoPCB;
}

int timenow()
{
	time_t now = time(NULL);
	struct tm *local = localtime(&now);
	int hours, minutes, seconds; //

	hours = local->tm_hour;
	minutes = local->tm_min;
	seconds = local->tm_sec;

	int segundos_totales;
	segundos_totales = hours * 60 * 60 + minutes * 60 + seconds;
	return segundos_totales;
}