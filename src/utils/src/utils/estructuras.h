#include <stdint.h>

typedef struct {
    uint32_t pid; 
    uint32_t pc; 
    char* instrucciones; 
    char* AX;
    char* BX;
    char* CX; 
    char* DX;
    char* EAX;
    char* EBX;
    char* ECX;
    char* EDX;
    char* RAX;
    char* RBX;
    char* RCX;
    char* RDX;

    double llegada_ready;
    double llegada_running;
    int estado_actual;
    int fd_conexion;

}t_pcb;

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