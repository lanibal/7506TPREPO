#include "TreeConstraits.h"

TreeConstraits::TreeConstraits() {

}

TreeConstraits::~TreeConstraits() {
}

int TreeConstraits::getFullSizeNode(){
	return Resource::getConfigProperties()->getAsInt("miBiografia.arbol.nodo.tamanio");
}

int TreeConstraits::getControlSizeNode(){
	return Resource::getConfigProperties()->getAsInt("miBiografia.arbol.nodo.control");
}

int TreeConstraits::getEfectiveSizeNode(){
	return Resource::getConfigProperties()->getAsInt("miBiografia.arbol.nodo.tamanio") -
			Resource::getConfigProperties()->getAsInt("miBiografia.arbol.nodo.control");
}

int TreeConstraits::getControlSizeRecord(){
	return Resource::getConfigProperties()->getAsInt("miBiografia.arbol.registro.control");
}

int TreeConstraits::getPercentRecord(){
	return Resource::getConfigProperties()->getAsInt("miBiografia.arbol.registro.porcentaje");
}

