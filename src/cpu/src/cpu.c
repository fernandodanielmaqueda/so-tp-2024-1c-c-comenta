/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "cpu.h"

char *MODULE_NAME = "cpu";

t_log *MODULE_LOGGER;
char *MODULE_LOG_PATHNAME = "cpu.log";

char *MODULE_CONFIG_PATHNAME = "cpu.config";
t_config *MODULE_CONFIG;

int TLB_ENTRY_COUNT;

const char *TLB_ALGORITHMS[] = {
    [FIFO_TLB_ALGORITHM] = "FIFO",
    [LRU_TLB_ALGORITHM] = "LRU"
};

e_TLB_Algorithm TLB_ALGORITHM;

t_MemorySize PAGE_SIZE;
long TIMESTAMP;
t_list *TLB;          // TLB que voy a ir creando para darle valores que obtengo de la estructura de t_tlb

t_Exec_Context EXEC_CONTEXT;
pthread_mutex_t MUTEX_EXEC_CONTEXT;

int EXECUTING = 0;
pthread_mutex_t MUTEX_EXECUTING;

e_Eviction_Reason EVICTION_REASON;

e_Kernel_Interrupt KERNEL_INTERRUPT;
pthread_mutex_t MUTEX_KERNEL_INTERRUPT;

int SYSCALL_CALLED;
t_Payload SYSCALL_INSTRUCTION;

int TLB_REPLACE_INDEX_FIFO = 0;

pthread_mutex_t MUTEX_TLB;

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
    pthread_mutex_init(&(SERVER_CPU_DISPATCH.shared_list_clients.mutex), NULL);
    pthread_mutex_init(&(SERVER_CPU_INTERRUPT.shared_list_clients.mutex), NULL);

    pthread_mutex_init(&MUTEX_EXEC_CONTEXT, NULL);
    pthread_mutex_init(&MUTEX_EXECUTING, NULL);
    pthread_mutex_init(&MUTEX_KERNEL_INTERRUPT, NULL);    
    pthread_mutex_init(&MUTEX_TLB, NULL);
}

void finish_mutexes(void) {
    pthread_mutex_destroy(&(SERVER_CPU_DISPATCH.shared_list_clients.mutex));
    pthread_mutex_destroy(&(SERVER_CPU_INTERRUPT.shared_list_clients.mutex));
    
    pthread_mutex_destroy(&MUTEX_EXEC_CONTEXT);
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
    SERVER_CPU_DISPATCH = (t_Server){.server_type = CPU_DISPATCH_PORT_TYPE, .clients_type = KERNEL_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_DISPATCH"), .shared_list_clients.list = list_create()};
    SERVER_CPU_INTERRUPT = (t_Server){.server_type = CPU_INTERRUPT_PORT_TYPE, .clients_type = KERNEL_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA_INTERRUPT"), .shared_list_clients.list = list_create()};
    TLB_ENTRY_COUNT = config_get_int_value(MODULE_CONFIG, "CANTIDAD_ENTRADAS_TLB");
    if(find_tlb_algorithm(config_get_string_value(MODULE_CONFIG, "ALGORITMO_TLB"), &TLB_ALGORITHM)) {
		log_error(MODULE_LOGGER, "ALGORITMO_PLANIFICACION invalido");
		exit(EXIT_FAILURE);
	}
}

int find_tlb_algorithm(char *name, e_TLB_Algorithm *destination) {

    if(name == NULL || destination == NULL)
        return 1;
    
    size_t tlb_algorithms_number = sizeof(TLB_ALGORITHMS) / sizeof(TLB_ALGORITHMS[0]);
    for (register e_TLB_Algorithm tlb_algorithm = 0; tlb_algorithm < tlb_algorithms_number; tlb_algorithm++)
        if (strcmp(TLB_ALGORITHMS[tlb_algorithm], name) == 0) {
            *destination = tlb_algorithm;
            return 0;
        }

    return 1;
}

void instruction_cycle(void)
{

    char *IR;
    t_Arguments *arguments = arguments_create(MAX_CPU_INSTRUCTION_ARGUMENTS);
    e_CPU_OpCode cpu_opcode;
    int exit_status;

    while(1) {

        pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
            KERNEL_INTERRUPT = NONE_KERNEL_INTERRUPT;
        pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);

        payload_init(&SYSCALL_INSTRUCTION);

        pthread_mutex_lock(&MUTEX_EXEC_CONTEXT);
            receive_process_dispatch(&EXEC_CONTEXT, ((t_Client *) list_get(SERVER_CPU_DISPATCH.shared_list_clients.list, 0))->fd_client);
        pthread_mutex_unlock(&MUTEX_EXEC_CONTEXT);

        pthread_mutex_lock(&MUTEX_EXECUTING);
            EXECUTING = 1;
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        log_trace(MODULE_LOGGER, "Contexto de ejecucion recibido del proceso : %i - Ciclo de instruccion ejecutando", EXEC_CONTEXT.PID);

        while(1) {

            // Fetch
            log_debug(MINIMAL_LOGGER,"PID: %d - FETCH - Program Counter: %d", EXEC_CONTEXT.PID, EXEC_CONTEXT.PC);
            cpu_fetch_next_instruction(&IR);
            if(IR == NULL) {
                log_error(MODULE_LOGGER, "Error al fetchear la instruccion");
                EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
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
                EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
                break;
            }

            if(decode_instruction(arguments->argv[0], &cpu_opcode)) {
                log_error(MODULE_LOGGER, "%s: Error al decodificar la instruccion", arguments->argv[0]);
                arguments_remove(arguments);
                free(IR);
                EVICTION_REASON = UNEXPECTED_ERROR_EVICTION_REASON;
                break;
            }

            // Execute
            exit_status = CPU_OPERATIONS[cpu_opcode].function(arguments->argc, arguments->argv);

            arguments_remove(arguments);
            free(IR);

            if(exit_status) {
                log_trace(MODULE_LOGGER, "Error en la ejecucion de la instruccion");
                // EVICTION_REASON ya debe ser asignado por la instrucción cuando falla
                break;
            }

            if(cpu_opcode == EXIT_CPU_OPCODE) {
                EVICTION_REASON = EXIT_EVICTION_REASON;
                break;
            }

            pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
                if (KERNEL_INTERRUPT == KILL_KERNEL_INTERRUPT) {
                    pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
                    EVICTION_REASON = KILL_KERNEL_INTERRUPT_EVICTION_REASON;
                    break;
                }
            pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);

            if (SYSCALL_CALLED) {
                EVICTION_REASON = SYSCALL_EVICTION_REASON;
                break;
            }

            pthread_mutex_lock(&MUTEX_KERNEL_INTERRUPT);
                if (KERNEL_INTERRUPT == QUANTUM_KERNEL_INTERRUPT) {
                    pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
                    EVICTION_REASON = QUANTUM_KERNEL_INTERRUPT_EVICTION_REASON;
                    break;
                }
            pthread_mutex_unlock(&MUTEX_KERNEL_INTERRUPT);
        }

        pthread_mutex_lock(&MUTEX_EXECUTING);
            EXECUTING = 0;
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        pthread_mutex_lock(&MUTEX_EXEC_CONTEXT);
            send_process_eviction(EXEC_CONTEXT, EVICTION_REASON, SYSCALL_INSTRUCTION, ((t_Client *) list_get(SERVER_CPU_DISPATCH.shared_list_clients.list, 0))->fd_client);
        pthread_mutex_unlock(&MUTEX_EXEC_CONTEXT);

        payload_destroy(&SYSCALL_INSTRUCTION);
    }

    arguments_destroy(arguments);
}

void *kernel_cpu_interrupt_handler(void *NULL_parameter) {

    cpu_start_server_for_kernel((void *) &SERVER_CPU_INTERRUPT);
    sem_post(&CONNECTED_KERNEL_CPU_INTERRUPT);

    e_Kernel_Interrupt kernel_interrupt;
    t_PID pid;

    while(1) {

        receive_kernel_interrupt(&kernel_interrupt, &pid, ((t_Client *) list_get(SERVER_CPU_INTERRUPT.shared_list_clients.list, 0))->fd_client);

        pthread_mutex_lock(&MUTEX_EXECUTING);
            if(!EXECUTING) {
                pthread_mutex_unlock(&MUTEX_EXECUTING);
                continue;
            }
        pthread_mutex_unlock(&MUTEX_EXECUTING);

        pthread_mutex_lock(&MUTEX_EXEC_CONTEXT);
            if(pid == EXEC_CONTEXT.PID) {
                pthread_mutex_unlock(&MUTEX_EXEC_CONTEXT);
                continue;
            }
        pthread_mutex_unlock(&MUTEX_EXEC_CONTEXT);

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
            payload_shift(&(package->payload), &pidBuscado, sizeof(pidBuscado) );
            payload_shift(&(package->payload), &frame_number, sizeof(frame_number) );
            package_destroy(package);
            
            log_debug(MINIMAL_LOGGER, "PID: %i - OBTENER MARCO - Página: %i - Marco: %i", pid, page_number, frame_number);

            if (TLB_ENTRY_COUNT > 0) {
                if (list_size(TLB) < TLB_ENTRY_COUNT)
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

            if(TLB_ALGORITHM == LRU_TLB_ALGORITHM) {
                tlb_entry->time = TIMESTAMP;
                TIMESTAMP++;
            }

            return 0;
        }
    }

    return 1;
}

void add_to_tlb(t_PID pid , t_Page_Number page, t_Frame_Number frame) {
    t_TLB *tlb_entry = malloc(sizeof(t_TLB));
    tlb_entry->PID = pid;
    tlb_entry->page_number = page;
    tlb_entry->frame = frame;
    tlb_entry->time = TIMESTAMP;
    TIMESTAMP++;
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

    switch(TLB_ALGORITHM) {
        case LRU_TLB_ALGORITHM:
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
            tlb_aux->time = TIMESTAMP;
            TIMESTAMP++;
            break;
        }
        case FIFO_TLB_ALGORITHM:
        {
            tlb_aux = list_get(TLB, TLB_REPLACE_INDEX_FIFO);
            tlb_aux->PID = pid;
            tlb_aux->page_number = page;
            tlb_aux->frame = frame;
            tlb_aux->time = TIMESTAMP;
            TIMESTAMP++;

            TLB_REPLACE_INDEX_FIFO++;
            if (TLB_REPLACE_INDEX_FIFO == list_size(TLB))
                TLB_REPLACE_INDEX_FIFO = 0;
            break;
        }
    }   
}

void request_frame_memory(t_PID pid, t_Page_Number page) {
    t_Package *package = package_create_with_header(FRAME_REQUEST);
    payload_append(&(package->payload), &page, sizeof(page));
    payload_append(&(package->payload), &pid, sizeof(pid));
    package_send(package, CONNECTION_MEMORY.fd_connection);
}

void cpu_fetch_next_instruction(char **line) {
    send_instruction_request(EXEC_CONTEXT.PID, EXEC_CONTEXT.PC, CONNECTION_MEMORY.fd_connection);
    receive_text_with_expected_header(INSTRUCTION_REQUEST, line, CONNECTION_MEMORY.fd_connection);
}

void ask_memory_page_size(void) {
    send_header(PAGE_SIZE_REQUEST, CONNECTION_MEMORY.fd_connection);

    t_Package* package;
    package_receive(&package, CONNECTION_MEMORY.fd_connection);
    payload_shift(&(package->payload), &PAGE_SIZE, sizeof(PAGE_SIZE));
    package_destroy(package);
}

void attend_write(t_PID pid, t_list *list_physical_addresses, void *source, size_t bytes) {

    t_Package* package;

    package = package_create_with_header(WRITE_REQUEST);
    payload_append(&(package->payload), &pid, sizeof(pid));
    list_serialize(&(package->payload), *list_physical_addresses, physical_address_serialize_element);
    payload_append(&(package->payload), &bytes, sizeof(t_MemorySize));
    payload_append(&(package->payload), source, (size_t) bytes);
    package_send(package, CONNECTION_MEMORY.fd_connection);
    package_destroy(package);

    receive_expected_header(WRITE_REQUEST,CONNECTION_MEMORY.fd_connection);
    log_info(MODULE_LOGGER, "PID: %i - Accion: ESCRIBIR OK", pid);
}

void attend_read(t_PID pid, t_list *list_physical_addresses, void *destination, size_t bytes) {
    if(list_physical_addresses == NULL || destination == NULL)
        return;

    t_Package* package = package_create_with_header(READ_REQUEST);
    payload_append(&(package->payload), &(pid), sizeof(pid));
    payload_append(&(package->payload), &bytes, sizeof(t_MemorySize));
    list_serialize(&(package->payload), *list_physical_addresses, physical_address_serialize_element);          
    package_send(package, CONNECTION_MEMORY.fd_connection);
    package_destroy(package);

    package_receive(&package, CONNECTION_MEMORY.fd_connection);
    if (package == NULL) {
        log_error(MODULE_LOGGER, "Error al recibir el paquete");
        exit(EXIT_FAILURE);
    } else {
        log_info(MODULE_LOGGER, "PID: %i - Accion: LEER OK", pid);
        payload_shift(&(package->payload), &destination, bytes);
    }
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