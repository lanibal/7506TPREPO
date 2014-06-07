#include "Resource.h"

// Inicializacion del miembro estatico
Properties Resource::configProperties("miBiografia.config");

Resource::Resource() {
}

Resource::~Resource() {
}

Properties* Resource::getConfigProperties(){
	Resource::configProperties.setFileName("miBiografia.config");
	return &configProperties;
}

