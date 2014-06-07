#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Huffman.h"
#include "Nodo.h"
#include "lz77.h"
#include "compresor.h"
#include <ostream>
#include <fstream>
#include <math.h>
#include <sstream>

#define BITS_POSICION 13
#define BITS_LONGITUD 4

void shiftearBitsBufferIzquierda(Buffer *buffer, int cantidad) {
  buffer->buffer = ntohl(buffer->buffer);
  buffer->buffer = buffer->buffer << cantidad;
  buffer->buffer = htonl(buffer->buffer);
}

void shiftearBitsBufferDerecha(Buffer *buffer, int cantidad) {
  buffer->buffer = ntohl(buffer->buffer);
  buffer->buffer = buffer->buffer >> cantidad;
  buffer->buffer = htonl(buffer->buffer);
}

void leerArchivoComprimir(FILE* archivoEntrada, FILE* archivoSalida, Huffman*arbolCompresor) {

  unsigned int posicion, longitud, bytesAAgregar, bitsADesplazar, esReferencia, bytesLeidos, deboQuitarRelleno;
  unsigned char caracter;
  arbolCompresor->setArchivoSalida(archivoSalida);
  Buffer buf;
  deboQuitarRelleno = 1;
  buf.bitsOcupados = 0;

  buf.bitsOcupados = 8 * fread(&buf.buffer, 1, sizeof (buf.buffer), archivoEntrada);

  while (buf.bitsOcupados > 0) {
    buf.buffer = ntohl(buf.buffer);
    esReferencia = buf.buffer & (1 << (sizeof (buf.buffer) * 8 - 1));
    buf.buffer = htonl(buf.buffer);

    if (esReferencia) {
      // referencia con longitud y posicion
      shiftearBitsBufferIzquierda(&buf, 1);

      // TODO: Extraer esto a una funcion!
      buf.buffer = ntohl(buf.buffer);
      longitud = buf.buffer;
      longitud = longitud >> (sizeof (longitud) * 8 - BITS_LONGITUD);
      buf.buffer <<= BITS_LONGITUD;
      posicion = buf.buffer;
      posicion = posicion >> (sizeof (posicion) * 8 - BITS_POSICION);
      buf.buffer <<= BITS_POSICION;
      buf.buffer = htonl(buf.buffer);

      arbolCompresor->comprimirLongitudyPosicion(longitud, arbolCompresor->a, true); //Es una longitud
      arbolCompresor->comprimirLongitudyPosicion(posicion, arbolCompresor->fijo, false); // Es una posicion

      buf.bitsOcupados -= 1 + BITS_POSICION + BITS_LONGITUD;
    } else {
      // caracter literal
      shiftearBitsBufferIzquierda(&buf, 1);
      memcpy(&caracter, &(buf.buffer), 1);
      shiftearBitsBufferIzquierda(&buf, 8);
      buf.bitsOcupados -= 9;
      arbolCompresor->comprimirCaracter(caracter);

    }

    if (buf.bitsOcupados < (sizeof (buf.buffer) * 8 - 8) && deboQuitarRelleno) {
      // hay que meterle mas data al buffer!
      bytesAAgregar = (sizeof (buf.buffer) * 8 - buf.bitsOcupados) / 8;
      bitsADesplazar = sizeof (buf.buffer) * 8 - bytesAAgregar * 8 - buf.bitsOcupados;
      shiftearBitsBufferDerecha(&buf, bitsADesplazar);

      bytesLeidos = fread((char*) (&(buf.buffer)) + 4 - bytesAAgregar, 1, bytesAAgregar, archivoEntrada);
      if (bytesLeidos != bytesAAgregar) {
        // si pido 3 bytes y leo 2 (porque no hay mas archivo) relleno el byte que me queda con ceros
        memset((char*) (&(buf.buffer)) + 4 - bytesAAgregar + bytesLeidos, 0, bytesAAgregar - bytesLeidos);
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

    }
  }
  arbolCompresor->emitirRelleno();
}

void aplicarHuffman(char *nombreOriginal) {
  FILE *archivoEntrada, *archivoSalida;
  char *nombreArchivoTemporal, *nombreArchivoFinal;

  nombreArchivoTemporal = (char*) malloc(strlen(nombreOriginal) + 8);
  strcpy(nombreArchivoTemporal, nombreOriginal);
  strcat(nombreArchivoTemporal, ".xx.tmp");

  nombreArchivoFinal = (char*) malloc(strlen(nombreOriginal) + 4);
  strcpy(nombreArchivoFinal, nombreOriginal);
  strcat(nombreArchivoFinal, ".xx");

  archivoEntrada = fopen(nombreArchivoTemporal, "rb");

  archivoSalida = fopen(nombreArchivoFinal, "wb");
  printf("Comprimiendo...\n");

  Huffman *arbolCompresor = new Huffman(); //Creo los vectores de frecuencias y caracteres Iniciales.
  arbolCompresor->armarArbolPosicion();

  leerArchivoComprimir(archivoEntrada, archivoSalida, arbolCompresor);

  fclose(archivoSalida);
  fclose(archivoEntrada);

  free(nombreArchivoTemporal);
  free(nombreArchivoFinal);
}

void leerArchivoDescomprimir(FILE* archivoEntrada, FILE* archivoSalida, Huffman*arbolDescompresor) {
  arbolDescompresor->setArchivoSalida(archivoSalida);
  Buffer bufferLectura;
  int deboQuitarRelleno, bytesAAgregar, bitsADesplazar, bytesLeidos;
  bufferLectura.bitsOcupados = 0;
  bufferLectura.buffer = 0;

  bufferLectura.bitsOcupados = 8 * fread(&bufferLectura.buffer, 1, sizeof (bufferLectura.buffer), archivoEntrada);

  bufferLectura.buffer = ntohl(bufferLectura.buffer);
  deboQuitarRelleno = 1;

  while (bufferLectura.bitsOcupados > 0) {

    if (bufferLectura.buffer & (uint32_t) pow(2, (sizeof (bufferLectura.buffer) * 8 - 1))) {

      arbolDescompresor->descomprimirCodigo(1);

    } else {

      arbolDescompresor->descomprimirCodigo(0);
    }

    bufferLectura.buffer <<= 1;
    bufferLectura.bitsOcupados--;

    if (bufferLectura.bitsOcupados <= (sizeof (bufferLectura.buffer) * 8 - 8) && deboQuitarRelleno) {
      // hay que meterle mas data al buffer!
      bytesAAgregar = (sizeof (bufferLectura.buffer) * 8 - bufferLectura.bitsOcupados) / 8;
      bitsADesplazar = sizeof (bufferLectura.buffer) * 8 - bytesAAgregar * 8 - bufferLectura.bitsOcupados;

      bytesLeidos = fread((char*) (&(bufferLectura.buffer)), 1, bytesAAgregar, archivoEntrada);
      if (bytesLeidos != bytesAAgregar) {
        // si pido 3 bytes y leo 2 (porque no hay mas archivo) relleno el byte que me queda con ceros
        memset((char*) (&(bufferLectura.buffer)) + bytesLeidos, 0, bytesAAgregar - bytesLeidos);

      }

      bufferLectura.bitsOcupados += bytesAAgregar * 8;

    }

    if (feof(archivoEntrada) && deboQuitarRelleno) {
      // si no hay mas archivo, entonces tengo que sacar el padding que consiste de 1000...000
      // para ello, voy shifteando de a un bit hasta que el valor del buffer aplicado con bitwise
      // AND a 0x0000000...001 me de algo distinto a cero

      bitsADesplazar = 0;
      while (!((bufferLectura.buffer >> bitsADesplazar) & 1)) {
        bitsADesplazar++;
      }
      // bufferLectura.buffer >> bitsADesplazar;
      bufferLectura.bitsOcupados = 31 - bitsADesplazar;
      deboQuitarRelleno = 0;
    }
  }
  arbolDescompresor->emitirRelleno();
}

char *revertirHuffman(char *nombreOriginal) {
  FILE *archivoEntrada, *archivoSalida;
  char *nombreArchivoTemporal;
  
  nombreArchivoTemporal = (char*) malloc(strlen(nombreOriginal) + 5);
  strcpy(nombreArchivoTemporal, nombreOriginal);
  strcat(nombreArchivoTemporal, ".tmp");
  
  archivoEntrada = fopen(nombreOriginal, "rb");
  archivoSalida = fopen(nombreArchivoTemporal, "wb");

  printf("\nDescomprimiendo...\n");

  Huffman* arbolDescompresor = new Huffman();
  arbolDescompresor->armarArbolPosicion();

  leerArchivoDescomprimir(archivoEntrada, archivoSalida, arbolDescompresor);
  fclose(archivoEntrada);
  fclose(archivoSalida);

  return nombreArchivoTemporal;
}

void imprimirInstrucciones() {
  printf("%s\n", "Uso: compresor [opcion] [archivo]");
  printf("%s\n", "");
  printf("%s\n", "  -c [archivo]     Comprime el archivo [archivo], generando [archivo].xx");
  printf("%s\n", "  -x [archivo]     Descomprime el archivo [archivo], recreando el original");
}

void imprimirError(int codigo) {
  if (codigo == ERR_ARCHIVO_NO_EXISTE) {
    printf("%s\n", "El archivo especificado no existe");
  } else if (codigo == ERR_ARCHIVO_NOMBRE_INVALIDO) {
    printf("%s\n", "El archivo especificado debe tener extension .xx");
  }
}

void eliminarTemporal(char *nombreOriginal, char *sufijo) {
  char *nombreArchivoTemporal;

  nombreArchivoTemporal = (char*) malloc(strlen(nombreOriginal) + strlen(sufijo) + 1);
  strcpy(nombreArchivoTemporal, nombreOriginal);
  strcat(nombreArchivoTemporal, sufijo);
  
  unlink(nombreArchivoTemporal);
  
  free(nombreArchivoTemporal);
}

//int main(int argc, char** argv) {
//  int resultado;
//  char *nombreArchivoTemporal;
//
//  resultado = EXIT_SUCCESS;
//
//  if (argc >= 2 && strcmp(argv[1], "-c") == 0) {
//    resultado = lz77(ACCION_COMPRIMIR, argv[2]);
//
//    if (resultado != EXIT_SUCCESS) {
//      imprimirError(resultado);
//    } else {
//      aplicarHuffman(argv[2]);
//    }
//
//    eliminarTemporal(argv[2], ".xx.tmp");
//
//  } else if (argc >= 2 && strcmp(argv[1], "-x") == 0) {
//    nombreArchivoTemporal = revertirHuffman(argv[2]);
//    lz77(ACCION_DESCOMPRIMIR, nombreArchivoTemporal);
//    free(nombreArchivoTemporal);
//    eliminarTemporal(argv[2], ".tmp");
//  } else {
//    imprimirInstrucciones();
//  }
//
//  return resultado;
//}
