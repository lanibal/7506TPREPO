#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "Cola.h"
#include "Nodo.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <algorithm>
#include <arpa/inet.h>


using namespace std;

const int MAX=271;
const int MAXFIJO=8192;

typedef struct {
  uint32_t buffer;
  unsigned int bitsOcupados;
} Buffer;


class Huffman {

private:
	const static int LONG_MAX_CODIGO_HUFF = 8;


public:

	int caracteres[MAX];	// Guardo todos los posibles caracteres en el inicio del arbol.
	int str;				// Vector auxiliar para copiar los caracteres a los nodos.
	int freq[MAX];			// Vector auxiliar para copiar las frecuencias a los nodos.
	bool c[MAX];			// Vector donde guardo el codigo meintras recorro el arbol.
	int sz;					// Longitud de codigo;
	Nodo* inicialCompleto[MAX];
	Nodo* a[MAX];			// ArbolCompersor
	Buffer buffer;
	Nodo* nodoDescompresor;
	FILE* archivoSalida;
	bool caracterNuevo, termino, buscarPosicion ;
	Nodo* fijo[MAXFIJO];
	int posiciones[MAXFIJO];
	Cola* codigo;
	int cantidadAimprimir, i;
	Huffman();
	Nodo* crearNodo(int a, int x);
	void  ordenarNodos(Nodo* a[], int n);
	void  crearArbol(Nodo* a[], int numeroMaximo);
	void  calcularCodigo(Nodo* arbol, bool c[], int n,int codigoChar);
	void comprimirCaracter(int codigo);
	void emitirRelleno();
	void acomodarNodos(Nodo* a[], int n);
	void descomprimirCodigo(int cod);
	void calcularCaracter(Nodo* arbol,int cod,int* numero);
	void setCodigo(Cola* codigo);
	void armarArbolPosicion();
	int pasarLongitudaNumero(int longitud);
	void emitirCodigo(bool codigo[], int longitudCodigo);
	void setArchivoSalida(FILE* archivoSalida);
	void comprimirLongitudyPosicion(int codigo,Nodo*nodoActual[],bool arbolDinamico);
	void emitirCaracterDescomprimido(int numeroCaracter);
	void emitirLongitudDescomprimido(int numeroLongitud);
	void emitirPosicionDescomprimido(int numeroPosicion);
};

#endif /* HUFFMAN_H_ */
