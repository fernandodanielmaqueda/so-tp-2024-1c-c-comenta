/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include <stdio.h>
#include <string.h>
#include <utils/serialize/pcb.h>

typedef enum e_CPU_Register {
    PC_REGISTER,
    
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
} e_CPU_Register;

typedef enum e_CPU_Register_DataType {
    UINT8_DATATYPE,
    UINT32_DATATYPE
} e_CPU_Register_DataType;

typedef struct t_CPU_Register_Accessor {
    e_CPU_Register_DataType register_dataType;
    void *register_pointer;
} t_CPU_Register_Accessor;

typedef struct t_CPU_Register_Info {
    char *name;
    e_CPU_Register_DataType dataType;
} t_CPU_Register_Info;

extern const t_CPU_Register_Info CPU_REGISTERS[];

int decode_register(char *name, e_CPU_Register *destination);
t_CPU_Register_Accessor get_register_accessor(t_PCB *pcb, e_CPU_Register cpu_register);
void set_register_value(t_CPU_Register_Accessor register_accessor, uint32_t value);
void get_register_value(t_PCB *pcb, e_CPU_Register cpu_register, uint32_t* value);
int get_register_size(e_CPU_Register registro);