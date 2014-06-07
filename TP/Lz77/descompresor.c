/* 
 * File:   descompresor.c
 * Author: federico
 */

#include <netinet/in.h>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Lz77.h"
#include "archivos.h"

int revisarNombreDeArchivo(char *nombreArchivo) {
  return !(strcmp(nombreArchivo + strlen(nombreArchivo) - 3, ".xx"));
}

FILE* crearArchivoOriginal(char *nombreArchivo) {
  char *nombreArchivoSalida;
  FILE *archivoSalida;

  nombreArchivoSalida = malloc(strlen(nombreArchivo) - 2);
  strncpy(nombreArchivoSalida, nombreArchivo, strlen(nombreArchivo) - 3);

  archivoSalida = fopen(nombreArchivoSalida, "wb");
  free(nombreArchivoSalida);

  return archivoSalida;
}

void shiftearBitsBufferIzquierda(BufferProceso *buffer, int cantidad) {
  buffer->buffer = ntohl(buffer->buffer);
  buffer->buffer = buffer->buffer << cantidad;
  buffer->buffer = htonl(buffer->buffer);
}

void shiftearBitsBufferDerecha(BufferProceso *buffer, int cantidad) {
  buffer->buffer = ntohl(buffer->buffer);
  buffer->buffer = buffer->buffer >> cantidad;
  buffer->buffer = htonl(buffer->buffer);
}

void ponerCaracterEnVentana(char caracter, char *ventana, FILE *archivo) {
  memcpy(ventana, ventana + 1, TAM_VENTANA - 1);
  memcpy(ventana + TAM_VENTANA - 1, &caracter, 1);
  
  fwrite(&caracter, 1, 1, archivo);
}

void ponerReferenciaEnVentana(int posicion, int longitud, char *ventana, FILE *archivo) {
  int i;
  
  for (i = 0; i < longitud; i++) {
    ponerCaracterEnVentana(ventana[posicion], ventana, archivo);  
  }
}

int descomprimir(char *nombreArchivo) {
  unsigned int posicion, longitud, bytesAAgregar, bitsADesplazar, esReferencia, bytesLeidos, deboQuitarRelleno;
  char *ventana, caracter;
  FILE *archivoEntrada, *archivoSalida;
  BufferProceso buf;
  unsigned char tmp[4];
  
  if (!revisarNombreDeArchivo(nombreArchivo)) {
    return ERR_ARCHIVO_NOMBRE_INVALIDO;
  }
  
  if ((archivoEntrada = fopen(nombreArchivo, "rb")) == NULL) {
    return ERR_ARCHIVO_NO_EXISTE;
  }
  
  deboQuitarRelleno = 1;
  ventana = malloc(TAM_VENTANA);
  memset(ventana, 0, TAM_VENTANA);
  buf.bitsOcupados = 0;
  archivoSalida = crearArchivoOriginal(nombreArchivo);

  buf.bitsOcupados = 8 * fread(&buf.buffer, 1, sizeof(buf.buffer), archivoEntrada);
  
  while (buf.bitsOcupados > 0) {
    buf.buffer = ntohl(buf.buffer);
    esReferencia = buf.buffer & (1 << (sizeof(buf.buffer) * 8 - 1));
    buf.buffer = htonl(buf.buffer);
    
    if (esReferencia) {
      // referencia con longitud y posicion
      shiftearBitsBufferIzquierda(&buf, 1);
      
      // TODO: Extraer esto a una funcion!
      buf.buffer = ntohl(buf.buffer);
      longitud = buf.buffer;
      longitud = longitud >> (sizeof(longitud) * 8 - BITS_LONGITUD);
      buf.buffer <<= BITS_LONGITUD;
      posicion = buf.buffer;
      posicion = posicion >> (sizeof(posicion) * 8 - BITS_POSICION);
      buf.buffer <<= BITS_POSICION;
      buf.buffer = htonl(buf.buffer);
      
      ponerReferenciaEnVentana(posicion, longitud, ventana, archivoSalida);
      buf.bitsOcupados -= 1 + BITS_POSICION + BITS_LONGITUD;
    } else {
      // caracter literal
      shiftearBitsBufferIzquierda(&buf, 1);
      memcpy(&caracter, &(buf.buffer), 1);
      shiftearBitsBufferIzquierda(&buf, 8);
      buf.bitsOcupados -= 9;
      ponerCaracterEnVentana(caracter, ventana, archivoSalida);
    }
    
    if (buf.bitsOcupados < (sizeof(buf.buffer) * 8 - 8) && deboQuitarRelleno) {
      // hay que meterle mas data al buffer!
      bytesAAgregar = (sizeof(buf.buffer) * 8 - buf.bitsOcupados) / 8;
      bitsADesplazar = sizeof(buf.buffer) * 8 - bytesAAgregar * 8 - buf.bitsOcupados;
      shiftearBitsBufferDerecha(&buf, bitsADesplazar);
      
      bytesLeidos = fread((char*)(&(buf.buffer)) + 4 - bytesAAgregar, 1, bytesAAgregar, archivoEntrada);
      if (bytesLeidos != bytesAAgregar) {
        // si pido 3 bytes y leo 2 (porque no hay mas archivo) relleno el byte que me queda con ceros
        memset((char*)(&(buf.buffer)) + 4 - bytesAAgregar + bytesLeidos, 0, bytesAAgregar - bytesLeidos);
      }
      
      shiftearBitsBufferIzquierda(&buf, bitsADesplazar);
      buf.bitsOcupados += bytesAAgregar * 8;
    }
    
    if (feof(archivoEntrada) && deboQuitarRelleno) {
      // si no hay mas archivo, entonces tengo que sacar el padding que consiste de 1000...000
      // para ello, voy shifteando de a un bit hasta que el valor del buffer aplicado con bitwise
      // AND a 0x0000000...001 me de algo distinto a cero
      buf.buffer = ntohl(buf.buffer);
      
      bitsADesplazar = 0;
      while (!((buf.buffer >> bitsADesplazar) & 1)) {
        bitsADesplazar++;
      }
      
      buf.buffer = htonl(buf.buffer);
      buf.bitsOcupados = 31 - bitsADesplazar;
      deboQuitarRelleno = 0;
      
      
      memcpy(tmp, &buf.buffer, 4);
    }
  }
  
  free(ventana);
  
  return EXIT_SUCCESS;
}
