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

    int estado_actual;
    int fd_conexion;


}t_pcb;