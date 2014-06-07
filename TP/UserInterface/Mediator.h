#ifndef MEDIATOR_H_
#define MEDIATOR_H_

#include "../common/Word.h"
#include "../common/DocumentGlobalWeight.h"
#include "../parser/FileParser.h"
#include "../Business/DataDocumentManager.h"
#include "../Business/IndexWrapper.h"
//#include "../btree/OcurrenceTree.h"
#include <set>
#include <list>
#include <map>
#include "../common/ListOfPositions.h"
#include "../common/RankingDocument.h"

class Mediator {

private:

	FileParser* fileParser;
	DataDocumentManager* dataDocumentManager;
	IndexWrapper* indexWrapper;
	BPlusTree* infinityNormIndex;
	BPlusTree* vocabularyIndex;
	ClassifBPlusTree* ocurrenceTree;
	fstream fileAutoIncInteger;
	int autoIncInteger;


	string codeTittle (string clave);
	string searchInternalID(string identificator);

public:

	static enum fileItem {NORMA_INFINITO = 0, TERMINOS = 1, OCURRENCIA_POSICIONAL = 2, LISTAS = 3} tFileItem;
	Mediator();
	~Mediator();
	Document* getDocument(string pathFile);
	bool addDocument(Document* document);
	bool removeDocument(int ID);
	string getListOfTakenDocuments();
	IndexWrapper* getIndexWrapper();
	bool indexingListOnIndex(IndexWrapper::indexItem);
	void initializeIndex(IndexWrapper::indexItem item, string filepath,int maxSize);
	void closeIndex(IndexWrapper::indexItem item);
	ByteString getDocumentFromFile(int id);
	void exportToPlainText(IndexWrapper::indexItem index, string toFile);
	void addtitulo(string titulo);
	string search(IndexWrapper::indexItem indexId, string toSearch);
	list<int>* findCoincidences(list<Word*> words);
	list<int>* findNotCoincidences(list<Word*> words, list<int>* coincidenceDocs);
	list<RankingDocument>* findTermProxIdDocuments(list<int>* idDocument,list<Word*> idTerms);
	list<DocumentGlobalWeight*>* orderDocumentByGlobalWeight(list<int>* idDocument, list<Word*> words, int totalDocument);
	double calculateDocumentGlobalWeight(int idDocument, int infinityNorm, int totalDocument, int numberDocsWithWord, int repetition);
	int getInfinityNorm(int idDocument);
	list<int>* getListOfPositions(int idDocument, int idTerm);
	void exportToTXT(fileItem item,string filepath);
	void exportInvertedList(string pathname);
};

#endif /* MEDIATOR_H_ */
