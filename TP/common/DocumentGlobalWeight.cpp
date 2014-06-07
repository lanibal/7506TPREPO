#include "DocumentGlobalWeight.h"

DocumentGlobalWeight::DocumentGlobalWeight() {
}

DocumentGlobalWeight::DocumentGlobalWeight(int idDocument, double globalWeight) {
	this->idDocument = idDocument;
	this->globalWeight = globalWeight;
}

DocumentGlobalWeight::~DocumentGlobalWeight() {
	// TODO Auto-generated destructor stub
}

int DocumentGlobalWeight::getIdDocument()
{
	return this->idDocument;
}

double DocumentGlobalWeight::getGlobalWeight()
{
	return this->globalWeight;
}

