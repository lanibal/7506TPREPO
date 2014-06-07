#ifndef DOCUMENTGLOBALWEIGHT_H_
#define DOCUMENTGLOBALWEIGHT_H_

class DocumentGlobalWeight {
private:
	int idDocument;
	double globalWeight;
public:
	DocumentGlobalWeight();
	DocumentGlobalWeight(int idDocument, double globalWeight);
	virtual ~DocumentGlobalWeight();

	int getIdDocument();
	double getGlobalWeight();
};

#endif /* DOCUMENTGLOBALWEIGHT_H_ */
