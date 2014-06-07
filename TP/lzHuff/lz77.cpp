/* 
 * File:   lz77.c
 * Author: federico
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lz77.h"
#include "compresor.h"
#include "descompresor.h"


int lz77(int accion, char* archivo) {
  int resultado;

  resultado = EXIT_SUCCESS;

  if (accion == ACCION_COMPRIMIR) {
    resultado = comprimirLZ77(archivo);
  } else {
    resultado = descomprimirLZ77(archivo);
  }
  
  return resultado;
}