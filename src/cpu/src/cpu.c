/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";

t_log *MODULE_LOGGER;
char *MODULE_LOG_PATHNAME = "cpu.log";

char *MODULE_CONFIG_PATHNAME = "cpu.config";
t_config *MODULE_CONFIG;

int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

t_MemorySize PAGE_SIZE;
long timestamp;
int direccion_logica; // momentaneo hasta ver de donde la saco
t_list *tlb;          // tlb que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

// Variables para trabajar con las instrucciones
//t_Page nro_page = 0;
uint32_t value = 0;

// char *recurso = NULL;
// no sirveaca me aprece--> t_PCB new_pcb;

t_PCB PCB;
pthread_mutex_t MUTEX_PCB;

int EXECUTING = 0;
pthread_mutex_t MUTEX_EXECUTING;

e_Kernel_Interrupt KERNEL_INTERRUPT;
pthread_mutex_t MUTEX_KERNEL_INTERRUPT;

int SYSCALL_CALLED;
t_Payload *SYSCALL_INSTRUCTION;

int dir_logica_origin = 0;
int dir_logica_destination = 0;

int dir_fisica_origin = 0;
int dir_fisica_destination = 0;

int tlb_replace_index_fifo = 0;

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
    pthread_mutex_init(&MUTEX_PCB, NULL);
    pthread_mutex_init(&MUTEX_EXECUTING, NULL);
    pthread_mutex_init(&MUTEX_KERNEL_INTERRUPT, NULL);    
    pthread_mutex_init(&MUTEX_TLB, NULL);
}

void finish_mutexes(void) {
    pthread_mutex_destroy(&MUTEX_PCB);
    pthread_mutex_destroy(&MUTEX_EXECUTING);
    pthread_mutex_destroy(&MUTEX_KERNEL_INTERRUPT);    
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

    tlb = list_create();

    //Se pide a memoria el tamaño de pagina y lo setea como dato global
    ask_memory_page_size();

    char *IR;
    t_Arguments *arguments = arguments_create(MAX_CPU_INSTRUCTION_ARGUMENTS);
    e_Eviction_Reason eviction_reason = INTERRUPTION_EVICTION_REASON;
    e_CPU_OpCode cpu_opcode;
    int exit_status;

    while(1) {

        pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
            KERNEL_INTERRUPT = NONE_KERNEL_INTERRUPT;
        pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);

        SYSCALL_INSTRUCTION = payload_create();

        pthread_mutex_lock(&MUTEX_PCB);
            receive_process_dispatch(&PCB, SERVER_CPU_DISPATCH.client.fd_client);
        pthread_mutex_unlock(&MUTEX_PCB);

        pthread_mutex_lock(&MUTEX_EXECUTING);
            EXECUTING = 1;
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        log_trace(MODULE_LOGGER, "PCB recibido del proceso : %i - Ciclo de instruccion ejecutando", PCB.PID);

        while(1) {

            // Fetch
            log_debug(MINIMAL_LOGGER,"PID: %d - FETCH - Program Counter: %d", PCB.PID, PCB.PC);
            cpu_fetch_next_instruction(&IR);
            if(IR == NULL) {
                log_error(MODULE_LOGGER, "Error al fetchear la instruccion");
                exit_status = 1;
                goto check_interrupts;
            }

            // Decode
            exit_status = arguments_use(arguments, IR);
            if(exit_status) {
                switch(errno) {
                    case E2BIG:
                        log_error(MODULE_LOGGER, "%s: Demasiados argumentos en la instruccion", IR);
                        break;
                    case ENOMEM:
                        log_error(MODULE_LOGGER, "arguments_use: Error al reservar memoria para los argumentos");
                        exit(EXIT_FAILURE);
                    default:
                        log_error(MODULE_LOGGER, "arguments_use: %s", strerror(errno));
                        break;
                }
                arguments_remove(arguments);
                free(IR);
                goto check_interrupts;
            }

            if(decode_instruction(arguments->argv[0], &cpu_opcode)) {
                log_error(MODULE_LOGGER, "%s: Error al decodificar la instruccion", arguments->argv[0]);
                exit_status = 1;
                arguments_remove(arguments);
                free(IR);
                goto check_interrupts;
            }

            // Execute
            exit_status = CPU_OPERATIONS[cpu_opcode].function(arguments->argc, arguments->argv);

            arguments_remove(arguments);
            free(IR);

            check_interrupts:
                if (exit_status) {
                    log_trace(MODULE_LOGGER, "Error en la ejecucion de la instruccion");
                    eviction_reason = ERROR_EVICTION_REASON;
                    break;
                }

                if(cpu_opcode == EXIT_CPU_OPCODE) {
                    eviction_reason = EXIT_EVICTION_REASON;
                    break;
                }

                pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
                    if (KERNEL_INTERRUPT == KILL_KERNEL_INTERRUPT) {
                        pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
                        eviction_reason = INTERRUPTION_EVICTION_REASON;
                        break;
                    }
                pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);

                if (SYSCALL_CALLED) {
                    eviction_reason = SYSCALL_EVICTION_REASON;
                    break;
                }

                pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
                    if (KERNEL_INTERRUPT == QUANTUM_KERNEL_INTERRUPT) {
                        pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
                        eviction_reason = INTERRUPTION_EVICTION_REASON;
                        break;
                    }
                pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
        }

        pthread_mutex_lock(&MUTEX_EXECUTING);
            EXECUTING = 0;
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        pthread_mutex_lock(&MUTEX_PCB);
            send_process_eviction(PCB, eviction_reason, *SYSCALL_INSTRUCTION, SERVER_CPU_DISPATCH.client.fd_client);
        pthread_mutex_unlock(&MUTEX_PCB);

        payload_destroy(SYSCALL_INSTRUCTION);
    }

    arguments_destroy(arguments);
}

void *kernel_cpu_interrupt_handler(void *NULL_parameter) {

    cpu_start_server_for_kernel((void *) &SERVER_CPU_INTERRUPT);
    sem_post(&CONNECTED_KERNEL_CPU_INTERRUPT);

    e_Kernel_Interrupt kernel_interrupt;
    t_PID pid;

    while(1) {

        receive_kernel_interrupt(&kernel_interrupt, &pid, SERVER_CPU_INTERRUPT.client.fd_client);

        pthread_mutex_lock(&MUTEX_EXECUTING);
            if(!EXECUTING) {
                pthread_mutex_unlock(&MUTEX_EXECUTING);
                continue;
            }
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        pthread_mutex_lock(&MUTEX_PCB);
        if(pid == PCB.PID) {
            pthread_mutex_unlock(&MUTEX_PCB);

            pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
                // Una forma de establecer prioridad entre interrupciones que se pisan, sólo va a quedar una
                if (KERNEL_INTERRUPT < kernel_interrupt)
                    KERNEL_INTERRUPT = kernel_interrupt;
            pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
        } else {
            pthread_mutex_unlock(&MUTEX_PCB);
        }

    }

    return NULL;
}

int mmu(uint32_t dir_logica, t_PID pid, int tamanio_pagina, int register_otrigin, e_CPU_Register register_destination, int in_out)
{

    t_Page nro_page = (t_Page) floor(dir_logica / PAGE_SIZE);
    int offset = dir_logica - nro_page * PAGE_SIZE;
    int nro_frame_required = 0;
    int dir_fisica = 0;

    int required_pages = 0;

    // CHEQUEO SI ESTA EN TLB EL FRAME QUE NECESITO
    pthread_mutex_lock(&MUTEX_TLB);
    int frame_tlb = check_tlb(pid, nro_page);
    pthread_mutex_unlock(&MUTEX_TLB);

    if (frame_tlb != -1) //HIT
    {
        log_debug(MINIMAL_LOGGER, "PID: %i - TLB HIT - PAGINA: %i ", pid, nro_page);
        nro_frame_required = frame_tlb;
        log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i - Marco: %d", pid, nro_page, nro_frame_required);
        tlb_access(pid, nro_page, nro_frame_required, dir_logica, register_otrigin, register_destination, in_out);

        dir_fisica = nro_frame_required * PAGE_SIZE + offset;

        return dir_fisica;       
    }
    else //NO HAY HIT
    {
        log_debug(MINIMAL_LOGGER, "PID: %i - TLB MISS - PAGINA: %i ", pid, nro_page);
        request_frame_memory(pid, nro_page);

        t_Package *package = package_receive(CONNECTION_MEMORY.fd_connection);
        t_PID pidBuscado;
        t_Frame frame;
        payload_dequeue(package->payload, &pidBuscado, sizeof(t_PID) );
        payload_dequeue(package->payload, &frame, sizeof(t_Frame) );
        
        log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i - Marco: %i", pid, nro_page, frame);

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

        dir_fisica = nro_frame_required * PAGE_SIZE + offset;
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

        package = package_create_with_header(READ_REQUEST);
        payload_enqueue(package->payload, &(pid), sizeof(t_PID) );
        payload_enqueue(package->payload, &nro_page, sizeof(t_Page) );
        payload_enqueue(package->payload, &nro_page, sizeof(t_MemorySize) );
        package_send(package, CONNECTION_MEMORY.fd_connection);
        package_destroy(package);

        package = package_receive(CONNECTION_MEMORY.fd_connection);
        if (package == NULL)
        {
            log_error(MODULE_LOGGER, "Error al recibir el paquete");
            exit(EXIT_FAILURE);
        }  else
        {
            log_info(MODULE_LOGGER, "PID: %i -Accion: ESCRIBIR - Pagina: %i - Direccion Fisica: %i %i ", pid, nro_page, frame_number_required, direc);           
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
            log_info(MODULE_LOGGER, "PID: %i -Accion: ESCRIBIR - Pagina: %i - Direccion Fisica: %i %i ", pid, nro_page, frame_number_required, direc);
        }
    }

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
    t_TLB *tlb_aux;
    
    if (strcmp(ALGORITMO_TLB, "LRU") == 0)
    {
        tlb_aux = list_get(tlb, 0);
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
    else //CASO FIFO
    {
        
        tlb_aux = list_get(tlb, tlb_replace_index_fifo);
        tlb_aux->PID = pid;
        tlb_aux->page_number = page;
        tlb_aux->frame = frame;
        tlb_aux->time = timestamp;
        timestamp++;

        tlb_replace_index_fifo++;
        if (tlb_replace_index_fifo == list_size(tlb)) tlb_replace_index_fifo = 0;
        
    }
    
}

void request_frame_memory(t_PID pid, t_Page page)
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


void ask_memory_page_size(){
    send_header(PAGE_SIZE_REQUEST, CONNECTION_MEMORY.fd_connection);

    t_Package* package = package_receive(CONNECTION_MEMORY.fd_connection);
    payload_dequeue(package->payload, &PAGE_SIZE, sizeof(t_MemorySize) );
    package_destroy(package);
}