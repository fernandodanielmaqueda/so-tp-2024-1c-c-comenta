/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *module_name = "cpu";
char *module_log_pathname = "cpu.log";
char *module_config_pathname = "cpu.config";

t_log *module_logger;
t_config *module_config;

int fd_kernel;
int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;

char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *PUERTO_ESCUCHA_DISPATCH;
char *PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

int module(int argc, char *argv[])
{

    initialize_module_logger();
    initialize_module_config();
    initialize_sockets();
    log_info(module_logger, "Modulo %s inicializado correctamente", module_name);

    instruction_cycle();

    return EXIT_SUCCESS;
}

void read_module_config(t_config *module_config)
{
    IP_MEMORIA = config_get_string_value(module_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(module_config, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(module_config, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(module_config, "PUERTO_ESCUCHA_INTERRUPT");
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(module_config, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(module_config, "ALGORITMO_TLB");
}

void initialize_sockets()
{

    // Inicializo memoria
    log_info(module_logger, "Iniciando clinte cpu para ir a memoria");
    fd_memoria = client_connect(IP_MEMORIA, PUERTO_MEMORIA);

    if (fd_memoria == -1)
    {
        log_error(module_logger, "No se pudo conectar a memoria en %s:%s", IP_MEMORIA, PUERTO_MEMORIA);
        exit(EXIT_FAILURE);
    }
    else
        log_info(module_logger, "Conectado a memoria en %s:%s", IP_MEMORIA, PUERTO_MEMORIA);

    // Dejo a CPU en modo server incializo server cpu dispatch
    fd_cpu_dispatch = start_server(NULL, PUERTO_ESCUCHA_DISPATCH);
    log_info(module_logger, "CPU en modo dispatch server escuchando en puerto %s \n", PUERTO_ESCUCHA_DISPATCH);

    // Dejo a CPU en modo server incialzio server cpu interrupt
    fd_cpu_interrupt = start_server(NULL, PUERTO_ESCUCHA_INTERRUPT);
    log_info(module_logger, "CPU en modo interrupt server escuchando en puerto %s \n", PUERTO_ESCUCHA_INTERRUPT);

    // Espero conexion  dispatch kernel
    log_info(module_logger, "Esperando conexion de kernel en puerto disptach %s \n", PUERTO_ESCUCHA_DISPATCH);
    fd_kernel = esperar_cliente(fd_cpu_dispatch);

    if (fd_kernel == -1)
    {
        log_error(module_logger, "No se pudo conectar a kernel en puerto dispatch");
        exit(EXIT_FAILURE);
    }
    else
        log_info(module_logger, "Conectado a kernel en puerto dispatch ");

    // Espero a conexion interrupt kernel
    log_info(module_logger, "Esperando conexion de kernel en puerto interrupt %s \n", PUERTO_ESCUCHA_INTERRUPT);
    fd_kernel = esperar_cliente(fd_cpu_interrupt);

    if (fd_kernel == -1)
    {
        log_error(module_logger, "No se pudo conectar a kernel en puerto interrupt");
        exit(EXIT_FAILURE);
    }
    else
        log_info(module_logger, "Conectado a kernel en puerto interrupt ");
}

void instruction_cycle()
{

    //   t_contexto* context = recibe_contexto(fd_kernel);
}

void execute(t_instruction_use *instruction, t_contexto *contexto)
{
    //size_t largo_nombre = 0;
   // uint32_t dir_logica = 0;
   // uint32_t dir_fisica = 0;
    uint32_t valor = 0;
    char *parametro = NULL;
    char *parametro2 = NULL;
   // char *recurso = NULL;
    t_register registro_destino;
    t_register registro_origen;

    // Declarame las variables que necesites para en el case de IO_GEN_SLEEP (Interfaz, Unidades de trabajo): Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.
    uint32_t unidades_trabajo = 0;
    char *interfaz = NULL;

    // contexto->pc++; // TODO: no hay problema con incrementarlo antes de ejecutar instruccion?// LO SAQIE DEL TP NO SE LA RESPUESTA AUN ..
    switch (instruction->operation)
    {
    case SET:
    {

        parametro = list_get(instruction->parameters, 0);
        parametro2 = list_get(instruction->parameters, 1);
        registro_destino = string_a_registro(parametro);
        valor = atoi(parametro2);
        contexto->registros[registro_destino] = valor;
        break;
    }

    case SUM:
    {
        parametro = list_get(instruction->parameters, 0);
        parametro2 = list_get(instruction->parameters, 1);
        registro_destino = string_a_registro(parametro);
        registro_origen = string_a_registro(parametro2);
        contexto->registros[registro_destino] = contexto->registros[registro_destino] + contexto->registros[registro_origen];
        break;
    }

    case SUB:
    {
        parametro = list_get(instruction->parameters, 0);
        parametro2 = list_get(instruction->parameters, 1);
        registro_destino = string_a_registro(parametro);
        registro_origen = string_a_registro(parametro2);
        contexto->registros[registro_destino] = contexto->registros[registro_destino] - contexto->registros[registro_origen];
        break;
    }

    case JNZ:
    {
        parametro = list_get(instruction->parameters, 0);
        parametro2 = list_get(instruction->parameters, 1);
        registro_destino = string_a_registro(parametro);
        valor = atoi(parametro2);
        if (contexto->registros[registro_destino] != 0)
        {
            contexto->pc = valor;
        }
        break;
    }

        // IO_GEN_SLEEP (Interfaz, Unidades de trabajo): Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O a que realice un sleep por una cantidad de unidades de trabajo.

        //============================== NO ESTOY SEGURO ==============================
    case IO_GEN_SLEEP:
    {
        interfaz = list_get(instruction->parameters, 0);
        unidades_trabajo = atoi(list_get(instruction->parameters, 1));
        sleep(unidades_trabajo);
    }

        // ==================================================================//////////////

    default:
    {
        log_error(module_logger, "Código %d desconocido.", instruction->operation);
        exit(EXIT_FAILURE);
    }
    }
}

t_register string_a_registro(const char *string)
{
    if (strcmp(string, "AX") == 0)
    {
        return AX;
    }
    else if (strcmp(string, "BX") == 0)
    {
        return BX;
    }
    else if (strcmp(string, "CX") == 0)
    {
        return CX;
    }
    else if (strcmp(string, "DX") == 0)
    {
        return DX;
    }
    else
    {
        log_error(module_logger, "Se intentó convertir string a registro un parámetro que no es registro.");
        exit(EXIT_FAILURE);
    }
}

/*
===============old estaba aca==========================================////////
void inicializar_ciclo_cpu() {
    // pthread_create(&hilo_ciclo_cpu, NULL, (void*)ciclo_cpu, NULL);
    // pthread_join(hilo_ciclo_cpu, NULL);
}

void ciclo_cpu(){

    while(1){
      
        t_paquete* paquete = recibir_paquete(conexion_kernel);
        if (paquete == NULL) {
            continue;
        }
        t_pcb* pcb = deserializar_pcb(paquete->buffer);

        t_list* instrucciones = parsearInstrucciones(pcb->instrucciones);

        cicloInstruccion(pcb, instrucciones);

        list_destroy(instrucciones);
     
    }

*/




