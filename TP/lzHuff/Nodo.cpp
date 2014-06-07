#include "Nodo.h"

 Nodo::Nodo() {
	 this->setFrecuencia(NULL);
     this->setHijoDerecho(NULL);
     this->setHijoIzquierdo(NULL);
     this->setNumero(NULL);

}

 Nodo::~Nodo (){
	this->setHijoDerecho(NULL);
	this->setHijoIzquierdo(NULL);

 }

 void Nodo::setFrecuencia(int f){
     this->frecuencia=f;
 }
 

 int Nodo::getFrecuencia () const{
     return this->frecuencia;
 }
 

 void Nodo::setHijoDerecho (Nodo* hijoDerecho){
     this->hijoDerecho=hijoDerecho;
 }
 

 void Nodo::setHijoIzquierdo (Nodo* hijoIzquierdo){
     this->hijoIzquierdo=hijoIzquierdo;
 }
 

 Nodo* Nodo::getHijoDerecho () const{
     return this->hijoDerecho;
 }
 

 Nodo* Nodo::getHijoIzquierdo() const{
     return this->hijoIzquierdo;
 }
 



 bool Nodo::tieneHijoDerecho() const{
     return (this->hijoDerecho!=NULL);
 }


 bool Nodo::tieneHijoIzquierdo() const{
     return (this->hijoIzquierdo!=NULL);
 }
 


 void Nodo::aumentarFrecuencia(){
	 (this->frecuencia)++;
 }

 void Nodo::setNumero(int numeroCaracter){
	 this->numeroCaracter = numeroCaracter;

 }

 int Nodo::getNumero(){
	 return this->numeroCaracter;
 }

