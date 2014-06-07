/* 
 * File:   compresor.c
 * Author: federico
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include "compresor.h"
#include "lz77.h"
#include "archivos.h"

unsigned int buscarCoincidencia(char *ventana, char *buffer, unsigned int *posicion) {
  /* TODO: Hacer las mejoras correspondientes aca! */
  int longitud, maxLongitud, i, j;

  maxLongitud = 0;

  for (i = 0; i < TAM_VENTANA; i++) {
    longitud = 0;

    for (j = 0; j < TAM_VENTANA; j++) {
      if (ventana[i + j] == buffer[j]) {
        longitud++;
      } else {
        break;
      }

      if (longitud > maxLongitud) {
        *posicion = i;
        maxLongitud = longitud;
      }

      if (maxLongitud == LONGITUD_MAXIMA_MATCH) {
        break;
      }
    }
  }

  return maxLongitud;
}

void desplazarVentana(char *ventana, int cantidad) {
  memcpy(ventana, ventana + cantidad, 2 * TAM_VENTANA - cantidad);
}

void emitir(BufferProceso *buf, int valor, int numeroDeBits, FILE *archivo) {
  unsigned int bitsADesplazar, bytesAEscribir;
  uint32_t datoAGuardar;
  
  buf->buffer <<= numeroDeBits;
  buf->bitsOcupados += numeroDeBits;
  buf->buffer |= valor;

  bytesAEscribir = buf->bitsOcupados / 8;

  if (bytesAEscribir > 0) {
    bitsADesplazar = 8 * sizeof (buf->buffer) - buf->bitsOcupados;
    buf->buffer <<= bitsADesplazar;

    datoAGuardar = htonl(buf->buffer);
    fwrite((char*)(&datoAGuardar), 1, bytesAEscribir, archivo);
    buf->bitsOcupados -= bytesAEscribir * 8;
    buf->buffer >>= bitsADesplazar;
  }
}

void emitirRelleno(BufferProceso *buf, FILE *archivo) {
  unsigned int bitsARellenar, relleno;

  bitsARellenar = 8 - buf->bitsOcupados;

  if (bitsARellenar == 0) {
    bitsARellenar = 8;
  }

  relleno = 1;
  relleno <<= (bitsARellenar - 1);

  emitir(buf, relleno, bitsARellenar, archivo);
}

void emitirReferencia(int posicion, int longitud, BufferProceso *buf, FILE *archivo) {
  unsigned int temp;

  temp = 1 << (BITS_LONGITUD + BITS_POSICION);
  temp |= (longitud << BITS_POSICION);
  temp |= posicion;

  emitir(buf, temp, (BITS_LONGITUD + BITS_POSICION + 1), archivo);
}

void emitirCaracter(unsigned char caracter, BufferProceso *buf, FILE *archivo) {
  emitir(buf, (unsigned int) caracter, 9, archivo);
}

FILE* crearArchivoSalida(char *nombreArchivo) {
  char *nombreArchivoSalida;
  FILE *archivoSalida;

  nombreArchivoSalida = (char*)malloc(strlen(nombreArchivo) + 8);
  strcpy(nombreArchivoSalida, nombreArchivo);
  strcat(nombreArchivoSalida, ".xx.tmp");

  archivoSalida = fopen(nombreArchivoSalida, "wb");
  free(nombreArchivoSalida);

  return archivoSalida;
}

int comprimirLZ77(char *nombreArchivo) {
  unsigned int posicion, tamanio, procesado, longitud;
  char *ventana, *buffer;
  FILE *archivoEntrada, *archivoSalida;
  BufferProceso bufferSalida;

  if ((archivoEntrada = fopen(nombreArchivo, "rb")) == NULL) {
    return ERR_ARCHIVO_NO_EXISTE;
  }

  archivoSalida = crearArchivoSalida(nombreArchivo);

  ventana = (char*)malloc(TAM_VENTANA * 2);
  memset(ventana, 0, TAM_VENTANA * 2);
  buffer = ventana + TAM_VENTANA;

  fread(buffer, 1, TAM_VENTANA, archivoEntrada);

  tamanio = obtenerTamanio(archivoEntrada);
  procesado = 0;

  /* TODO: PASAR A UN TDA! ESTO ESTÁ MAL! */
  bufferSalida.bitsOcupados = 0;
  bufferSalida.buffer = 0;

  while (procesado < tamanio) {
    longitud = buscarCoincidencia(ventana, buffer, &posicion);

    if (longitud >= LONGITUD_MINIMA_MATCH) {
      if (procesado + longitud > tamanio) {
        emitirReferencia(posicion, tamanio - procesado, &bufferSalida, archivoSalida);
      } else {
        emitirReferencia(posicion, longitud, &bufferSalida, archivoSalida);
      }
    } else {
      emitirCaracter(buffer[0], &bufferSalida, archivoSalida);
      longitud = 1;
    }

    desplazarVentana(ventana, longitud);
    fread(ventana + TAM_VENTANA * 2 - longitud, 1, longitud, archivoEntrada);
    procesado += longitud;
  }

  emitirRelleno(&bufferSalida, archivoSalida);

  /* Limpieza de recursos */
  fclose(archivoEntrada);
  fclose(archivoSalida);
  free(ventana);

  return EXIT_SUCCESS;
}