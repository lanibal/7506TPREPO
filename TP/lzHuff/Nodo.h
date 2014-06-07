#ifndef NODO_H_INCLUDED
#define NODO_H_INCLUDED
#include<iostream>

//const int MAXIMO=268;

class Nodo {

private:
    int frecuencia;
    Nodo* hijoDerecho;
    Nodo* hijoIzquierdo;
    int numeroCaracter;


public:

    Nodo ();
    virtual ~Nodo();
    void setFrecuencia(int f);
    int getFrecuencia() const;
    int getNumero();
    void setHijoDerecho(Nodo* hijoDerecho);
    void setHijoIzquierdo(Nodo* hijoIzquierdo);
    void setNumero(int numero);
    Nodo* getHijoDerecho() const;
    Nodo* getHijoIzquierdo() const;
    Nodo* getPadre() const;
    bool tieneHijoDerecho() const;
    bool tieneHijoIzquierdo() const;
    bool tienePadre() const;
    void aumentarFrecuencia();

};

#endif // NODO_H_INCLUDED
