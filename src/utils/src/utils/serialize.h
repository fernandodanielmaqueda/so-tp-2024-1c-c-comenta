/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "estructuras.h"
#include "package.h"

#ifndef DEBUG_SERIALIZATION
#define DEBUG_SERIALIZATION 0
#endif

#define DEBUGGING_SERIALIZATION if(DEBUG_SERIALIZATION)

typedef struct t_instruction_use {
	enum HeaderCode operation;
	t_list* parameters;
} t_instruction_use;


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param codigoOperacion Codigo de operacion a enviar
 * @param mensaje Mensaje a enviar
 * @param socket Socket destino
 */
void message_send(enum HeaderCode codigoOperacion, char* mensaje, int socket);

/**
 * @brief Eliminar pcb.
 * @param pcb_target t_pcb a eliminar.
 */
//void kill_pcb(t_pcb *pcb_target);


/**
 * @brief Eliminar instruccion.
 * @param lineInstruccion t_instruccion a eliminar.
 */
//void instruction_delete(t_instruccion *lineInstruccion);


/**
 * @brief Serializacion del t_pcb para ser enviada.
 * @param package Package a rellenar.
 * @param pcb Pcb a serializar
 */
void serialize_pcb(Package *package, t_pcb *pcb);


/**
 * @brief Deserializacion del t_pcb para ser enviada.
 * @param socketClient Socket desde donde se va a recibir el pcb.
 */
t_pcb* deserialize_pcb(int socketClient);


/**
 * @brief Enviar pcb (incluye el serializado)
 * @param pcbEnviado t_pcb a enviar.
 * @param socket Socket desde donde se va a recibir el pcb.
 */
void send_pcb_to(t_pcb* pcbEnviado, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param instruccion Instruccion a enviar
 * @param socket Socket destino
 */
void send_instruccion(t_instruction_use* instruccion, int socket);


/**
 * @brief Obtiene el codigo de operacion de un paquete
 * @param socket Socket a recibir
 */
t_instruction_use* receive_instruccion(int socket);


/**
 * @brief Libera la memoria reservada para una instruccion determinada.
 * @param lineaInstruccion Instruccion a liberar
 */
void instruction_delete(t_instruction_use *lineaInstruccion);

void serialize_pcb_2(Package* paquete, t_pcb* pcb);
void send_pcb(int socket, t_pcb* pcb);
void deserialize_pcb_2(t_pcb* pcb, void* stream);
void receive_pcb(int socket, t_pcb *pcb);

#endif // SERIALIZE_H