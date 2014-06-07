/* 
 * File:   main.h
 * Author: federico
 */

#ifndef LZ77_H

#include <inttypes.h>

#define	lz77_H

#define ACCION_COMPRIMIR 1
#define ACCION_DESCOMPRIMIR 2
#define ACCION_DESCONOCIDA 3

#define ERR_ARCHIVO_NO_EXISTE 1
#define ERR_ARCHIVO_NOMBRE_INVALIDO 2

#define TAM_VENTANA 8192
#define BITS_POSICION 13
#define BITS_LONGITUD 4
#define LONGITUD_MINIMA_MATCH 3
#define LONGITUD_MAXIMA_MATCH 15

typedef struct {
  uint32_t buffer;
  unsigned int bitsOcupados;
} BufferProceso;


#endif	/* MAIN_H */
