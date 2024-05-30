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

typedef struct t_pcb {
    uint32_t pid;
    uint32_t pc;
    //registers_1[t_register];
    uint8_t AX;
    uint8_t BX;
    uint8_t CX; 
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t RAX;
    uint32_t RBX;
    uint32_t RCX;
    uint32_t RDX;
    uint32_t SI;
    uint32_t DI;

    uint32_t quantum;
    enum Process_State current_state;
    //int fd_conexion;
    double arrival_READY;
    double arrival_RUNNING;
} t_pcb;

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
} t_register;

#endif // ESTRUCTURAS_H