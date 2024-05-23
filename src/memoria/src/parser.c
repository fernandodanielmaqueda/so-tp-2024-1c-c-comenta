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

void parser_file(char* path, t_list list_instruction ){

    FILE* file = fopen(path, "r");
    if(file == NULL){
        log_error(module_logger, "Error al abrir el archivo de pseudocodigo");
        exit(EXIT_FAILURE);
    }
    else{
        if(dictionary_file == NULL)dictionary_file = dictionary_create();
        
        while(!feof(file)) create_instruction(file, list_instruction);
       
        fclose(file);
    }
}


void create_instruction(FILE* file, t_list list_instruction){

 t_instruccion* 

}


int main(int argc, char* argv[]) {
    module(argc, argv);
    return 0;
}

*/