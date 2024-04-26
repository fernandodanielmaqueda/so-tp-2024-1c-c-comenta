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
	EXITED
} t_opcode;