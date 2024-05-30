#include "console.h"

t_log *module_logger_consola;

void initialize_interactive_console(void) {

    module_logger_consola = log_create("consola.log", "Consola", true, LOG_LEVEL_TRACE);

    char* leido;
    leido = readline("> ");
    bool validate_lecture;

    while(strcmp(leido, "\0 ") != 0){
 
         validate_lecture = validate_command_console(leido);
         if(validate_lecture){
             log_error(module_logger_consola, "Comando de CONSOLA invalido");
             free(leido);
             leido = readline("> ");
    continue;
    }
 
 else{
    log_info(module_logger_consola, "Comando de CONSOLA valido");
    attend_command_console(leido);
    free(leido);
    leido = readline("> ");
     }
}

}

bool validate_command_console(char* leido){
    bool result_validation = false;

    char** command_console = string_split(leido, " ");
    t_funcion funcion = pedir_enum_funcion(command_console);
   

    switch (funcion)
    {
        
        case EJECUTAR_SCRIPT:
            if(strcmp(command_console[0], "EJECUTAR_SCRIPT") == 0 && string_is_empty(command_console[1]) == false){
                result_validation = true;
            }
            break;

        case INICIAR_PROCESO:
            if(strcmp(command_console[0], "INICIAR_PROCESO") == 0  && string_is_empty(command_console[1]) == false){
                result_validation = true;
            }
            break;

        case FINALIZAR_PROCESO:
            if(strcmp(command_console[0], "FINALIZAR_PROCESO") == 0  && string_is_empty(command_console[1]) == false){
                result_validation = true;
            }
            break;

        case DETENER_PLANIFICACION:
            if(strcmp(command_console[0], "DETENER_PLANIFICACION") == 0 ) {
                result_validation = true;
            }
            break;

        case INICIAR_PLANIFICACION:
            if(strcmp(command_console[0], "INICIAR_PLANIFICACION") == 0 ){
                result_validation = true;
            }
            break;
        case MULTIPROGRAMACION:
            if(strcmp(command_console[0], "MULTIPROGRAMACION") == 0  && string_is_empty(command_console[1]) == false){
                result_validation = true;
            }
            break;

        case PROCESO_ESTADO:
            if(strcmp(command_console[0], "PROCESO_ESTADO") == 0  ){
                result_validation = true;
            }
            break;

        default:
            result_validation = false;
            break;

    }

    return result_validation;
}


//RECIBO EL COMANDO DE LA CONSOLA Y EJECUTO SEGUN LO PEDIDO
void attend_command_console(char* leido){
    char** command_console = string_split(leido, " ");
    t_funcion funcion = pedir_enum_funcion(command_console);

    switch (funcion)
    {
        case EJECUTAR_SCRIPT:
        {
            char* path_script = strdup(command_console[1]);
            log_trace(module_logger_consola, "Se va a ejecutar el script %s", path_script);
            //TODO: ejecutar_script(path);==========================//
            break;
        }
        case INICIAR_PROCESO:
        {
            // strdup: duplica la cadena de caracteres y le asigna memoria sufuente
            char* path = strdup(command_console[1]);
            log_trace(module_logger_consola, "Se va a iniciar el proceso %s", path);

            //TODO: iniciar_proceso(path);==========================//
            break;
        }

        //usar
        case FINALIZAR_PROCESO:
        {
         int pid = atoi(command_console[1]);
         //NECESITO EL PCB PARA SEGUIR ACA =-=======================//
         log_trace(module_logger_consola, "Se fianlizo el proceso %d", pid);
          break;
        }
        //usar
        case DETENER_PLANIFICACION:
        {
          log_trace(module_logger_consola, "Se  detuvo la planificacion");
          stop_planificacion();
          break;
        }
        //usar
        case INICIAR_PLANIFICACION:
        {
         log_trace(module_logger_consola, "Se  inicio la planificacion");
         init_planificacion();
         break;
        }
         case MULTIPROGRAMACION:
         {
          int nuevo_grado = atoi(command_console[1]);
        log_trace(module_logger_consola, " Grado Actual: %d",  nuevo_grado);   
        //TODO:crear_nuevo_semaforo_multiprog==========================// 
         break;
        }
        //usar
        //necesito el PCB
        case PROCESO_ESTADO:
        // listState();
        break;

        default:
            log_error(module_logger_consola, "Comando de CONSOLA invalido");
            break;
    }

    
    free_strv(command_console);
}

int pedir_enum_funcion(char** sublinea)
{
	if(strcmp(sublinea[0], "INICIAR_PROCESO") == 0)
		return INICIAR_PROCESO;
       else if(strcmp(sublinea[0], "EJECUTAR_SCRIPT") == 0)
        return EJECUTAR_SCRIPT;
	else if(strcmp(sublinea[0], "FINALIZAR_PROCESO") == 0)
		return FINALIZAR_PROCESO;
	else if(strcmp(sublinea[0], "DETENER_PLANIFICACION") == 0)		
		return DETENER_PLANIFICACION;
	else if(strcmp(sublinea[0], "INICIAR_PLANIFICACION") == 0)
		return INICIAR_PLANIFICACION;
	else if(strcmp(sublinea[0], "MULTIPROGRAMACION") == 0)
		return MULTIPROGRAMACION;
	else if(strcmp(sublinea[0], "PROCESO_ESTADO") == 0)
		return PROCESO_ESTADO;
	else
		return -1;
}