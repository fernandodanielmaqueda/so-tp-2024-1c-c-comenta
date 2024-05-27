/* 
 ===========================IDEA DE COMO ENCARAR PARA LEER LAS ESTRUCTURAS Y MADNAR AL CPU=====================================



#include "parser.h"
//Lo que voy a ahacer aca es que memoria lea un archivo de pseudocodigo paragenerar todo listo asi se lo envia a cpu

t_dictionary* dictionary_file = NULL;

const char* instruction_strings [] = {
    "SET",
    "MOV_IN",
    "MOV_OUT",
    "SUM",
    "SUB",
    "JNZ",
    "RESIZE",
    "COPY_STRING",
    "WAIT",
    "SIGNAL",
    "IO_GEN_SLEEP",
    "IO_STDIN_READ",
    "IO_STDOUT_WRITE",
    "IO_FS_CREATE",
    "IO_FS_DELETE",
    "IO_FS_TRUNCATE",
    "IO_FS_WRITE",
    "IO_FS_READ",
    "EXIT",
    
};


void parser_file(char* path, t_list* list_instruction ){

    FILE* file;
    if ((file = fopen(path, "r")) == NULL)
    {
        log_error(module_logger, "[ERROR] No se pudo abrir el archivo de pseudocodigo indicado.");
        exit(EXIT_FAILURE);
    }
        //if(dictionary_file == NULL)dictionary_file = dictionary_create();
        
        while(!feof(file)) create_instruction(file, list_instruction);
       
        fclose(file);
    
}


void create_instruction(FILE* file, t_list* list_instruction){

    t_instruction_use* nueva_instruccion = malloc(sizeof(t_instruction_use));
    char *linea = string_new();
    int tamanio_buffer = 0;
 
    getline(&linea, (size_t *restrict)&tamanio_buffer, file);

    if (linea[strlen(linea) - 1] == '\n') linea[strlen(linea) - 1] = '\0';
  
    char** campos = string_split(linea," ");

    nueva_instruccion->operation = (t_opcode)(campos[0]);
    nueva_instruccion->parameters = list_create();

    int numero_elementos = count_elements(campos);
    for (int pos = 1; pos < numero_elementos; pos++) 
	{
        list_add(nueva_instruccion->parameters, campos[pos]); 
	}

	list_add(list_instruction, nueva_instruccion);

    free(linea);

}

*/