/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdint.h>
typedef enum e_Process_State {
    NEW,
    READY,
    EXECUTING,
    BLOCKED, 
	EXIT
} e_Process_State;

//Estructuras que necesita la consola para entender las palabras
typedef enum e_Function {
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO,
    EJECUTAR_SCRIPT
} e_Function;

typedef enum e_Register {
	AX_REGISTER,
	BX_REGISTER,
	CX_REGISTER,
	DX_REGISTER,
    EAX_REGISTER,
    EBX_REGISTER,
    ECX_REGISTER,
    EDX_REGISTER,
    RAX_REGISTER,
    RBX_REGISTER,
    RCX_REGISTER,
    RDX_REGISTER,
    SI_REGISTER,
    DI_REGISTER
} e_Register;

typedef struct t_TLB {
	int PID;
	int page_number;
	int frame;
    int time; //para el LRU
} t_TLB;



#endif // ESTRUCTURAS_H