/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include "utils/serialize/cpu_instruction.h"

void cpu_instruction_send(t_CPU_Instruction *instruction, int fd_socket) {
  t_Package *package = package_create_with_header(CPU_INSTRUCTION_HEADER);
  cpu_instruction_serialize(package->payload, instruction);
  package_send(package, fd_socket);
  package_destroy(package);
}

void cpu_instruction_serialize(t_Payload *payload, t_CPU_Instruction *instruction) {
    
    payload_enqueue(payload, &(instruction->opcode), sizeof(e_CPU_Opcode));
    int campos = list_size(instruction->parameters);
    payload_enqueue(payload, &(campos), sizeof(int));

    for (size_t i = 0; i < campos ; i++)
    {
      char *contenido = list_get(instruction->parameters, i);
      int len = strlen(contenido);
      payload_enqueue(payload, &(len), sizeof(int));
      payload_enqueue(payload, &(contenido), len);
    }
  
}

t_CPU_Instruction *cpu_instruction_deserialize(t_Payload *payload) {
  t_CPU_Instruction *instruction = malloc(sizeof(t_CPU_Instruction));

  uint32_t offset = 0;
  t_list* lista_campos = list_create();

  offset = memcpy_deserialize(&(instruction->opcode), payload->stream, offset, sizeof(e_CPU_Opcode));
  int elementos = 0;
  offset = memcpy_deserialize(&(elementos), payload->stream, offset, sizeof(int));

  for (size_t i = 0; i < elementos; i++)
  {
    char* campo = string_new();
    int len=0;

    offset = memcpy_deserialize(&(len), payload->stream, offset, sizeof(int));
    offset = memcpy_deserialize(&(campo), payload->stream, offset, len);

    list_add(lista_campos, campo);
  }
  
  cpu_instruction_print(instruction);
  return instruction;
}

void delete_instruction(t_CPU_Instruction* instr){
  int elementos = list_size(instr->parameters);
  for (size_t i = 0; i < elementos; i++)
  {
    char* campo = list_get(instr->parameters,i);
    free(campo);
  }
  
  free(instr);

}

void cpu_instruction_print(t_CPU_Instruction *instruction) {
    log_info(SERIALIZE_LOGGER, "INSTRUCCION: %d\n", instruction->opcode);
    int size = list_size(instruction->parameters);
    
    for (size_t i = 0; i < size; i++)
    {
      char * parametro = list_get(instruction->parameters,i);
      log_info(SERIALIZE_LOGGER, "PARAMETRO [%ld]: %s :\n", i, parametro);
    }
}