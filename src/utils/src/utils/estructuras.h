/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include <stdint.h>

typedef struct t_pcb {
    uint32_t pid; 
    uint32_t pc; 
    uint32_t AX;
    uint32_t BX;
    uint32_t CX; 
    uint32_t DX;
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
    int estado_actual;
    int fd_conexion;
    double llegada_ready;
    double llegada_running;

} t_pcb;

//IDEA DE BRAI===============
typedef struct t_instruccion {
    char* id;
    char* param1;
    char* param2;
    char* param3;
    char* param4;
    char* param5;

} t_instruccion;
////////////////////////////

typedef enum t_opcode {
    DESCONEXION = -1,
    //Estado PCB
    NEW,
    READY,
    EXECUTING,
    BLOCKED,
	EXITED,
    //::OPERACIONES MODULOS::
    //Kernel - Memoria
    PROCESS_NEW,
    //Kernel - IO
    //Kernel - CPU
    //CPU - Memoria
    //IO - Memoria
    //Instrucciones
    SET,
    MOVE_IN,
    MOVE_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
} t_opcode;


//Estructuras que necesita la consola para entender las palabras
typedef enum {
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO,
    EJECUTAR_SCRIPT
} t_funcion;


//////Para el ciclo de instruccion sugerencia ========================
typedef enum {
    SIN_INT,     //sin interrupcion
    FIN_PROCESO, //por exit
    FIN_QUANTUM, //por syscall 

} t_int;

typedef struct{
    int id;
    unsigned int pc; 
    uint32_t registros[4];
    t_int interrupcion;  
} t_contexto;
////////////////////////////////////////////////////////

///SUGUERENCIA ESTRUCTURA OPTIMA PARA  MANEJO DE RECURSO PARA EL CICLO EXECUTE////

/* 
typedef enum {
    SET,
    MOV_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIT,
} t_instruction_type_use;
*/

typedef struct {
	t_opcode operation;
	t_list* parameters;
} t_instruction_use;

typedef enum {
	AX,
	BX,
	CX,
	DX,
} t_register;





/////////////////=================//////////////////////