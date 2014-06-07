#include "Nodo.h"

#ifndef NODOSDECOLA_H_
#define NODODECOLA_H_

class NodoDeCola {

private:
    //Dato almacenado
    int dato;
    //Puntero al siguiente nodo
    NodoDeCola* siguiente;
    
public:
    
    NodoDeCola();
    NodoDeCola(int dato);
    virtual ~NodoDeCola();
    void setDato(int dato);
    int getDato();
    void setSiguiente(NodoDeCola* siguiente);
    NodoDeCola* getSiguiente();
    bool tieneSiguiente();

};

#endif
