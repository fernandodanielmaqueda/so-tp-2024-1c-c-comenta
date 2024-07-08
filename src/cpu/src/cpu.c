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
t_Logical_Address direccion_logica; // momentaneo hasta ver de donde la saco
t_list *TLB;          // TLB que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

t_PCB PCB;
pthread_mutex_t MUTEX_PCB;

int EXECUTING = 0;
pthread_mutex_t MUTEX_EXECUTING;

e_Kernel_Interrupt KERNEL_INTERRUPT;
pthread_mutex_t MUTEX_KERNEL_INTERRUPT;

int SYSCALL_CALLED;
t_Payload *SYSCALL_INSTRUCTION;

int tlb_replace_index_fifo = 0;

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

    TLB = list_create();

    //Se pide a memoria el tamaño de pagina y lo setea como dato global
    ask_memory_page_size();

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
    pthread_mutex_init(&(SERVER_CPU_DISPATCH.mutex_clients), NULL);
    pthread_mutex_init(&(SERVER_CPU_INTERRUPT.mutex_clients), NULL);

    pthread_mutex_init(&MUTEX_PCB, NULL);
    pthread_mutex_init(&MUTEX_EXECUTING, NULL);
    pthread_mutex_init(&MUTEX_KERNEL_INTERRUPT, NULL);    
    pthread_mutex_init(&MUTEX_TLB, NULL);
}

void finish_mutexes(void) {
    pthread_mutex_destroy(&(SERVER_CPU_DISPATCH.mutex_clients));
    pthread_mutex_destroy(&(SERVER_CPU_INTERRUPT.mutex_clients));
    
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
    SERVER_CPU_DISPATCH = (t_Server){.server_type = CPU_DISPATCH_PORT_TYPE, .clients_type = KERNEL_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), .clients = list_create()};
    SERVER_CPU_INTERRUPT = (t_Server){.server_type = CPU_INTERRUPT_PORT_TYPE, .clients_type = KERNEL_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), .clients = list_create()};
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB");
}

void instruction_cycle(void)
{

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
            receive_process_dispatch(&PCB, ((t_Client *) list_get(SERVER_CPU_DISPATCH.clients, 0))->fd_client);
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
                eviction_reason = ERROR_EVICTION_REASON;
                break;
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
                eviction_reason = ERROR_EVICTION_REASON;
                break;
            }

            if(decode_instruction(arguments->argv[0], &cpu_opcode)) {
                log_error(MODULE_LOGGER, "%s: Error al decodificar la instruccion", arguments->argv[0]);
                arguments_remove(arguments);
                free(IR);
                eviction_reason = ERROR_EVICTION_REASON;
                break;
            }

            // Execute
            exit_status = CPU_OPERATIONS[cpu_opcode].function(arguments->argc, arguments->argv);

            arguments_remove(arguments);
            free(IR);

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
            send_process_eviction(PCB, eviction_reason, *SYSCALL_INSTRUCTION, ((t_Client *) list_get(SERVER_CPU_DISPATCH.clients, 0))->fd_client);
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

        receive_kernel_interrupt(&kernel_interrupt, &pid, ((t_Client *) list_get(SERVER_CPU_INTERRUPT.clients, 0))->fd_client);

        pthread_mutex_lock(&MUTEX_EXECUTING);
            if(!EXECUTING) {
                pthread_mutex_unlock(&MUTEX_EXECUTING);
                continue;
            }
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        pthread_mutex_lock(&MUTEX_PCB);
            if(pid == PCB.PID) {
                pthread_mutex_unlock(&MUTEX_PCB);
                continue;
            }
        pthread_mutex_unlock(&MUTEX_PCB);

        pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
            // Una forma de establecer prioridad entre interrupciones que se pisan, sólo va a quedar una
            if (KERNEL_INTERRUPT < kernel_interrupt)
                KERNEL_INTERRUPT = kernel_interrupt;
        pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);

    }

    return NULL;
}

t_list *mmu(t_PID pid, t_Logical_Address logical_address, size_t bytes) {
    t_Page_Number page_number = (t_Page_Number) floor(logical_address / PAGE_SIZE);
    t_Offset offset = (t_Offset) (logical_address - page_number * PAGE_SIZE);

    t_Frame_Number frame_number;
    t_Physical_Address physical_address;

    t_list *list_physical_addresses = list_create();
    t_Page_Quantity required_pages = seek_quantity_pages_required(logical_address, bytes);

    for(size_t i = 0; i < required_pages; i++) {
        page_number += i;

        // CHEQUEO SI ESTA EN TLB EL FRAME QUE NECESITO
        pthread_mutex_lock(&MUTEX_TLB);
        if(check_tlb(pid, page_number, &frame_number)) { // NO HAY HIT
            pthread_mutex_unlock(&MUTEX_TLB);

            log_debug(MINIMAL_LOGGER, "PID: %i - TLB MISS - PAGINA: %i ", pid, page_number);

            request_frame_memory(pid, page_number);

            t_Package *package;
            package_receive(&package, CONNECTION_MEMORY.fd_connection);
            t_PID pidBuscado;
            payload_dequeue(package->payload, &pidBuscado, sizeof(t_PID) );
            payload_dequeue(package->payload, &frame_number, sizeof(t_Frame_Number) );
            package_destroy(package);
            
            log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i - Marco: %i", pid, page_number, frame_number);

            if (CANTIDAD_ENTRADAS_TLB > 0) {
                if (list_size(TLB) < CANTIDAD_ENTRADAS_TLB)
                {
                    add_to_tlb(pid, page_number, frame_number);
                    log_trace(MODULE_LOGGER, "Agrego entrada a la TLB");
                }
                else
                {
                    replace_tlb_input(pid, page_number, frame_number);
                    log_trace(MODULE_LOGGER, "Reemplazo entrada a la TLB");
                }
            }

        } else { // HAY HIT
            pthread_mutex_unlock(&MUTEX_TLB);

            log_debug(MINIMAL_LOGGER, "PID: %i - TLB HIT - PAGINA: %i ", pid, page_number);
            log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i - Marco: %d", pid, page_number, frame_number);

        }

        physical_address = frame_number * PAGE_SIZE + offset;

        if(offset)
            offset = 0; //El offset solo es importante en la 1ra pagina buscada

        list_add(list_physical_addresses, &physical_address);
    }
    
    return list_physical_addresses;
}

int check_tlb(t_PID process_id, t_Page_Number page_number, t_Frame_Number *destination) {

    t_TLB *tlb_entry = NULL;
    for (int i = 0; i < list_size(TLB); i++) {

        tlb_entry = (t_TLB *) list_get(TLB, i);
        if (tlb_entry->PID == process_id && tlb_entry->page_number == page_number) {
            *destination = tlb_entry->frame;

            if (strcmp(ALGORITMO_TLB, "LRU") == 0) {
                tlb_entry->time = timestamp;
                timestamp++;
            }

            return 0;
        }
    }

    return 1;
}

void tlb_access(t_PID pid, t_Page_Number nro_page, t_Frame_Number frame_number_required, t_Physical_Address direc, e_In_Out in_out) {

    t_Package* package;

    if (in_out == IN) {

        package = package_create_with_header(READ_REQUEST);
        payload_enqueue(package->payload, &(pid), sizeof(t_PID) );
        payload_enqueue(package->payload, &nro_page, sizeof(t_Page_Number) );
        payload_enqueue(package->payload, &nro_page, sizeof(t_MemorySize) );
        package_send(package, CONNECTION_MEMORY.fd_connection);
        package_destroy(package);

        package_receive(&package, CONNECTION_MEMORY.fd_connection);
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
        payload_enqueue(package->payload, &nro_page, sizeof(t_Page_Number) );
        package_send(package, CONNECTION_MEMORY.fd_connection);
        package_destroy(package);

        package_receive(&package, CONNECTION_MEMORY.fd_connection);
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

void add_to_tlb(t_PID pid , t_Page_Number page, t_Frame_Number frame) {
    t_TLB *tlb_entry = malloc(sizeof(t_TLB));
    tlb_entry->PID = pid;
    tlb_entry->page_number = page;
    tlb_entry->frame = frame;
    tlb_entry->time = timestamp;
    timestamp++;
    list_add(TLB, tlb_entry);
}

void delete_tlb_entry_by_pid_on_resizing(t_PID pid, int resize_number) {
    t_TLB *tlb_entry;
    int size = list_size(TLB);

    if(size != 0){

        for (size_t i = (size -1); i != -1; i--)
        {
            tlb_entry = list_get(TLB, i);
            if((tlb_entry->PID == pid) && (tlb_entry->page_number >= (resize_number -1))){
                list_remove(TLB, i);
                free(tlb_entry);
            }
        }
    }
}

void delete_tlb_entry_by_pid_deleted(t_PID pid) {
    t_TLB *tlb_entry;
    int size = list_size(TLB);

    if(size != 0) {

        for (size_t i = (size -1); i != -1; i--) {
            tlb_entry = list_get(TLB, i);
            if(tlb_entry->PID == pid) {
                list_remove(TLB, i);
                free(tlb_entry);
            }
        }
    }
}

void replace_tlb_input(t_PID pid, t_Page_Number page, t_Frame_Number frame) {
    t_TLB *tlb_aux;
    
    if (strcmp(ALGORITMO_TLB, "LRU") == 0)
    {
        tlb_aux = list_get(TLB, 0);
        int replace_value = tlb_aux->time + 1;
        int index_replace = 0;
            for(int i = 0; i < list_size(TLB); i++){

                t_TLB *replaced_tlb = list_get(TLB, i);
                if(replaced_tlb->time < replace_value){
                    replace_value = replaced_tlb->time;
                    index_replace = i;
                } //guardo el d emenor tiempo
            }

        tlb_aux = list_get(TLB, index_replace);
        tlb_aux->PID = pid;
        tlb_aux->page_number = page;
        tlb_aux->frame = frame;
        tlb_aux->time = timestamp;
        timestamp++;
    }
    else //CASO FIFO
    {
        tlb_aux = list_get(TLB, tlb_replace_index_fifo);
        tlb_aux->PID = pid;
        tlb_aux->page_number = page;
        tlb_aux->frame = frame;
        tlb_aux->time = timestamp;
        timestamp++;

        tlb_replace_index_fifo++;
        if (tlb_replace_index_fifo == list_size(TLB)) tlb_replace_index_fifo = 0;
    }
    
}

void request_frame_memory(t_PID pid, t_Page_Number page) {
    t_Package *package = package_create_with_header(FRAME_REQUEST);
    payload_enqueue(package->payload, &page, sizeof(t_Page_Number));
    payload_enqueue(package->payload, &pid, sizeof(t_PID));
    package_send(package, CONNECTION_MEMORY.fd_connection);
}

void cpu_fetch_next_instruction(char **line) {
    send_instruction_request(PCB.PID, PCB.PC, CONNECTION_MEMORY.fd_connection);
    receive_text_with_expected_header(INSTRUCTION_REQUEST, line, CONNECTION_MEMORY.fd_connection);
}


void ask_memory_page_size(void) {
    send_header(PAGE_SIZE_REQUEST, CONNECTION_MEMORY.fd_connection);

    t_Package* package;
    package_receive(&package, CONNECTION_MEMORY.fd_connection);
    payload_dequeue(package->payload, &PAGE_SIZE, sizeof(t_MemorySize) );
    package_destroy(package);
}

void attend_write(t_PID pid, t_list *list_physical_addresses, size_t bytes, u_int32_t contenido) {

    t_Package* package;

    package = package_create_with_header(WRITE_REQUEST);
    payload_enqueue(package->payload, &(pid), sizeof(t_PID) );
    payload_enqueue(package->payload, &bytes, sizeof(t_MemorySize) );
    payload_enqueue(package->payload, &contenido, (size_t) bytes );
    list_serialize(package->payload, *list_physical_addresses, physical_address_serialize_element);      
    package_send(package, CONNECTION_MEMORY.fd_connection);
    package_destroy(package);

    receive_expected_header(WRITE_REQUEST,CONNECTION_MEMORY.fd_connection);
    log_info(MODULE_LOGGER, "PID: %i -Accion: ESCRIBIR OK", pid);
    
}

void attend_read(t_PID pid, t_list *list_physical_addresses, size_t bytes, e_CPU_Register register_destination) {
    t_Package* package;
    void* leido;

    package = package_create_with_header(READ_REQUEST);
    payload_enqueue(package->payload, &(pid), sizeof(t_PID) );
    payload_enqueue(package->payload, &bytes, sizeof(t_MemorySize) );
    list_serialize(package->payload, *list_physical_addresses, physical_address_serialize_element);          
    package_send(package, CONNECTION_MEMORY.fd_connection);
    package_destroy(package);

    package_receive(&package, CONNECTION_MEMORY.fd_connection);

    log_info(MODULE_LOGGER, "PID: %i - Accion: LEER OK", pid);

    payload_dequeue(package->payload, leido, bytes);
    set_register_value(&PCB, register_destination, (uint32_t)leido);        
    
    package_destroy(package);
    
}

t_Page_Quantity seek_quantity_pages_required(t_Logical_Address dir_log, size_t bytes){
    t_Page_Quantity quantity_pages = 0;

    t_Page_Number nro_page = (t_Page_Number) floor(dir_log / PAGE_SIZE);
    t_Offset offset = (t_Offset) (dir_log - nro_page * PAGE_SIZE);;

    if (offset != 0)
    {
        bytes -= (PAGE_SIZE - offset);
        quantity_pages++;
    }

    quantity_pages += (t_Page_Quantity) floor(bytes / PAGE_SIZE);
    
    return quantity_pages;
}