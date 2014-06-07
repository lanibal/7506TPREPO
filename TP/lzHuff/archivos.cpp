/* 
 * File:   archivos.c
 * Author: federico
 */

#include <stdio.h>
#include "archivos.h"

int obtenerTamanio(FILE* archivo) {
  int posActual, tamanio;

  posActual = ftell(archivo);
  fseek(archivo, 0, SEEK_END);

  tamanio = ftell(archivo);
  fseek(archivo, posActual, SEEK_SET);

  return tamanio;
}