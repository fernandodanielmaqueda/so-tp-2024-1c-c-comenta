#include <stdint.h>

typedef struct {
    uint32_t pid; 
    uint32_t pc; 
    char* instrucciones; 
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

}t_pcb;

typedef struct {
    char* id;
    char* param1;
    char* param2;
    char* param3;
    char* param4;
    char* param5;

}t_instruccion;
typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCKED,
	EXITED,
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
