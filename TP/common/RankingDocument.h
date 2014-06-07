#ifndef RANKINGDOCUMENT_H_
#define RANKINGDOCUMENT_H_

class RankingDocument {
private:
	int idDocument;
	int distanceTerms;
public:
	RankingDocument();
	virtual ~RankingDocument();
    int getDistanceTerms() const;
    int getIdDocument() const;
    void setDistanceTerms(int distanceTerms);
   void setIdDocument(int idDocument);
};

#endif /* RANKINGDOCUMENT_H_ */
