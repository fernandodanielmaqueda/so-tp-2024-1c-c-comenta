
/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "memoria.h"

char *MODULE_NAME = "memoria";
char *MODULE_LOG_PATHNAME = "memoria.log";
char *MODULE_CONFIG_PATHNAME = "memoria.config";

t_log *MODULE_LOGGER;
extern t_log *SOCKET_LOGGER;
t_config *MODULE_CONFIG;

void *memoria_principal;
pthread_t hilo_kernel;
pthread_t hilo_cpu;
pthread_t hilo_io;
t_list* lista_procesos;
t_list* lista_marcos;
t_list* lista_marcos_libres;

int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

int module(int argc, char* argv[]) {

	initialize_loggers();
	initialize_configs(MODULE_CONFIG_PATHNAME);

    memoria_principal = (void*) malloc(TAM_MEMORIA);
    memset(memoria_principal, (u_int32_t) '0', TAM_MEMORIA); //Llena de 0's el espacio de memoria
    lista_procesos = list_create();
    create_marcos();

    initialize_sockets();

    log_debug(MODULE_LOGGER, "Modulo %s inicializado correctamente\n", MODULE_NAME);

    listen_kernel(FD_CLIENT_KERNEL);
 
 /*   
    pthread_create(&hilo_cpu, NULL, (void *)listen_cpu, (void *)FD_CLIENT_CPU);
    pthread_create(&hilo_kernel, NULL, (void *)listen_kernel, (void *)FD_CLIENT_KERNEL);
    pthread_create(&hilo_io, NULL, (void *)listen_io, (void *)fd_io);

    pthread_join(hilo_cpu, NULL);
    pthread_join(hilo_kernel, NULL);
    pthread_join(hilo_io, NULL);

    */

	//finish_threads();
	finish_sockets();
	//finish_configs();
	finish_loggers();

   return EXIT_SUCCESS;
}

void read_module_config(t_config* MODULE_CONFIG) {
    COORDINATOR_MEMORY = (t_Server) {.server_type = MEMORY_PORT_TYPE, .clients_type = TO_BE_DEFINED_PORT_TYPE, .port = config_get_string_value(MODULE_CONFIG, "PUERTO_ESCUCHA")};
    TAM_MEMORIA = config_get_int_value(MODULE_CONFIG, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(MODULE_CONFIG, "TAM_PAGINA");
    PATH_INSTRUCCIONES = config_get_string_value(MODULE_CONFIG, "PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(MODULE_CONFIG, "RETARDO_RESPUESTA");
}

void listen_kernel(int fd_kernel) {
    while(1) {
        t_Package* paquete = package_receive(fd_kernel);
        e_Header header = paquete->header; //enum HeaderCode headerCode = package_receive_header(fd_kernel);
        switch(header) {
            case PROCESS_NEW:
                log_info(MODULE_LOGGER, "KERNEL: Proceso nuevo recibido.");
                create_process(paquete->payload);
                break;
                
            case PROCESS_FINALIZED:
                log_info(MODULE_LOGGER, "KERNEL: Proceso finalizado recibido.");
                kill_process(paquete->payload);
                break;

            case DISCONNECTION_HEADER:
                log_warning(MODULE_LOGGER, "Se desconecto kernel.");
                log_destroy(MODULE_LOGGER);
                return;
            
            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
        }
        package_destroy(paquete);
    }
}

void create_process(t_Payload* socketRecibido) {

    t_Process *new_process = malloc(sizeof(t_Process));
    t_list* instructions_list = list_create();
    t_list* pages_table = list_create();

    //Leo los valores recibidos por parametro
    //t_list *lista_elememtos = NULL; //t_list *lista_elememtos = get_package_like_list(socketRecibido);
    int cursor = 0;
    //new_process->name = string_duplicate(list_get(lista_elememtos, ++cursor));
    //new_process->PID = *(int *)list_get(lista_elememtos, ++cursor);
    int string_len = -1;   
    cursor = memcpy_deserialize(&(string_len), socketRecibido->stream, cursor, sizeof(int));
    cursor = memcpy_deserialize(&(new_process->name), socketRecibido->stream, cursor, string_len);
    cursor = memcpy_deserialize(&(new_process->PID), socketRecibido->stream, cursor, sizeof(int));

    //Busco el archivo deseado
    char* path_buscado = string_duplicate(PATH_INSTRUCCIONES);
    string_append(&path_buscado, "/");
    string_append(&path_buscado, new_process->name);
    log_debug(MODULE_LOGGER, "Archivo Buscado: %s", path_buscado);

    //CREAR LISTA INST CON EL PARSER
    parser_file(path_buscado, instructions_list);

    new_process->number_of_instructions = list_size(instructions_list);
    new_process->instructions_list = instructions_list;
    new_process->pages_table = pages_table;
    list_add(lista_procesos, new_process);
    
    log_debug(MODULE_LOGGER, "Archivo leido: %s", path_buscado);

    //ENVIAR RTA OK A KERNEL --> En este caso solo envio el PID del proceso origen
    send_int(new_process->PID,FD_CLIENT_KERNEL,PROCESS_CREATED);
    
}

void kill_process (t_Payload* socketRecibido){
    int pid = 0; //int pid = atoi(message_receive(socketRecibido));
    memcpy(&pid, socketRecibido->stream, sizeof(int));
    t_Process* process = seek_process_by_pid(pid);
    t_Page* paginaBuscada;
    
    int size = list_size(process->pages_table);
    for (size_t i = size; i == 0 ; i--)
    {
        paginaBuscada = list_get(process->pages_table, i);
        t_Frame *marco = list_get(lista_marcos, paginaBuscada->assigned_frame);
        list_add(lista_marcos_libres, marco);
        free(paginaBuscada);
    }
    free(process);
    
    //ENVIAR RTA OK A KERNEL --> En este caso solo envio el PID del proceso origen
    send_int(pid,FD_CLIENT_KERNEL,PROCESS_CREATED);
    
}

void parser_file(char* path, t_list *list_instruction) {

    FILE* file;
    if ((file = fopen(path, "r")) == NULL) {
        log_error(MODULE_LOGGER, "[ERROR] No se pudo abrir el archivo de pseudocodigo indicado.");
        exit(EXIT_FAILURE);
    }

    char *line = NULL, *subline;
    size_t length;
    ssize_t nread;

    while(1) {

        nread = getline(&line, &length, file);

        if(nread == -1) {
            if(errno) {
                log_warning(MODULE_LOGGER, "Funcion getline: %s", strerror(errno));
                free(line);
                exit(EXIT_FAILURE);
            }

            // Se terminó de leer el archivo
            free(line);
            break;
        }

        subline = strip_whitespaces(line);

        if(*subline) {
           list_add(list_instruction, arguments_create(subline, MODULE_LOGGER));
        }
    }
       
        fclose(file);
}

void listen_cpu(int fd_cpu) {
    while(1) {
        t_Package* paquete = package_receive(fd_cpu);
        e_Header header = paquete->header;
        //e_CPU_Memory_Request memory_request = 0; //enum HeaderCode headerCode = package_receive_header(fd_cpu);
        switch (header) {
            case INSTRUCTION_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de instruccion recibido.");
                seek_instruccion(paquete->payload);
                break;
                
            case FRAME_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de frame recibido.");
                respond_frame_request(paquete->payload);
                break;

            /*
                log_warning(MODULE_LOGGER, "Se desconecto CPU.");
                log_destroy(MODULE_LOGGER);
                return;
            */
                
            case PAGE_SIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                send_int(PAGE_SIZE_REQUEST, TAM_PAGINA,FD_CLIENT_CPU);
                break;

            case RESIZE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de tamaño de pagina recibido.");
                resize_process(paquete->payload);
                break;
                
            case READ_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de lectura recibido.");
                //read_memory(paquete->payload, FD_CLIENT_CPU);
                break;
                
            case WRITE_REQUEST:
                log_info(MODULE_LOGGER, "CPU: Pedido de lectura recibido.");
                //write_memory(paquete->payload, FD_CLIENT_CPU);
                break;
            
            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
            }
    }
}

void listen_io(int fd_io) {
    while(1) {
        t_Package* paquete = package_receive(fd_io);
        e_Header header = paquete->header; //enum HeaderCode headerCode = package_receive_header(fd_kernel);
        switch(header) {
            /*
            case PROCESS_NEW:
                log_info(MODULE_LOGGER, "IO: Nueva peticion recibido.");
                create_process(paquete->payload);
                break;
                */

            case DISCONNECTION_HEADER:
                log_warning(MODULE_LOGGER, "Se desconecto kernel.");
                log_destroy(MODULE_LOGGER);
                return;
            
            default:
                log_warning(MODULE_LOGGER, "Operacion desconocida..");
                break;
        }
        package_destroy(paquete);
    }

}

t_Process* seek_process_by_pid(int pidBuscado) {

    t_Process* procesoBuscado;
    int size= list_size(lista_procesos);

    procesoBuscado = list_get(lista_procesos,0); //SUPONEMOS QUE SIEMPRE ENCUENTRA EL PID

    for (size_t i = 0; i < size; i++)
    {
        procesoBuscado = list_get(lista_procesos,i);
        if (procesoBuscado->PID == pidBuscado) i=size;
        
    }
    
    return procesoBuscado;
}

void seek_instruccion(t_Payload* socketRecibido) {
    //t_list *lista_elememtos = NULL; //t_list *lista_elememtos = get_package_like_list(socketRecibido);
    //int cursor = 0;
    //int pid = *(int *)list_get(lista_elememtos, ++cursor);
    int pid = -1;
    int pc = -1;
    
    receive_2int(&pid,&pc,socketRecibido);
    //memcpy(&pid, socketRecibido->stream, sizeof(int));
    //memcpy(&pc,( socketRecibido->stream + sizeof(int)), sizeof(int));
    //int pc = *(int *)list_get(lista_elememtos, ++cursor);
    //list_destroy_and_destroy_elements(lista_elememtos, &free);

    
    t_Process* procesoBuscado = seek_process_by_pid(pid);
    //Suponemos que la instruccion es encontrada siempre
    t_CPU_Instruction* instruccionBuscada = list_get(procesoBuscado->instructions_list, pc);

    usleep(RETARDO_RESPUESTA * 1000);
    cpu_instruction_send(instruccionBuscada, FD_CLIENT_CPU);
    log_info(MODULE_LOGGER, "Instruccion enviada.");
}

void create_marcos(){
    int cantidad_marcos = TAM_MEMORIA / TAM_PAGINA;
    
    lista_marcos = list_create();
    lista_marcos_libres = list_create();

    for (size_t i = 0; i < cantidad_marcos; i++)
    {
        t_Frame *marcoNuevo = malloc(sizeof(t_Frame));
        marcoNuevo->id = i;
        marcoNuevo->assigned_page = NULL;
        marcoNuevo->PID = -1;
        list_add(lista_marcos, marcoNuevo);
        list_add(lista_marcos_libres, marcoNuevo);
    }
    
}

void free_marcos(){
    int cantidad_marcos = list_size(lista_marcos);
    t_Frame *marco_liberar;

    for (size_t i = cantidad_marcos; i == 0; i--)
    {
        marco_liberar = list_get(lista_marcos, i);
        free(marco_liberar);
    }

}

void respond_frame_request(t_Payload* socketRecibido){
//Recibir parametros
 /* t_list *propiedadesPlanas = NULL; // t_list *propiedadesPlanas = get_package_like_list(socketRecibido);
  int cursor = 0;
  int pageBuscada = *(int*)list_get(propiedadesPlanas, cursor);
  int pidProceso = *(int*)list_get(propiedadesPlanas, cursor);
  list_destroy_and_destroy_elements(propiedadesPlanas, &free);
*/
    int pageBuscada = -1;
    int pidProceso = -1;
    receive_2int(&pageBuscada,&pidProceso,socketRecibido);
//Buscar frame
    t_Process* procesoBuscado = seek_process_by_pid(pidProceso);
    int marcoEncontrado = seek_marco_with_page_on_TDP(procesoBuscado->pages_table, pageBuscada);

//Respuesta    
    usleep(RETARDO_RESPUESTA * 1000);
    send_2int(pidProceso,marcoEncontrado,FD_CLIENT_CPU,FRAME_REQUEST);
   /* t_Package* package = package_create_with_header(FRAME_REQUEST);
    payload_enqueue(package->payload, &pidProceso, sizeof(int));
    payload_enqueue(package->payload, &marcoEncontrado, sizeof(int));
    package_send(package, FD_CLIENT_CPU);
    */
}

int seek_marco_with_page_on_TDP(t_list* tablaPaginas, int pagina) {
    t_Page* paginaBuscada;
    int marcoObjetivo = -1;
    int size= list_size(tablaPaginas);
    for(size_t i = 0; i < size ; i++) {
        paginaBuscada = list_get(tablaPaginas, i);
        if(paginaBuscada->pagid == pagina) {
            marcoObjetivo = paginaBuscada->assigned_frame;
            i = size + 1;
        }
    }

    return marcoObjetivo;
}

void read_memory(t_Payload* socketRecibido, int socket) {
    int dir_fisica = 0;
    int pidBuscado = 0;
    int bytes = 0;
    //receive_2int(&dir_fisica,&pidBuscado,socketRecibido);
    receive_read_request(&pidBuscado, &dir_fisica, &bytes,socketRecibido);

    char* lectura;
    void* posicion = memoria_principal + dir_fisica;
    memcpy(&lectura, posicion, bytes);

    send_String_1int(pidBuscado,lectura, socket, READ_REQUEST);
    
}


void write_memory(t_Payload* socketRecibido, int socket){
    int dir_fisica = 0;
    int pidBuscado = 0;
    int bytes = 0;
    char* contenido;
    int temp_dir_fis = 0;

    
    receive_write_request(&pidBuscado, &dir_fisica, &bytes, &contenido, socketRecibido);
    
    //receive_2int_1uint32(&dir_fisica,&pidBuscado,&contenido, socketRecibido);

    int pages = sizeof(contenido)/TAM_PAGINA;
    int resto = sizeof(contenido) % TAM_PAGINA;
    if (resto != 0) pages += 1;
    void* posicion = memoria_principal + dir_fisica;
    
    int current_frame = dir_fisica / TAM_PAGINA;

    if(pages < 2){//En caso de que sea menor a 2 pagina
         memcpy(posicion, &contenido, bytes);
         //Actualizar pagina/TDP
         update_page(current_frame);
    }
    else{//En caso de que el contenido supere a 1 pagina
        int bytes_restantes = bytes;
        for (size_t i = 1; i > pages; i++)
        {
            if (i == pages)
            {
                memcpy(posicion, &contenido, bytes_restantes);
            }
            if(i<pages){
                memcpy(posicion, &contenido, TAM_PAGINA);
                update_page(current_frame);
                bytes_restantes -= TAM_PAGINA;

                temp_dir_fis = get_next_dir_fis(current_frame,pidBuscado);
                current_frame = temp_dir_fis / TAM_PAGINA;
                //Posicion de la proxima escritura
                posicion = memoria_principal + temp_dir_fis;
            }
            
        }
    }

    send_int(pidBuscado,socket,WRITE_REQUEST);
}

//Actualizar page y TDP
void update_page(int current_frame){
    t_Frame* marco = list_get(lista_marcos, current_frame);
    t_Page* current_page = marco->assigned_page;
    current_page->last_use = time(NULL);
}


//En caso de varias paginas
int get_next_dir_fis(int current_frame, int pid){
    t_Frame* marco = list_get(lista_marcos, current_frame);
    t_Page* current_page = marco->assigned_page;
    int pagid = current_page->pagid;
    t_Process* proceso = seek_process_by_pid(pid);
    current_page = list_get(proceso->pages_table, (pagid+1));
    int next_frame = current_page->assigned_frame;
    int offset = 0;
    int next_dir_fis = next_frame * TAM_PAGINA + offset;

    /*
    int count = list_size(proceso->pages_table);
    for (size_t i = 0; i < count; i++)
    {
        current_page = list_get (proceso->pages_table,i);
        if (current_page->pagid == pagid)
        {
            
        }
        
    }
     
    int nro_page = floor(dir_logica / tamanio_pagina);
    int offset = dir_logica - nro_page * tamanio_pagina; 
    dir_fisica = nro_frame_required * tamanio_pagina + offset;
*/
    return next_dir_fis;
}


void resize_process(t_Payload* socketRecibido){
    int pid;
    int bytes;
    receive_2int(&pid,&bytes,socketRecibido);
    t_Process* procesoBuscado = seek_process_by_pid(pid);

    int paginas = bytes / TAM_PAGINA;
    int resto = bytes % TAM_PAGINA;
    if (resto == 0) paginas += 1;
    

    int size = list_size(procesoBuscado->pages_table);
    if(size<paginas){//Agregar paginas

        //CASO: OUT OF MEMORY
        if (list_size(lista_marcos_libres) < (paginas - size))
        {
            send_int(pid, FD_CLIENT_CPU, OUT_OF_MEMORY);
        }
        else{
                //CASO: HAY ESPACIO Y SUMA PAGINAS
                for (size_t i = size; i < paginas; i++)
                {
                    t_Page* pagina = malloc(sizeof(t_Page));
                    t_Frame* marcoLibre = list_get(lista_marcos_libres,0);
                    list_remove(lista_marcos_libres,0);
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
                
            send_int(pid, FD_CLIENT_CPU, RESIZE_REQUEST);
        }
        
    }
    if(size>paginas){ //RESTA paginas
         
        for (size_t i = size; i > paginas; i--)
        {
            int pos_lista = seek_oldest_page_updated(procesoBuscado->pages_table);
            t_Page* pagina = list_get(procesoBuscado->pages_table, pos_lista);
            list_remove(procesoBuscado->pages_table, pos_lista);
            t_Frame* marco = list_get(lista_marcos, pagina->assigned_frame);
            list_add(lista_marcos_libres,marco);

            free(pagina);
        }
        
            send_int(pid, FD_CLIENT_CPU, RESIZE_REQUEST);
        
    }
    //No hace falta el caso page == size ya que no sucederia nada
}

int seek_oldest_page_updated(t_list* page_list){

    t_Page* mas_antigua = list_get(page_list,0);
    int size = list_size(page_list);
    int oldest_pos = 0;

    for (size_t i = 1; i < size; i++) {
        t_Page* page_temp = list_get(page_list,i);

        if (difftime(page_temp->last_use, mas_antigua->last_use) < 0) {
            mas_antigua = page_temp;
            oldest_pos = i;
        }
    }
    return oldest_pos;

}