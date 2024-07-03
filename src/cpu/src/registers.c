/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "registers.h"

const t_CPU_Register_Info CPU_REGISTERS[] = {
    [PC_REGISTER] = {.name = "PC" , .dataType = UINT32_DATATYPE},

    [AX_REGISTER] = {.name = "AX" , .dataType = UINT8_DATATYPE},
    [BX_REGISTER] = {.name = "BX" , .dataType = UINT8_DATATYPE},
    [CX_REGISTER] = {.name = "CX" , .dataType = UINT8_DATATYPE},
    [DX_REGISTER] = {.name = "DX" , .dataType = UINT8_DATATYPE},
    [EAX_REGISTER] = {.name = "EAX" , .dataType = UINT32_DATATYPE},
    [EBX_REGISTER] = {.name = "EBX" , .dataType = UINT32_DATATYPE},
    [ECX_REGISTER] = {.name = "ECX" , .dataType = UINT32_DATATYPE},
    [EDX_REGISTER] = {.name = "EDX" , .dataType = UINT32_DATATYPE},
    [RAX_REGISTER] = {.name = "RAX" , .dataType = UINT32_DATATYPE},
    [RBX_REGISTER] = {.name = "RBX" , .dataType = UINT32_DATATYPE},
    [RCX_REGISTER] = {.name = "RCX" , .dataType = UINT32_DATATYPE},
    [RDX_REGISTER] = {.name = "RDX" , .dataType = UINT32_DATATYPE},
    [SI_REGISTER] = {.name = "SI" , .dataType = UINT32_DATATYPE},
    [DI_REGISTER] = {.name = "DI" , .dataType = UINT32_DATATYPE},
};

int decode_register(char *name, e_CPU_Register *destination) {
    if(name == NULL || destination == NULL)
        return 1;
    
    size_t cpu_registers_number = sizeof(CPU_REGISTERS) / sizeof(CPU_REGISTERS[0]);
    for (register e_CPU_Register cpu_register = 0; cpu_register < cpu_registers_number; cpu_register++)
        if (!strcmp(CPU_REGISTERS[cpu_register].name, name)) {
            *destination = cpu_register;
            return 0;
        }

    return 1;
}

t_CPU_Register_Accessor get_register_accessor(t_PCB *pcb, e_CPU_Register cpu_register) {
    t_CPU_Register_Accessor register_accessor;
    register_accessor.register_dataType = CPU_REGISTERS[cpu_register].dataType;
    register_accessor.register_pointer = NULL;

    switch (cpu_register) {
        case PC_REGISTER:
            register_accessor.register_pointer = &(pcb->PC);
            break;
            
        case AX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.AX);
            break;
        case BX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.BX);
            break;
        case CX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.CX);
            break;
        case DX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.DX);
            break;
        case EAX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.EAX);
            break;
        case EBX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.EBX);
            break;
        case ECX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.ECX);
            break;
        case EDX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.EDX);
            break;
        case RAX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.RAX);
            break;
        case RBX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.RBX);
            break;
        case RCX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.RCX);
            break;
        case RDX_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.RDX);
            break;
        case SI_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.SI);
            break;
        case DI_REGISTER:
            register_accessor.register_pointer = &(pcb->cpu_registers.DI);
            break;
    }

    return register_accessor;
}

void set_register_value(t_CPU_Register_Accessor register_accessor, uint32_t value) {
    switch (register_accessor.register_dataType) {
        case UINT8_DATATYPE:
            *(uint8_t *) register_accessor.register_pointer = (uint8_t) value;
            break;
        case UINT32_DATATYPE:
            *(uint32_t *) register_accessor.register_pointer = value;
            break;
    }
}


void get_register_value(t_PCB *pcb, e_CPU_Register cpu_register, uint32_t* value)
{    
    t_CPU_Register_Accessor register_accessor = get_register_accessor(pcb, cpu_register);

    switch (register_accessor.register_dataType) {
        case UINT8_DATATYPE:
            *value = *(uint8_t *) register_accessor.register_pointer;
            break;
        case UINT32_DATATYPE:
            *value = *(uint32_t *) register_accessor.register_pointer;
            break;
        default://Caso tipo de dato diferente
            *value = 0;
            break;
    }
}


int get_register_size(e_CPU_Register registro){
    int bytes = 0;
    switch (CPU_REGISTERS[registro].dataType) {
        case UINT8_DATATYPE:
            bytes = sizeof(uint8_t);
            break;
        case UINT32_DATATYPE:
            bytes = sizeof(uint32_t);
            break;
    }

    return bytes;
}