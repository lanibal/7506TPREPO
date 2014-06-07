#include "NodoDeCola.h"

#ifndef NULL
#define NULL 0
#endif


NodoDeCola::NodoDeCola(){
	dato = NULL;
    siguiente = NULL;
}


NodoDeCola::~NodoDeCola(){
	dato=NULL;
	siguiente=NULL;
}


void NodoDeCola::setDato(int dato){
    this->dato = dato;
}


int NodoDeCola::getDato(){
    return this->dato;
}


NodoDeCola* NodoDeCola::getSiguiente(){
    return this->siguiente;
}


void NodoDeCola::setSiguiente(NodoDeCola* siguiente){
    this->siguiente = siguiente;
}


bool NodoDeCola::tieneSiguiente(){
   if (this->siguiente != NULL)
	   return true;
   else
	   return false;
}
