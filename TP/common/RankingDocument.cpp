#include "RankingDocument.h"

RankingDocument::RankingDocument() {
	// TODO Auto-generated constructor stub

}

int RankingDocument::getDistanceTerms() const
{
    return distanceTerms;
}

int RankingDocument::getIdDocument() const
{
    return idDocument;
}

void RankingDocument::setDistanceTerms(int distanceTerms)
{
    this->distanceTerms = distanceTerms;
}

void RankingDocument::setIdDocument(int idDocument)
{
    this->idDocument = idDocument;
}

RankingDocument::~RankingDocument() {
	// TODO Auto-generated destructor stub
}
