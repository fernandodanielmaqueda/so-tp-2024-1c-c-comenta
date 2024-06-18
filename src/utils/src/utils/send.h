/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SEND_H
#define UTILS_SEND_H

#include "utils/package.h"
#include "utils/serialize/cpu_memory_request.h"
#include "utils/serialize/cpu_opcode.h"
#include "utils/serialize/eviction_reason.h"
#include "utils/serialize/kernel_interrupt.h"
#include "utils/serialize/pcb.h"
#include "utils/serialize/subheader.h"
#include "utils/serialize/subpayload.h"
#include "utils/serialize/text.h"


/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcb t_PCB a enviar.
 * @param fd_socket Socket desde donde se va a recibir el pcb.
 */
void send_cpu_memory_request(e_CPU_Memory_Request *memory_request, int socket);


/**
 * @brief Enviar cpu_opcode (incluye el serializado)
 * @param cpu_opcode e_CPU_OpCode a enviar.
 * @param fd_socket Socket desde donde se va a recibir el cpu_opcode.
 */
void send_cpu_opcode(e_CPU_OpCode *cpu_opcode, int fd_socket);


/**
 * @brief Enviar eviction_reason (incluye el serializado)
 * @param eviction_reason e_Eviction_Reason a enviar.
 * @param fd_socket Socket desde donde se va a recibir el eviction_reason.
 */
void send_eviction_reason(e_Eviction_Reason *eviction_reason, int fd_socket);


/**
 * @brief Enviar kernel_interrupt (incluye el serializado)
 * @param kernel_interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el kernel_interrupt.
 */
void send_kernel_interrupt(e_Kernel_Interrupt *kernel_interrupt, int fd_socket);


/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcb t_PCB a enviar.
 * @param fd_socket Socket desde donde se va a recibir el pcb.
 */
void send_pcb(t_PCB *pcb, int fd_socket); //INT HEADER TERCER PARAMETREO ENVIO PCB A CUALQUIEWR SOCKET


/**
 * @brief Enviar interrupt (incluye el serializado)
 * @param interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el interrupt.
 */
void send_header(e_Header *header, int fd_socket);


/**
 * @brief Enviar texto (incluye el serializado)
 * @param string Texto a enviar.
 * @param fd_socket Socket desde donde se va a recibir el texto.
 */
void send_text(char *text, int fd_socket);


//

void send_string(char* mensaje, int socket, int opcod);
void send_int(int nro, int socket, int opcod);
void send_2int(int nro, int nro2, int socket, int opcod);
void receive_2int(int* nro1, int* nro2, t_Payload* payload);
void send_String_1int(int nro, char* mensaje, int socket, int opcod);
void receive_String_1int(int* pid, char** mensaje, t_Payload* payload);
void receive_2int_1uint32(int* nro1, int* nro2,uint32_t* contenido, t_Payload* payload);
void send_2int_1uint32(int nro1, int nro2,uint32_t contenido, int socket, int opcod);
void receive_write_request(int* pid, int* dir_fis, int* bytes, char* contenido, t_Payload* payload);
void send_write_request(int pid, int dir_fis, char* mensaje, int socket, int opcod);
void receive_read_request(int* pid, int* dir_fis, int* bytes, t_Payload* payload);
void send_read_request(int pid, int dir_fis, int bytes, int socket, int opcod);

#endif // UTILS_SEND_H