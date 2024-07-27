
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "memoria.h"

char *MODULE_NAME = "memoria";

t_log *MODULE_LOGGER;
char *MODULE_LOG_PATHNAME = "memoria.log";

char *MODULE_CONFIG_PATHNAME = "memoria.config";

t_config *MODULE_CONFIG;

void *MAIN_MEMORY;

t_list *LIST_PROCESSES;
t_list *LIST_FRAMES;
t_list *LIST_FREE_FRAMES;

pthread_mutex_t MUTEX_MAIN_MEMORY;
pthread_mutex_t MUTEX_LIST_FREE_FRAMES;

t_MemorySize TAM_MEMORIA;
t_MemorySize TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs(MODULE_CONFIG_PATHNAME);
    initialize_mutexes();
	initialize_semaphores();

    SHARED_LIST_CLIENTS_IO.list = list_create();

    MAIN_MEMORY = (void *) malloc(TAM_MEMORIA);
    memset(MAIN_MEMORY, (uint32_t) '0', TAM_MEMORIA); //Llena de 0's el espacio de memoria
    LIST_PROCESSES = list_create();
    create_frames();

    initialize_sockets();

    log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

    listen_kernel();

	//finish_threads();
    free_memory();
	finish_sockets();
	//finish_configs();
	finish_loggers();
	finish_semaphores();
	finish_mutexes();

   return EXIT_SUCCESS;
}

void initialize_mutexes(void) {
    pthread_mutex_init(&(SHARED_LIST_CLIENTS_IO.mutex), NULL);
    pthread_mutex_init(&MUTEX_MAIN_MEMORY, NULL);
    pthread_mutex_init(&MUTEX_LIST_FREE_FRAMES, NULL);
}

void finish_mutexes(void) {
    pthread_mutex_destroy(&(SHARED_LIST_CLIENTS_IO.mutex));
    pthread_mutex_destroy(&(MUTEX_MAIN_MEMORY));
    pthread_mutex_destroy(&(MUTEX_LIST_FREE_FRAMES));
}

void initialize_semaphores(void) {
    
}

void finish_semaphores(void) {
    
}

void read_module_config(t_config* MODULE_CONFIG) {
    SERVER_MEMORY = (t_Server) {.server_type = MEMORY_PORT_TYPE, .clients_type = TO_BE_IDENTIFIED_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA")};
    TAM_MEMORIA = (t_MemorySize) config_get_int_value(MODULE_CONFIG, "TAM_MEMORIA");
    TAM_PAGINA = (t_MemorySize) config_get_int_value(MODULE_CONFIG, "TAM_PAGINA");

    PATH_INSTRUCCIONES = config_get_string_value(MODULE_CONFIG, "PATH_INSTRUCCIONES");
        if(PATH_INSTRUCCIONES[0]) {

            size_t length = strlen(PATH_INSTRUCCIONES);
            if(PATH_INSTRUCCIONES[length - 1] == '/') {
                PATH_INSTRUCCIONES[length - 1] = '\0';
            }

            DIR *dir = opendir(PATH_INSTRUCCIONES);
            if(dir == NULL) {
                log_error(MODULE_LOGGER, "No se pudo abrir el directorio de instrucciones.");
                // TODO
                exit(EXIT_FAILURE);
            }
            // closedir(dir);
        }

    RETARDO_RESPUESTA = config_get_int_value(MODULE_CONFIG, "RETARDO_RESPUESTA");
}

void listen_kernel(void) {

    t_Package* package;
    
    while(1) {
        if(package_receive(&package, CLIENT_KERNEL->fd_client)) {
            // TODO

            pthread_cancel(SERVER_MEMORY.thread_server);
            pthread_join(SERVER_MEMORY.thread_server, NULL);
            close(SERVER_MEMORY.fd_listen);

            // ESTE ES EL HILO PRINCIPAL
            // pthread_cancel(CLIENT_KERNEL->thread_client_handler);
            // pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
            close(CLIENT_KERNEL->fd_client);

            pthread_cancel(CLIENT_CPU->thread_client_handler);
            pthread_join(CLIENT_CPU->thread_client_handler, NULL);
            close(CLIENT_CPU->fd_client);
            exit(1);
        }
        switch(package->header) {
            case PROCESS_CREATE_HEADER:
                log_info(MODULE_LOGGER, "KERNEL: Proceso nuevo recibido.");
                create_process(&(package->payload));
                break;
                
            case PROCESS_DESTROY_HEADER:
                log_info(MODULE_LOGGER, "KERNEL: Proceso finalizado recibido.");
                kill_process(&(package->payload));
                break;
            
            default:
                log_warning(MODULE_LOGGER, "%s: Header desconocido (%d)", "", package->header);
                break;
        }
        package_destroy(package);
    }
}

void create_process(t_Payload *process_data) {

    char *argument_path, *target_path;
    t_Return_Value flag_relative_path;
    t_Process *new_process = malloc(sizeof(t_Process));
    t_list *instructions_list = list_create();
    //t_list *pages_table = list_create();

    payload_shift(process_data, &(new_process->PID), sizeof(new_process->PID));
    text_deserialize(process_data, &(argument_path));
    return_value_deserialize(process_data, &flag_relative_path);

    if(!flag_relative_path) {
        // Ruta absoluta
        target_path = argument_path;
    } else {
        // Ruta relativa
        target_path = malloc((PATH_INSTRUCCIONES[0] ? (strlen(PATH_INSTRUCCIONES) + 1) : 0) + strlen(argument_path) + 1);
        if(target_path == NULL) {
            log_error(MODULE_LOGGER, "malloc: No se pudo reservar memoria para la ruta relativa.");
            exit(EXIT_FAILURE);
        }

        register int i;
        for(i = 0; PATH_INSTRUCCIONES[i]; i++) {
            target_path[i] = PATH_INSTRUCCIONES[i];
        }

        if(PATH_INSTRUCCIONES[0])
            target_path[i++] = '/';

        register int j;
        for(j = 0; argument_path[j]; j++) {
            target_path[i + j] = argument_path[j];
        }

        target_path[i + j] = '\0';
    }
    log_debug(MODULE_LOGGER, "Archivo Buscado: %s", target_path);

    //CREAR LISTA INST CON EL PARSER
    if(parser_file(target_path, instructions_list)) {
        //ENVIAR RTA ERROR A KERNEL
        if(send_return_value_with_header(PROCESS_CREATE_HEADER, 1, CLIENT_KERNEL->fd_client)) {
            // TODO

            pthread_cancel(SERVER_MEMORY.thread_server);
            pthread_join(SERVER_MEMORY.thread_server, NULL);
            close(SERVER_MEMORY.fd_listen);

            // ESTE ES EL HILO PRINCIPAL
            // pthread_cancel(CLIENT_KERNEL->thread_client_handler);
            // pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
            close(CLIENT_KERNEL->fd_client);

            pthread_cancel(CLIENT_CPU->thread_client_handler);
            pthread_join(CLIENT_CPU->thread_client_handler, NULL);
            close(CLIENT_CPU->fd_client);
            exit(1);
        }
        return;
    }

    new_process->number_of_instructions = list_size(instructions_list);
    new_process->instructions_list = instructions_list;
    new_process->pages_table = list_create();
    list_add(LIST_PROCESSES, new_process);
    
    log_debug(MODULE_LOGGER, "Archivo leido: %s", target_path);

    //ENVIAR RTA OK A KERNEL
    if(send_return_value_with_header(PROCESS_CREATE_HEADER, 0, CLIENT_KERNEL->fd_client)) {
        // TODO

        pthread_cancel(SERVER_MEMORY.thread_server);
        pthread_join(SERVER_MEMORY.thread_server, NULL);
        close(SERVER_MEMORY.fd_listen);

        // ESTE ES EL HILO PRINCIPAL
        // pthread_cancel(CLIENT_KERNEL->thread_client_handler);
        // pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
        close(CLIENT_KERNEL->fd_client);

        pthread_cancel(CLIENT_CPU->thread_client_handler);
        pthread_join(CLIENT_CPU->thread_client_handler, NULL);
        close(CLIENT_CPU->fd_client);
        exit(1);
    }
    free(target_path);
}

void kill_process(t_Payload *payload) {

    t_PID pid;
    payload_shift(payload, &pid, sizeof(pid));

    t_Process *process = seek_process_by_pid(pid);
    t_Page *paginaBuscada;
    
    int size = list_size(process->pages_table);
    for (; size > 0 ; size--)
    {
        paginaBuscada = list_get(process->pages_table, size - 1);
        t_Frame *marco = list_get(LIST_FRAMES, paginaBuscada->assigned_frame);
        list_add(LIST_FREE_FRAMES, marco);
        free(paginaBuscada);
    }
    free(process);
    
    log_debug(MINIMAL_LOGGER, "PID: <%" PRIu16 "> - Tamaño: <%" PRIu32">", pid, size);
    
    //ENVIAR RTA OK A KERNEL
    if(send_return_value_with_header(PROCESS_DESTROY_HEADER, 0, CLIENT_KERNEL->fd_client)) {
        // TODO

        pthread_cancel(SERVER_MEMORY.thread_server);
        pthread_join(SERVER_MEMORY.thread_server, NULL);
        close(SERVER_MEMORY.fd_listen);

        // ESTE ES EL HILO PRINCIPAL
        // pthread_cancel(CLIENT_KERNEL->thread_client_handler);
        // pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
        close(CLIENT_KERNEL->fd_client);

        pthread_cancel(CLIENT_CPU->thread_client_handler);
        pthread_join(CLIENT_CPU->thread_client_handler, NULL);
        close(CLIENT_CPU->fd_client);
        exit(1);
    }
    
}

int parser_file(char* path, t_list *list_instruction) {

    FILE* file;
    if ((file = fopen(path, "r")) == NULL) {
        log_warning(MODULE_LOGGER, "%s: No se pudo abrir el archivo de pseudocodigo indicado.", path);
        return 1;
    }

    char *line = NULL, *subline;
    size_t length;
    ssize_t nread;

    while(1) {

        errno = 0;
        if((nread = getline(&line, &length, file)) == -1) {
            if(errno) {
                log_warning(MODULE_LOGGER, "Funcion getline: %s", strerror(errno));
                free(line);
                exit(EXIT_FAILURE);
            }

            // Se terminó de leer el archivo
            break;
        }

        // Ignora líneas en blanco
        subline = strip_whitespaces(line);

        if(*subline) {
            // Se leyó una línea con contenido
            list_add(list_instruction, strdup(subline));
        }
    }

    free(line);       
    fclose(file);
    return 0;
}

void listen_cpu(void) {
    t_Package *package;

    while(1) {
        if(package_receive(&package, CLIENT_CPU->fd_client)) {
            // TODO

            pthread_cancel(SERVER_MEMORY.thread_server);
            pthread_join(SERVER_MEMORY.thread_server, NULL);
            close(SERVER_MEMORY.fd_listen);

            pthread_cancel(CLIENT_KERNEL->thread_client_handler);
            pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
            close(CLIENT_KERNEL->fd_client);

            
            // ES ESTE HILO
            // pthread_cancel(CLIENT_CPU->thread_client_handler);
            // pthread_join(CLIENT_CPU->thread_client_handler, NULL);
            close(CLIENT_CPU->fd_client);
            exit(1);
        }
        switch (package->header) {
            case INSTRUCTION_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de instruccion recibido.");
                seek_instruccion(&(package->payload));
                package_destroy(package);
                break;
                
            case FRAME_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de frame recibido.");
                respond_frame_request(&(package->payload));
                package_destroy(package);
                break;

            /*
                log_warning(MODULE_LOGGER, "Se desconecto CPU.");
                log_destroy(MODULE_LOGGER);
                return;
            */
                
            case PAGE_SIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                package_destroy(package);

                package = package_create_with_header(PAGE_SIZE_REQUEST);
                payload_append(&(package->payload), &TAM_PAGINA, sizeof(TAM_PAGINA));
                package_send(package, CLIENT_CPU->fd_client);
                package_destroy(package);

                break;

            case RESIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                resize_process(&(package->payload));
                package_destroy(package);
                break;
                
            case READ_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de lectura recibido.");
                read_memory(&(package->payload), CLIENT_CPU->fd_client);
                package_destroy(package);
                break;
                
            case WRITE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de lectura recibido.");
                write_memory(&(package->payload), CLIENT_CPU->fd_client);
                package_destroy(package);
                break;
            
            default:
                log_warning(MODULE_LOGGER, "%s: Header desconocido (%d)", "", package->header);
                package_destroy(package);
                break;
        }
    }
}

void listen_io(t_Client *client) {
    t_Package *package;

    while(1) {
        if(package_receive(&package, client->fd_client)) {
            log_warning(MODULE_LOGGER, "Terminada conexion con [Cliente] Entrada/Salida");

            pthread_mutex_lock(&(SHARED_LIST_CLIENTS_IO.mutex));
                list_remove_by_condition_with_comparation(SHARED_LIST_CLIENTS_IO.list, (bool (*)(void *, void *)) client_matches_pthread, (void *) &(client->thread_client_handler));
            pthread_mutex_unlock(&(SHARED_LIST_CLIENTS_IO.mutex));

            close(client->fd_client);
            free(client);

            return;
        }
        switch(package->header) {

            case IO_STDIN_WRITE_MEMORY:
                log_info(MODULE_LOGGER, "IO: Nueva peticion STDIN_IO (write) recibido.");
                write_memory(&(package->payload), client->fd_client);
                break;
            
            case IO_STDOUT_READ_MEMORY:
                log_info(MODULE_LOGGER, "IO: Nueva peticion STDOUT_IO (read) recibido.");
                read_memory(&(package->payload), client->fd_client);
                break;
            
            case IO_FS_READ_MEMORY:
                log_info(MODULE_LOGGER, "IO: Nueva peticion STDOUT_IO (write) recibido.");
                write_memory(&(package->payload), client->fd_client);
                break;
            
            case IO_FS_WRITE_MEMORY:
                log_info(MODULE_LOGGER, "IO: Nueva peticion STDOUT_IO (read) recibido.");
                read_memory(&(package->payload), client->fd_client);
                break;
            
            default:
                log_warning(MODULE_LOGGER, "%s: Header desconocido (%d)", "", package->header);
                break;
        }
        package_destroy(package);
    }
}

t_Process* seek_process_by_pid(t_PID pid) {

    t_Process* procesoBuscado;
    int size= list_size(LIST_PROCESSES);

    procesoBuscado = list_get(LIST_PROCESSES, 0); //SUPONEMOS QUE SIEMPRE ENCUENTRA EL PID

    for (size_t i = 0; i < size; i++)
    {
        procesoBuscado = list_get(LIST_PROCESSES,i);
        if (procesoBuscado->PID == pid) i=size;
        
    }
    
    return procesoBuscado;
}

void seek_instruccion(t_Payload *payload) {
    t_PID PID;
    t_PC PC;

    payload_shift(payload, &PID, sizeof(PID));
    payload_shift(payload, &PC, sizeof(PC));
    
    t_Process *procesoBuscado = seek_process_by_pid(PID);

    char* instruccionBuscada = NULL;
    if(PC < list_size(procesoBuscado->instructions_list)) {
        instruccionBuscada = list_get(procesoBuscado->instructions_list, PC);
    }

    usleep(RETARDO_RESPUESTA * 1000);
    if(send_text_with_header(INSTRUCTION_REQUEST, instruccionBuscada, CLIENT_CPU->fd_client)) {
        // TODO

        pthread_cancel(SERVER_MEMORY.thread_server);
        pthread_join(SERVER_MEMORY.thread_server, NULL);
        close(SERVER_MEMORY.fd_listen);

        pthread_cancel(CLIENT_KERNEL->thread_client_handler);
        pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
        close(CLIENT_KERNEL->fd_client);

        
        // ES ESTE HILO
        // pthread_cancel(CLIENT_CPU->thread_client_handler);
        // pthread_join(CLIENT_CPU->thread_client_handler, NULL);
        close(CLIENT_CPU->fd_client);
        exit(1);
    }
    log_info(MODULE_LOGGER, "Instruccion enviada.");
}

void create_frames(void) {
    t_MemorySize cantidad_marcos = TAM_MEMORIA / TAM_PAGINA;
    t_MemorySize offset = TAM_MEMORIA % TAM_PAGINA;
    if(offset != 0) cantidad_marcos++;
    
    LIST_FRAMES = list_create();
    LIST_FREE_FRAMES = list_create();

    for (t_MemorySize i = 0; i < cantidad_marcos; i++) {
        t_Frame *marcoNuevo = malloc(sizeof(t_Frame));
        marcoNuevo->id = i;
        marcoNuevo->assigned_page = NULL;
        marcoNuevo->PID = -1;
        list_add(LIST_FRAMES, marcoNuevo);
        list_add(LIST_FREE_FRAMES, marcoNuevo);
    }
}

void free_frames(){
    t_Frame *marco_liberar;

    for (int i = list_size(LIST_FRAMES); i > 0; i--)
    {
        marco_liberar = (t_Frame *) list_get(LIST_FRAMES, i - 1);
        free(marco_liberar);
    }

}

void respond_frame_request(t_Payload *payload) {
//Recibir parametros
    t_Page_Number page_number;
    t_PID pidProceso;

    payload_shift(payload, &page_number, sizeof(page_number));
    payload_shift(payload, &pidProceso, sizeof(pidProceso));

//Buscar frame
    t_Process* procesoBuscado = seek_process_by_pid(pidProceso);
    t_Frame_Number marcoEncontrado = seek_frame_number_by_page_number(procesoBuscado->pages_table, page_number);

    log_debug(MINIMAL_LOGGER, "AAA PID: <%" PRIu16 "> - Pagina: <%" PRIu32 "> - Marco: <%" PRIu32 ">", pidProceso, page_number, marcoEncontrado);

//Respuesta    
    usleep(RETARDO_RESPUESTA * 1000);
    
    t_Package* package = package_create_with_header(FRAME_REQUEST);
    payload_append(&(package->payload), &pidProceso, sizeof(pidProceso));
    payload_append(&(package->payload), &marcoEncontrado, sizeof(marcoEncontrado));
    package_send(package, CLIENT_CPU->fd_client);
}

t_Frame_Number seek_frame_number_by_page_number(t_list *tablaPaginas, t_Page_Number page_number) {
    t_Page *paginaBuscada;
    t_Frame_Number frame_number;
    uint32_t size = list_size(tablaPaginas);

    if(size >= page_number){
        log_error(MODULE_LOGGER, "El numero de página <%" PRIu32 "> no existe en la tabla de paginas.", page_number);
        exit(1);
    }

    for(size_t i = 0; i < size ; i++) {
        paginaBuscada = (t_Page *) list_get(tablaPaginas, i);
        if(paginaBuscada->pagid == page_number) {
            frame_number = paginaBuscada->assigned_frame;
            i = size + 1;
        }
    }

    return frame_number;
}

void read_memory(t_Payload *payload, int socket) {
    t_PID pid;
    t_list *list_physical_addresses = list_create();
    t_MemorySize bytes;

    payload_shift(payload, &pid, sizeof(pid));
    list_deserialize(payload, list_physical_addresses, physical_address_deserialize_element);
    payload_shift(payload, &bytes, sizeof(bytes));

    t_Physical_Address physical_address = *((t_Physical_Address *) list_get(list_physical_addresses, 0));
    
    void *posicion = (void *)(((uint8_t *) MAIN_MEMORY) + physical_address);

    log_debug(MINIMAL_LOGGER, "PID: <%" PRIu16 "> - Accion: <LEER> - Direccion fisica: <%" PRIu32 "> - Tamaño <%" PRIu32 ">", pid, physical_address, bytes);

    t_Frame_Number current_frame = physical_address / TAM_PAGINA;

    t_Package* package = package_create_with_header(READ_REQUEST);

    if(list_size(list_physical_addresses) == 1) { //En caso de que sea igual a una página
        pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
        payload_append(&(package->payload), posicion, bytes);
        pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
         //Actualizar pagina/TDP
        update_page(current_frame);
    }
    else { //En caso de que el contenido supere a 1 pagina
        t_MemorySize bytes_restantes = bytes;
        int bytes_inicial = TAM_PAGINA - (physical_address - (current_frame * TAM_PAGINA));
        
        for (t_MemorySize i = 1; i > list_size(list_physical_addresses); i++)
        {
            physical_address = *((t_Physical_Address *) list_get(list_physical_addresses, i - 1));
            current_frame = physical_address / TAM_PAGINA;
            //Posicion de la proxima escritura
            posicion = (void *)(((uint8_t *) MAIN_MEMORY) + physical_address);

            if (i == 1)//Primera pagina
            {
                pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
                payload_append(&(package->payload), posicion, bytes_inicial);
                pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
                update_page(current_frame);
                bytes_restantes -= bytes_inicial;
            }
            if ((i == list_size(list_physical_addresses)) && (i != 1))//Ultima pagina
            {
                pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
                payload_append(&(package->payload), posicion, bytes_restantes);
                pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
                update_page(current_frame);
            }
            if ((i < list_size(list_physical_addresses)) && (i != 1))//Paginas del medio
            {
                pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
                payload_append(&(package->payload), posicion, TAM_PAGINA);
                pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
                update_page(current_frame);
                bytes_restantes -= TAM_PAGINA;
            }
            
        }
    }

    package_send(package, socket);
    package_destroy(package);
}

void write_memory(t_Payload *payload, int socket) {
    t_PID pid;
    t_list *list_physical_addresses = list_create();
    t_MemorySize bytes;
    
    payload_shift(payload, &pid, sizeof(pid));
    list_deserialize(payload, list_physical_addresses, physical_address_deserialize_element);
    payload_shift(payload, &bytes, sizeof(bytes));

    t_Physical_Address physical_address = *((t_Physical_Address *) list_get(list_physical_addresses, 0));
    void *posicion = (void *)(((uint8_t *) MAIN_MEMORY) + physical_address);
    
    t_Frame_Number current_frame = physical_address / TAM_PAGINA;

    log_debug(MINIMAL_LOGGER, "PID: <%" PRIu16 "> - Accion: <ESCRIBIR> - Direccion fisica: <%" PRIu32 "> - Tamaño <%" PRIu32 ">", pid, physical_address, bytes);

//COMIENZA LA ESCRITURA
    if(list_size(list_physical_addresses) == 1) {//En caso de que sea igual a 1 página
        pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
        payload_shift(payload, posicion, (size_t) bytes);
        pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
         //Actualizar pagina/TDP
        update_page(current_frame);
    }
    else{//En caso de que el contenido supere a 1 pagina
        t_MemorySize bytes_restantes = bytes;
        int bytes_inicial = TAM_PAGINA - (physical_address - (current_frame * TAM_PAGINA));
        
        for (t_MemorySize i = 1; i > list_size(list_physical_addresses); i++)
        {
            physical_address = *((t_Physical_Address *) list_get(list_physical_addresses, i - 1));
            current_frame = physical_address / TAM_PAGINA;
            //Posicion de la proxima escritura
            posicion = (void *)(((uint8_t *) MAIN_MEMORY) + physical_address);

            if (i == 1)//Primera pagina
            {
                pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
                payload_shift(payload, posicion, (size_t) bytes_inicial);
                pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
                update_page(current_frame);
                bytes_restantes -= bytes_inicial;
            }
            if ((i == list_size(list_physical_addresses)) && (i != 1))//Ultima pagina
            {
                pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
                payload_shift(payload, posicion, (size_t) bytes_restantes);
                pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
                update_page(current_frame);
                bytes_restantes -= bytes_inicial;
            }
            if ((i < list_size(list_physical_addresses)) && (i != 1))//Paginas del medio
            {
                pthread_mutex_lock(&MUTEX_MAIN_MEMORY);
                payload_shift(payload, posicion, (size_t) TAM_PAGINA);
                pthread_mutex_unlock(&MUTEX_MAIN_MEMORY);
                update_page(current_frame);
                bytes_restantes -= TAM_PAGINA;
            }
            
        }
    }

    list_destroy_and_destroy_elements(list_physical_addresses, free);

    if(send_return_value_with_header(WRITE_REQUEST, 0, socket)) {
        // TODO
        exit(1);
    }
}

//Actualizar page y TDP
void update_page(t_Frame_Number current_frame){
    t_Frame* marco = list_get(LIST_FRAMES, (int) current_frame);
    t_Page* current_page = marco->assigned_page;
    current_page->last_use = time(NULL);
}


//En caso de varias paginas
int get_next_dir_fis(t_Frame_Number current_frame, t_PID pid){
    t_Frame* marco = (t_Frame *) list_get(LIST_FRAMES, current_frame);
    t_Page* current_page = marco->assigned_page;
    int pagid = current_page->pagid;
    t_Process* proceso = seek_process_by_pid(pid);
    current_page = list_get(proceso->pages_table, (pagid+1));
    int next_frame = current_page->assigned_frame;
    int offset = 0;
    int next_dir_fis = next_frame * TAM_PAGINA + offset;

    return next_dir_fis;
}


void resize_process(t_Payload *payload){
    t_PID pid;
    t_MemorySize new_size;

    payload_shift(payload, &pid, sizeof(pid));
    payload_shift(payload, &new_size, sizeof(new_size));

    t_Process* procesoBuscado = seek_process_by_pid(pid);

    t_MemorySize paginas = new_size / TAM_PAGINA;
    t_MemorySize resto = new_size % TAM_PAGINA;

    if (resto != 0)
        paginas += 1;

    int size = list_size(procesoBuscado->pages_table);
    t_Return_Value return_value;

    if (new_size > TAM_MEMORIA)
    {
        return_value = 1;
    }
    else{
        
        if(size < paginas) { //Agregar paginas

            //CASO: OUT OF MEMORY
            if (list_size(LIST_FREE_FRAMES) < (paginas - size))
                return_value = 1;

            else {
                
                log_debug(MINIMAL_LOGGER, "PID: <%" PRIu16 "> - Tamaño Actual: <%" PRIu32 "> - Tamaño a Ampliar: <%" PRIu32 ">", pid, size, paginas);

                //CASO: HAY ESPACIO Y SUMA PAGINAS
                for (size_t i = size; i < paginas; i++)
                {
                    t_Page *pagina = malloc(sizeof(t_Page));
                    pthread_mutex_lock(&(MUTEX_LIST_FREE_FRAMES));
                    t_Frame *marcoLibre = list_get(LIST_FREE_FRAMES,0);
                    list_remove(LIST_FREE_FRAMES,0);
                    pthread_mutex_unlock(&(MUTEX_LIST_FREE_FRAMES));
                    pagina->assigned_frame = marcoLibre->id;
                    pagina->bit_modificado = false;
                    pagina->bit_presencia = false;
                    pagina->bit_uso = false;
                    pagina->pagid = i;
                    pagina->last_use = 0;

                    //Actualizo el marco asignado
                    marcoLibre->PID= pid;
                    marcoLibre->assigned_page = pagina;

                    list_add(procesoBuscado->pages_table, pagina);
                }
                    
                return_value = 0;
            }
        }

        if(size > paginas) { // RESTA paginas
                
            log_debug(MINIMAL_LOGGER, "PID: <%" PRIu16 "> - Tamaño Actual: <%" PRIu32 "> - Tamaño a Reducir: <%" PRIu32 ">", pid, size, paginas);
            
            for (size_t i = size; i > paginas; i--)
            {
                int pos_lista = seek_oldest_page_updated(procesoBuscado->pages_table);
                t_Page* pagina = list_get(procesoBuscado->pages_table, pos_lista);
                list_remove(procesoBuscado->pages_table, pos_lista);
                pthread_mutex_lock(&(MUTEX_LIST_FREE_FRAMES));
                t_Frame* marco = list_get(LIST_FRAMES, pagina->assigned_frame);
                list_add(LIST_FREE_FRAMES,marco);
                pthread_mutex_unlock(&(MUTEX_LIST_FREE_FRAMES));

                free(pagina);
            }

            return_value = 0;
        }
    }
    //No hace falta el caso page == size ya que no sucederia nada

    if(send_return_value_with_header(RESIZE_REQUEST, return_value, CLIENT_CPU->fd_client)) {
        // TODO

        pthread_cancel(SERVER_MEMORY.thread_server);
        pthread_join(SERVER_MEMORY.thread_server, NULL);
        close(SERVER_MEMORY.fd_listen);

        pthread_cancel(CLIENT_KERNEL->thread_client_handler);
        pthread_join(CLIENT_KERNEL->thread_client_handler, NULL);
        close(CLIENT_KERNEL->fd_client);

        
        // ES ESTE HILO
        // pthread_cancel(CLIENT_CPU->thread_client_handler);
        // pthread_join(CLIENT_CPU->thread_client_handler, NULL);
        close(CLIENT_CPU->fd_client);

        log_debug(MINIMAL_LOGGER, "[OUT OF MEMORY]PID: <%" PRIu16 "> - Tamaño Actual: <%" PRIu32 "> - Tamaño a Reducir: <%" PRIu32 ">", pid, size, paginas);

        exit(EXIT_FAILURE);
    }
}

int seek_oldest_page_updated(t_list* page_list){

    int size = list_size(page_list);
    return (size - 1);
    
    /*
    t_Page* mas_antigua = list_get(page_list,0);
    
    int oldest_pos = 0;

    for (size_t i = 1; i < size; i++) {
        t_Page* page_temp = list_get(page_list,i);

        if (difftime(page_temp->last_use, mas_antigua->last_use) < 0) {
            mas_antigua = page_temp;
            oldest_pos = i;
        }
    }
    return oldest_pos;
    */
}

void free_memory(){
    free_all_process();
    free_frames();
    free(MAIN_MEMORY);
}

void free_all_process(){
    
    int size = list_size(LIST_PROCESSES);
    int size_TDP = 0;
    t_Process* processKilled;
    t_Page* pageKill;

    for (int i = (size -1); size > -1 ; size--)
    {
        processKilled = list_get(LIST_PROCESSES,i);
        size_TDP = list_size(processKilled->pages_table);
        for (int p = (size_TDP -1); size_TDP > -1 ; p--)
        {
            pageKill = list_get(processKilled->pages_table, p);
            free(pageKill);
        }
        
        free(processKilled);
    }
    
    log_debug(MODULE_LOGGER, "Se ha liberado todos los procesos y paginas.");
    
}