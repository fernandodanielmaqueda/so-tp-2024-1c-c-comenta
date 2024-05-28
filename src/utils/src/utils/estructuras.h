/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include <stdint.h>

typedef enum process_status{
    //Estado PCB
    NEW,
    READY,
    EXECUTING,
    BLOCKED, 
	EXIT
} process_status;

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
    enum process_status current_status;
    //int fd_conexion;
    double arrival_READY;
    double arrival_RUNNING;

} t_pcb;


typedef enum t_opcode {
    DISCONNECTED = -1,
    //::OPERACIONES MODULOS::
    //Kernel - Memoria
    PROCESS_NEW,
    PROCESS_CREATED,
    //Kernel - IO
    //Kernel - CPU
    TYPE_INTERRUPT_SIN_INT,
    TYPE_INTERRUPT_FIN_PROCESO,
    TYPE_INTERRUPT_FIN_QUANTUM,
    ///////////////
    //CPU - Memoria////
    INSTUCTION_REQUEST,
    ///////////////////
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

