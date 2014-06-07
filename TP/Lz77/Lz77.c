/* 
 * File:   main.c
 * Author: federico
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lz77.h"
#include "compresor.h"
#include "descompresor.h"

void imprimirInstrucciones() {
  printf("%s\n", "Uso: compresor [opcion] [archivo]");
  printf("%s\n", "");
  printf("%s\n", "  -c [archivo]     Comprime el archivo [archivo], generando [archivo].xx");
  printf("%s\n", "  -x [archivo]     Descomprime el archivo [archivo], recreando el original");
}

int parsearParametros(int argc, char** argv) {
  if (argc != 3) {
    return ACCION_DESCONOCIDA;
  } else {
    if (!strcmp(argv[1], "-c")) {
      return ACCION_COMPRIMIR;
    } else if (!strcmp(argv[1], "-x")) {
      return ACCION_DESCOMPRIMIR;
    } else {
      return ACCION_DESCONOCIDA;
    }
  }
}

void imprimirError(int codigo) {
  if (codigo == ERR_ARCHIVO_NO_EXISTE) {
    printf("%s\n", "El archivo especificado no existe");
  } else if (codigo == ERR_ARCHIVO_NOMBRE_INVALIDO) {
    printf("%s\n", "El archivo especificado debe tener extension .xx");
  }
}

//int main(int argc, char** argv) {
//  int accion, resultado;
//
//  accion = parsearParametros(argc, argv);
//  resultado = EXIT_SUCCESS;
//
//  if (accion == ACCION_COMPRIMIR) {
//    resultado = comprimir(argv[2]);
//  } else if (accion == ACCION_DESCOMPRIMIR) {
//    resultado = descomprimir(argv[2]);
//  } else {
//    imprimirInstrucciones();
//  }
//
//  if (resultado != EXIT_SUCCESS) {
//    imprimirError(resultado);
//  }
//
//  return resultado;
//}
