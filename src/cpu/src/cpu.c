/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";

t_log *MODULE_LOGGER;
char *MODULE_LOG_PATHNAME = "cpu.log";

char *MODULE_CONFIG_PATHNAME = "cpu.config";
t_config *MODULE_CONFIG;

// Tipos de interrupciones para el ciclo
int interruption_io;

int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

int size_pag;
long timestamp;
int direccion_logica; // momentaneo hasta ver de donde la saco
t_list *tlb;          // tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

// Variables para trabajar con las instrucciones
//t_Page nro_page = 0;
uint32_t value = 0;

// char *recurso = NULL;
// no sirveaca me aprece--> t_PCB new_pcb;

e_Kernel_Interrupt KERNEL_INTERRUPT;
int SYSCALL_CALLED;
t_PCB PCB;
t_Payload *SYSCALL_INSTRUCTION;

int dir_logica_origin = 0;
int dir_logica_destination = 0;

int dir_fisica_origin = 0;
int dir_fisica_destination = 0;

uint32_t unit_work = 0;
char *interfaz = NULL;

pthread_mutex_t MUTEX_TLB;

const char *t_interrupt_type_string[] = {
    [ERROR_EVICTION_REASON] = "ERROR_EVICTION_REASON",
    [SYSCALL_EVICTION_REASON] = "SYSCALL_EVICTION_REASON",
    [INTERRUPTION_EVICTION_REASON] = "INTERRUPTION_EVICTION_REASON"
};

int module(int argc, char *argv[])
{

    initialize_loggers();
    initialize_configs(MODULE_CONFIG_PATHNAME);
    initialize_mutexes();
	initialize_semaphores();
    initialize_sockets();

    log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

    instruction_cycle();

    // finish_threads();
    finish_sockets();
    // finish_configs();
    finish_loggers();
	finish_semaphores();
	finish_mutexes();

    return EXIT_SUCCESS;
}

void initialize_mutexes(void) {
    pthread_mutex_init(&MUTEX_TLB, NULL);
}

void finish_mutexes(void) {
    pthread_mutex_destroy(&MUTEX_TLB);
}

void initialize_semaphores(void) {
    
}

void finish_semaphores(void) {
    
}

void read_module_config(t_config *MODULE_CONFIG)
{
    CONNECTION_MEMORY = (t_Connection){.client_type = CPU_PORT_TYPE, .server_type = MEMORY_PORT_TYPE, .ip = config_get_string_value(MODULE_CONFIG, "IP_MEMORIA"), .port = config_get_string_value(MODULE_CONFIG, "PUERTO_MEMORIA")};
    SERVER_CPU_DISPATCH = (t_Single_Client_Server){.server = (t_Server){.server_type = CPU_DISPATCH_PORT_TYPE, .clients_type = KERNEL_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH")}};
    SERVER_CPU_INTERRUPT = (t_Single_Client_Server){.server = (t_Server){.server_type = CPU_INTERRUPT_PORT_TYPE, .clients_type = KERNEL_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT")}};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB");
}

void instruction_cycle(void)
{

    char *IR;
    t_Arguments *arguments = arguments_create(MAX_CPU_INSTRUCTION_ARGUMENTS, false);
    e_Eviction_Reason eviction_reason = INTERRUPTION_EVICTION_REASON;
    e_CPU_OpCode cpu_opcode;
    int exit_status;

    tlb = list_create();

    while(1) {

        receive_process_dispatch(&PCB, SERVER_CPU_DISPATCH.client.fd_client);
        log_trace(MODULE_LOGGER, "PCB recibido del proceso : %i - Ciclo de instruccion ejecutando", PCB.PID);
        
        KERNEL_INTERRUPT = NONE_KERNEL_INTERRUPT;
        SYSCALL_INSTRUCTION = payload_create();

        while(1) {

            // Fetch
            log_debug(MINIMAL_LOGGER,"PID: %d - FETCH - Program Counter: %d", PCB.PID, PCB.PC);
            cpu_fetch_next_instruction(&IR);
            if(IR == NULL) {
                log_error(MODULE_LOGGER, "Error al fetchear la instruccion");
                exit_status = 1;
            } else {
                // Decode
                arguments_add(arguments, IR);
                // FALTA VALIDAR LA SALIDA DE arguments_add
                if(decode_instruction(arguments->argv[0], &cpu_opcode)) {
                    log_error(MODULE_LOGGER, "%s: Error al decodificar la instruccion", arguments->argv[0]);
                    exit_status = 1;
                } else {
                    // Execute
                    exit_status = CPU_OPERATIONS[cpu_opcode].function(arguments->argc, arguments->argv);
                }

                arguments_remove(arguments);
                free(IR);
            }

            // Check interrupts
            if (exit_status) {
                log_trace(MODULE_LOGGER, "Error en la ejecucion de la instruccion");
                eviction_reason = ERROR_EVICTION_REASON;
                break;
            }

            if(cpu_opcode == EXIT_CPU_OPCODE) {
                eviction_reason = EXIT_EVICTION_REASON;
                break;
            }

            if (KERNEL_INTERRUPT == KILL_KERNEL_INTERRUPT) {
                eviction_reason = INTERRUPTION_EVICTION_REASON;
                break;
            }

            if (SYSCALL_CALLED) {
                eviction_reason = SYSCALL_EVICTION_REASON;
                break;
            }

            if (KERNEL_INTERRUPT == QUANTUM_KERNEL_INTERRUPT) {
                eviction_reason = INTERRUPTION_EVICTION_REASON;
                break;
            }
        }

        send_process_eviction(PCB, eviction_reason, *SYSCALL_INSTRUCTION, SERVER_CPU_DISPATCH.client.fd_client);

        payload_destroy(SYSCALL_INSTRUCTION);
    }
}

void *kernel_cpu_interrupt_handler(void *NULL_parameter)
{

    cpu_start_server_for_kernel((void *) &SERVER_CPU_INTERRUPT);
    sem_post(&CONNECTED_KERNEL_CPU_INTERRUPT);

    e_Kernel_Interrupt kernel_interrupt;
    t_PID pid;

    while(1) {

        receive_kernel_interrupt(&kernel_interrupt, &pid, SERVER_CPU_INTERRUPT.client.fd_client);

        // Una forma de establecer prioridad entre interrupciones que se pisan, sólo va a quedar una
        if (KERNEL_INTERRUPT < kernel_interrupt)
            KERNEL_INTERRUPT = kernel_interrupt;
    }

    return NULL;
}

int mmu(uint32_t dir_logica, t_PID pid, int tamanio_pagina, int register_otrigin, int register_destination, int in_out)
{

    t_Page nro_page = (t_Page) floor(dir_logica / tamanio_pagina);
    int offset = dir_logica - nro_page * tamanio_pagina;
    int nro_frame_required = 0;
    int dir_fisica = 0;

    // CHEQUEO SI ESTA EN TLB EL FRAME QUE NECESITO
    pthread_mutex_lock(&MUTEX_TLB);
    int frame_tlb = check_tlb(pid, nro_page);
    pthread_mutex_unlock(&MUTEX_TLB);

    if (frame_tlb != -1)
    {
        nro_frame_required = frame_tlb;
        log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i", pid, nro_page);
        log_debug(MINIMAL_LOGGER, "PID: %i - TLB HIT - PAGINA: %i ", pid, nro_page);
        tlb_access(pid, nro_page, nro_frame_required, dir_logica, register_otrigin, register_destination, in_out);

        dir_fisica = nro_frame_required * tamanio_pagina + offset;

        return dir_fisica;       
    }
    else
    {
        request_frame_memory(pid, nro_page);
        //Obtener Marco: “PID: <PID> - OBTENER MARCO - Página: <NUMERO_PAGINA> - Marco: <NUMERO_MARCO>”.
        log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i", pid, nro_page);

        t_Package *package = package_receive(CONNECTION_MEMORY.fd_connection);
        t_PID pidBuscado;
        t_Frame frame;
        payload_dequeue(package->payload, &pidBuscado, sizeof(t_PID) );
        payload_dequeue(package->payload, &frame, sizeof(t_Frame) );

        if (CANTIDAD_ENTRADAS_TLB > 0)
        {

            if (list_size(tlb) < CANTIDAD_ENTRADAS_TLB)
            {

                add_to_tlb(pid, nro_page, frame);
                log_trace(MODULE_LOGGER, "Agrego entrada a la TLB");
            }
            else
            {

                replace_tlb_input(pid, nro_page, frame);
                log_trace(MODULE_LOGGER, "Reemplazo entrada a la TLB");
            }

            log_debug(MINIMAL_LOGGER, "PID: %i - TLB MISS - PAGINA: %i", pid, nro_page);
            package_destroy(package);
        }

        dir_fisica = nro_frame_required * tamanio_pagina + offset;
        return dir_fisica;
    }
}

int check_tlb(t_PID process_id, t_Page page_number)
{

    t_TLB *tlb_entry = NULL;
    t_Frame frame_number = -1;
    for (int i = 0; i < list_size(tlb); i++)
    {

        tlb_entry = list_get(tlb, i);
        if (tlb_entry->PID == process_id && tlb_entry->page_number == page_number)
        {
            frame_number = tlb_entry->frame;

            if (strcmp(ALGORITMO_TLB, "LRU") == 0)
            {
                tlb_entry->time = timestamp;
                timestamp++;
            }
        }
    }
    return frame_number;
}

void tlb_access(t_PID pid, t_Page nro_page, int frame_number_required, int direc, int register_origin, int register_destination, int in_out)
{

    t_Package* package;

    if (in_out == IN)
    {

        package = package_create_with_header(WRITE_REQUEST);
        payload_enqueue(package->payload, &(pid), sizeof(t_PID) );
        payload_enqueue(package->payload, &nro_page, sizeof(t_Page) );
        package_send(package, CONNECTION_MEMORY.fd_connection);
        package_destroy(package);

        package = package_receive(CONNECTION_MEMORY.fd_connection);
        if (package == NULL)
        {
            log_error(MODULE_LOGGER, "Error al recibir el paquete");
            exit(EXIT_FAILURE);
        }  else
        {
            log_info(MODULE_LOGGER, "PID: %i -Accion:ESCRIBIR  - Pagina: %i - Direccion Fisica: %i %i ", pid, nro_page, frame_number_required, direc);
           
        }
        
    }                
      else //out
    {

        package = package_create_with_header(WRITE_REQUEST);
        payload_enqueue(package->payload, &(pid), sizeof(t_PID) );
        payload_enqueue(package->payload, &nro_page, sizeof(t_Page) );
        package_send(package, CONNECTION_MEMORY.fd_connection);
        package_destroy(package);

        package = package_receive(CONNECTION_MEMORY.fd_connection);
        if (package == NULL)
        {
            log_error(MODULE_LOGGER, "Error al recibir el paquete");
            exit(EXIT_FAILURE);
        }  else
        {
            log_info(MODULE_LOGGER, "PID: %i -Accion:ESCRIBIR - Pagina: %i - Direccion Fisica: %i %i ", pid, nro_page, frame_number_required, direc);
        }
    }

   // package_destroy(package);
}


void add_to_tlb(t_PID pid , t_Page page, t_Frame frame)
{
    t_TLB *tlb_entry = malloc(sizeof(t_TLB));
    tlb_entry->PID = pid;
    tlb_entry->page_number = page;
    tlb_entry->frame = frame;
    tlb_entry->time = timestamp;
    timestamp++;
    list_add(tlb, tlb_entry);

}

void replace_tlb_input(t_PID pid, t_Page page, t_Page frame)
{
    t_TLB *tlb_aux = list_get(tlb, 0);
    int replace_value = tlb_aux->time + 1;
    int index_replace = 0;
        for(int i = 0; i < list_size(tlb); i++){

            t_TLB *replaced_tlb = list_get(tlb, i);
            if(replaced_tlb->time < replace_value){
                replace_value = replaced_tlb->time;
                index_replace = i;
            } //guardo el d emenor tiempo
        }

    tlb_aux = list_get(tlb, index_replace);
    tlb_aux->PID = pid;
    tlb_aux->page_number = page;
    tlb_aux->frame = frame;
    tlb_aux->time = timestamp;
    timestamp++;
    
}

void request_frame_memory(t_Page page, t_PID pid)
{
    t_Package *package = package_create_with_header(FRAME_REQUEST);
    payload_enqueue(package->payload, &page, sizeof(t_Page));
    payload_enqueue(package->payload, &pid, sizeof(t_PID));
    package_send(package, CONNECTION_MEMORY.fd_connection);
}



void cpu_fetch_next_instruction(char **line)
{
    send_instruction_request(PCB.PID, PCB.PC, CONNECTION_MEMORY.fd_connection);
    receive_text_with_header(INSTRUCTION_REQUEST, line, CONNECTION_MEMORY.fd_connection);
}