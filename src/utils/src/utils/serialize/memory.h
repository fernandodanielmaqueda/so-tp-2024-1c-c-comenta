/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#ifndef UTILS_SERIALIZE_MEMORY_H
#define UTILS_SERIALIZE_MEMORY_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "commons/config.h"
#include "commons/string.h"
#include "utils/package.h"

#include "commons/log.h"
#include "utils/module.h"

typedef uint32_t t_MemorySize;
typedef uint32_t t_Logical_Address;
typedef uint32_t t_Physical_Address;
typedef uint32_t t_Offset;
typedef uint32_t t_Page_Number;
typedef t_Page_Number t_Frame_Number;
typedef uint32_t t_Page_Quantity;

#endif // UTILS_SERIALIZE_MEMORY_H