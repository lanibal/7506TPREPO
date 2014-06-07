#include "Huffman.h"

Huffman::Huffman() {
	this->sz = 0;
	this->buffer.bitsOcupados = 0;
	this->buffer.buffer = 0;
	this->caracterNuevo = true;
	this->termino = false;
	this->buscarPosicion = false;
	this->codigo = new Cola();
	this->cantidadAimprimir = 1;
	this->i = 1;

	for (int i = 0; i < MAX; i++) {
		inicialCompleto[i] = crearNodo(i, 1);
		a[i] = inicialCompleto[i]; // Asigno al vector "a" todo el vector inicialCompleto
	}
}

Nodo* Huffman::crearNodo(int a, int x) {

	Nodo* nodoNuevo = new Nodo();
	nodoNuevo->setFrecuencia(x);
	nodoNuevo->setNumero(a);
	nodoNuevo->setHijoDerecho(NULL);
	nodoNuevo->setHijoIzquierdo(NULL);
	return nodoNuevo;
}

bool CompareNodes(const Nodo* n1, const Nodo* n2) {
	return n1->getFrecuencia() < n2->getFrecuencia();
}

void Huffman::ordenarNodos(Nodo* a[], int n) {
	std::sort(a, a + n, CompareNodes);
}

void Huffman::acomodarNodos(Nodo* a[], int n) {
	for (int i = 1; i < n - 1; i++) {
		delete a[i];
		a[i] = a[i + 1];
	}
}

void Huffman::crearArbol(Nodo* nodoActual[], int numeroMaximo) {
	bool yaEncontrePosicion;
	int u = 0, posicionInsercion, frecuencia;
	Nodo* ptr;
	ordenarNodos(nodoActual, numeroMaximo);

	for (int n = numeroMaximo; n > 1; n--) {
		//ordenarNodos(nodoActual,n);
		u = nodoActual[0]->getFrecuencia() + nodoActual[1]->getFrecuencia();
		str = nodoActual[0]->getNumero() + nodoActual[1]->getNumero();
		ptr = crearNodo(str, u);

		ptr->setHijoDerecho(nodoActual[1]);
		ptr->setHijoIzquierdo(nodoActual[0]);

		nodoActual[0] = ptr;

		yaEncontrePosicion = false;
		posicionInsercion = n - 2;
		frecuencia = nodoActual[0]->getFrecuencia();
		for (int i = 1; i < n - 1; i++) {
			if (!yaEncontrePosicion && nodoActual[i]->getFrecuencia()
					> frecuencia) {
				posicionInsercion = i - 2;
				yaEncontrePosicion = true;
			}
			nodoActual[i] = nodoActual[i + 1];
		}

		for (int j = 0; j < posicionInsercion; j++) {
			nodoActual[j] = nodoActual[j + 1];
		}

		nodoActual[posicionInsercion] = ptr;
	}
}

void Huffman::calcularCodigo(Nodo* arbol, bool c[], int n, int codigoChar) {

	if ((arbol->getHijoIzquierdo() == NULL)
			&& (arbol->getHijoDerecho() == NULL)) {
		if (arbol->getNumero() == codigoChar) {
			emitirCodigo(c, n);

		}
	} else {
		c[n] = 0; // Asigno un 0 si va a la izquierda
		n++;
		calcularCodigo(arbol->getHijoIzquierdo(), c, n, codigoChar); // Vuelve a llamar al hijo izquierdo
		c[n - 1] = 1; // Asigno un 1 si va a la derecha
		calcularCodigo(arbol->getHijoDerecho(), c, n, codigoChar); // Lo vuelve a llamar al hijo derecho
	}
}

void Huffman::emitirCodigo(bool codigo[], int longitudCodigo) {
	int bytesAGuardar, bitsADesplazar;
	uint32_t aGuardar;

	for (int i = 0; i < longitudCodigo; i++) {
		this->buffer.buffer <<= 1;
		// Si es un uno, cambio el bit (incialmente esta todo en 0);
		if (codigo[i]) {
			this->buffer.buffer |= 1;
		}
		this->buffer.bitsOcupados++;
		this->sz++;
	}

	bytesAGuardar = this->buffer.bitsOcupados / 8;

	if (bytesAGuardar > 0) {
		bitsADesplazar = 8 * sizeof(this->buffer.buffer)
				- this->buffer.bitsOcupados;
		this->buffer.buffer <<= bitsADesplazar;

		aGuardar = htonl(this->buffer.buffer);
		fwrite((char*) (&aGuardar), 1, bytesAGuardar, this->archivoSalida);
		this->buffer.bitsOcupados -= bytesAGuardar * 8;
		this->buffer.buffer >>= bitsADesplazar;
	}

}

void Huffman::comprimirLongitudyPosicion(int codigo, Nodo* nodoActual[],
		bool esLongitud) {

	if (esLongitud) {
		crearArbol(nodoActual, MAX);
		codigo = pasarLongitudaNumero(codigo);
	}
	calcularCodigo(nodoActual[0], c, 0, codigo);
	if (esLongitud) {
		for (int i = 0; i < MAX; i++) {
			nodoActual[i] = inicialCompleto[i];
			if (nodoActual[i]->getNumero() == codigo)
				nodoActual[i]->setFrecuencia(nodoActual[i]->getFrecuencia() + 1); // Incremento la frecuencia del caracter codificado.
		}
	}
}

void Huffman::comprimirCaracter(int codigo) {

	crearArbol(a, MAX);
	calcularCodigo(a[0], c, 0, codigo); //CHEQUEAR LA C 	// a[0] es la raiz
	for (int i = 0; i < MAX; i++) {
		a[i] = inicialCompleto[i];
		if (a[i]->getNumero() == codigo)
			a[i]->setFrecuencia(a[i]->getFrecuencia() + 1); // Incremento la frecuencia del caracter codificado.
	}
}

void Huffman::descomprimirCodigo(int cod) {
	int numeroCaracter = NULL;

	if (buscarPosicion) {
		if (caracterNuevo) {
			this->caracterNuevo = false;
			calcularCaracter(fijo[0], cod, &numeroCaracter); //Busca en el arbolPosicion
		} else {
			calcularCaracter(this->nodoDescompresor, cod, &numeroCaracter);
		}

	} else {
		if (caracterNuevo) {
			this->caracterNuevo = false;
			crearArbol(a, MAX);
			calcularCaracter(a[0], cod, &numeroCaracter); //Busca en el arbolCaracter
		} else {
			calcularCaracter(this->nodoDescompresor, cod, &numeroCaracter);
		}

		if (this->termino) {
			for (int i = 0; i < MAX; i++) {
				this->a[i] = this->inicialCompleto[i];
				if (a[i]->getNumero() == numeroCaracter)
					a[i]->setFrecuencia(a[i]->getFrecuencia() + 1); // Incremento la frecuencia del caracter codificado.
			}
			this->termino = false;
		}
	}
}

void Huffman::calcularCaracter(Nodo* arbol, int cod, int* numeroCaracter) {
	//char strint;
	Nodo* actual;
	bool codigoAimprimir[30];
	int x;

	if (cod == 0) { // Me muevo a la izquierda, sino a la derecha.
		actual = arbol->getHijoIzquierdo();
		codigo->push(0);
	} else {
		actual = arbol->getHijoDerecho();
		codigo->push(1);
	}

	if ((actual->getHijoIzquierdo() == NULL) && (actual->getHijoDerecho()
			== NULL)) { //Se fija si es hoja

		*numeroCaracter = actual->getNumero();
		if (!buscarPosicion) {
			//printf("%d\n", actual->getNumero());
			if (actual->getNumero() > 255) {
				buscarPosicion = true;
				emitirLongitudDescomprimido(actual->getNumero() - 255);
				this->termino = true;
				//printf("%d\n", actual->getNumero());

			} else {
				//strint = (char)actual->getNumero();
				emitirCaracterDescomprimido(actual->getNumero());
				//cout<<strint<<endl;
				this->termino = true;

			}
		} else {
			buscarPosicion = false;
			//cout<<actual->getNumero()<<endl;
			emitirPosicionDescomprimido(actual->getNumero());
		}
		this->caracterNuevo = true;
	} else
		nodoDescompresor = actual;
}

void Huffman::emitirLongitudDescomprimido(int numeroLongitud) {
	bool codigo[5];
	//printf("%d", numeroLongitud);
	// Como es una longitud, el discriminador para lz77 es 1. Despues se tiene que
	// descomprimir la posicion, sin colocar nada delante.
	codigo[0] = 1;

	// Voy tomando los bits de la longitud y los almaceno en el codigo a guardar
	numeroLongitud <<= 28;
	// La longitud ocupa 4 bits a lo sumo.
	for (int i = 0; i <= 3; i++) {
		if (numeroLongitud & (unsigned int) pow(2, (sizeof(numeroLongitud) * 8)
				- 1)) {

			codigo[i + 1] = 1;
		} else {

			codigo[i + 1] = 0;
		}
		numeroLongitud <<= 1;
	}

	emitirCodigo(codigo, 5);
}

void Huffman::emitirPosicionDescomprimido(int numeroPosicion) {
	bool codigo[13];
	// El discriminador para lz77 ya se emitio en la longitud, asi que aca no agrego nada.

	// Voy tomando los bits de la posicion y los almaceno en el codigo a guardar
	// Muevo los datos importantes al principio del entero de 4 bytes
	numeroPosicion <<= 19;
	// La longitud ocupa 14 bits a lo sumo.
	for (int i = 0; i <= 12; i++) {
		if (numeroPosicion & (unsigned int) pow(2, (sizeof(numeroPosicion) * 8)
				- 1)) {
			codigo[i] = 1;
		} else {
			codigo[i] = 0;
		}
		numeroPosicion <<= 1;
	}
	emitirCodigo(codigo, 13);
}

void Huffman::emitirCaracterDescomprimido(int numeroCaracter) {
	bool codigo[9];
	// Como es un caracter, el discriminador para lz77 es 0
	codigo[0] = 0;

	// Voy tomando los bits del caracter literal y los almaceno en el codigo a guardar
	numeroCaracter <<= 24;
	for (int i = 0; i <= 7; i++) {
		if (numeroCaracter & (unsigned int) pow(2, (sizeof(numeroCaracter) * 8)
				- 1)) {
			codigo[i + 1] = 1;
		} else {
			codigo[i + 1] = 0;
		}
		numeroCaracter <<= 1;
	}
	emitirCodigo(codigo, 9);
}

void Huffman::emitirRelleno() {
	int diff;

	if ((sz % 8) == 0) { // Si el codigo me quedo multiplo de 8 lo escribo directamente.
		bool relleno[8];
		relleno[0] = 1;
		for (int i = 1; i < 7; i++) { // Agrego 1 byte asi desp para la descompresion solo elimino hasta el primer 1
			relleno[i] = 0; //de atras para adelante.
		}
		emitirCodigo(relleno, 8);

	} else {
		diff = 8 - (sz % 8);
		bool relleno[diff]; // Calculo el numero de ceros a colocar
		relleno[0] = 1;
		for (int i = 1; i < diff; i++) {
			relleno[i] = 0;// Pone la cantidad de ceros necesitarios para que quede de 8 bits
		}
		emitirCodigo(relleno, diff);

	}
}

void Huffman::armarArbolPosicion() {

	for (int ref = 0; ref < MAXFIJO; ref++) {
		fijo[ref] = crearNodo(ref, ref);
	}
	crearArbol(fijo, MAXFIJO);
}

int Huffman::pasarLongitudaNumero(int longitud) {
	return 255 + longitud;

}

void Huffman::setArchivoSalida(FILE* archivoSalida) {
	this->archivoSalida = archivoSalida;
}

