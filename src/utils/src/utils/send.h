/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SEND_H
#define UTILS_SEND_H

#include "utils/package.h"
#include "utils/serialize/cpu_opcode.h"
#include "utils/serialize/eviction_reason.h"
#include "utils/serialize/kernel_interrupt.h"
#include "utils/serialize/pcb.h"
#include "utils/serialize/return_value.h"
#include "utils/serialize/subheader.h"
#include "utils/serialize/subpayload.h"
#include "utils/serialize/text.h"


/**
 * @brief Enviar interrupt (incluye el serializado)
 * @param interrupt t_Interrupt a enviar.
 * @param fd_socket Socket desde donde se va a recibir el interrupt.
 */
void send_header(e_Header header, int fd_socket);


void send_return_value_with_header(e_Header header, t_Return_Value return_value, int fd_socket);
void receive_return_value_with_header(e_Header expected_header, t_Return_Value *return_value, int fd_socket);


/**
 * @brief Enviar eviction_reason (incluye el serializado)
 * @param eviction_reason e_Eviction_Reason a enviar.
 * @param fd_socket Socket desde donde se va a recibir el eviction_reason.
 */
void send_eviction_reason(e_Eviction_Reason eviction_reason, int fd_socket);


void send_kernel_interrupt(e_Kernel_Interrupt type, t_PID pid, int fd_socket);


/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcb t_PCB a enviar.
 * @param fd_socket Socket desde donde se va a recibir el pcb.
 */
void send_pcb(t_PCB pcb, int fd_socket); //INT HEADER TERCER PARAMETREO ENVIO PCB A CUALQUIEWR SOCKET


/**
 * @brief Enviar texto (incluye el serializado)
 * @param string Texto a enviar.
 * @param fd_socket Socket desde donde se va a recibir el texto.
 */
void send_text_with_header(e_Header header, char *text, int fd_socket);

void receive_text_with_header(e_Header header, char **text, int fd_socket);

#endif // UTILS_SEND_H