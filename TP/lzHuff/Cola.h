#ifndef COLA_H_
#define COLA_H_

#include "NodoDeCola.h"


class Cola {
private:
	NodoDeCola* frente;
	NodoDeCola* ultimo;
    
public:
	int cantidadUno;
    Cola();
    ~Cola();
    void push(int dato);
    void pop(int& x);
    Nodo* top();
    bool estaVacia();
    void popUltimo();
};

#endif
