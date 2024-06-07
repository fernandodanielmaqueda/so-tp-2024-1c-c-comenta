/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdint.h>
typedef enum Process_State {
    NEW,
    READY,
    EXECUTING,
    BLOCKED, 
	EXIT
} Process_State;

//Estructuras que necesita la consola para entender las palabras
typedef enum t_funcion {
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO,
    EJECUTAR_SCRIPT
} t_funcion;

typedef enum t_register {
	AX,
	BX,
	CX,
	DX,
    EAX,
    EBX,
    ECX,
    EDX,
    RAX,
    RBX,
    RCX,
    SI,
    DI

} t_register;

typedef struct t_tlb {
	int PID;
	int nro_page;
	int frame;
    int time; //para el LRU
	
} t_tlb;



#endif // ESTRUCTURAS_H