#include "Cola.h"

Cola::Cola() {
    frente = NULL;
    ultimo = NULL;
    cantidadUno = 0;
}


/*Cola::~Cola() {
    while (!estaVacia()) {
        pop();
    }
}*/

void Cola::push(int  dato) {
	NodoDeCola* aux = new NodoDeCola();
    aux->setDato(dato);
    if (frente == NULL) {
    	frente = aux;
    	ultimo = aux;
    }
    else {

    	ultimo->setSiguiente(aux);
    	ultimo =aux;
    }
}

void Cola::pop(int& x) {

    NodoDeCola* aux = frente;
    x = aux->getDato();
    frente = frente->getSiguiente();
    if (frente == NULL)
        	ultimo = NULL;
    aux->setSiguiente(NULL);
    delete aux;
}

void Cola::popUltimo(){

	NodoDeCola*aux = frente;
	while (aux->tieneSiguiente()){
		if (aux->getSiguiente() == ultimo){

			if(this->cantidadUno != 1){
				if(aux->getSiguiente()->getDato() == 1){
					this->cantidadUno = 1; //  Hago esto para que solo borre el primer uno po
				}							// sino tiene que borrar todos el byte.
				ultimo = aux;
				aux->setSiguiente(NULL);
			}
		}

		if (aux->getSiguiente()!= NULL)
				aux = aux->getSiguiente();
	}

}


/*Nodo* Cola::top() {
    if (frente == NULL) {
        return NULL;
    }
    return frente->getDato();
}*/

bool Cola::estaVacia() {
    return (frente == NULL);
}
