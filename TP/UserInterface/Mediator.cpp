#include "Mediator.h"
#include "../common/List.h"
#include "../common/ListIterator.h"
#include "unistd.h"
#include <math.h>
#include <list>
using namespace std;

Mediator::Mediator()
{
	//FIXME
	this->dataDocumentManager = new DataDocumentManager("ArchivoDeLibros");
	this->fileParser = new FileParser();
	this->indexWrapper = IndexWrapper::getInstanceOfIndexWrapper();

	// ---------------------------------------------------------------------------------------------------------------------------- //
	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.index");

	this->vocabularyIndex = new BPlusTree(indexPath + "indexVocabulario", 50);
	this->infinityNormIndex = new BPlusTree(indexPath + "indexNormaInfinito", 50);
	// ---------------------------------------------------------------------------------------------------------------------------- //

	// ---------------------------------------------------------------------------------------------------------------------------- //
	//string ocurrencePath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control") + "ocurrencias";
	//this->ocurrenceTree = new OcurrenceTree(ocurrencePath, 50);
	string ocurrencePath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.index");
	this->ocurrenceTree = new ClassifBPlusTree(ocurrencePath  + "indexOcurrencias", 50);
	// ---------------------------------------------------------------------------------------------------------------------------- //

	// ---------------------------------------------------------------------------------------------------------------------------- //
	string incrementPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control") + "IntAutoIncVocabulario.inc";
	this->fileAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
	bool open = this->fileAutoIncInteger.is_open();
	if(!open)
	{
		fopen(incrementPath.c_str(),"w+");
		this->fileAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
		this->fileAutoIncInteger.seekp(0);
		int cero = 0;
		this->fileAutoIncInteger.write((char*)&cero,sizeof(int));
	}

	this->fileAutoIncInteger.seekg(0);
	int incInteger;
	this->fileAutoIncInteger.read((char*)&incInteger,sizeof(int));
	this->autoIncInteger = incInteger;
	// ---------------------------------------------------------------------------------------------------------------------------- //
}

Mediator::~Mediator()
{
	delete this->dataDocumentManager;
	delete this->fileParser;
	// --------------------------------------------------------- //
	delete this->vocabularyIndex;
	delete this->infinityNormIndex;
	// --------------------------------------------------------- //
	delete this->ocurrenceTree;
	// --------------------------------------------------------- //
	this->fileAutoIncInteger.seekp(0);
	int lastId = this->autoIncInteger;
	this->fileAutoIncInteger.write((char*)&lastId,sizeof(int));
	this->fileAutoIncInteger.close();
	// --------------------------------------------------------- //
	//delete this->indexWrapper;
}

Document* Mediator::getDocument(string pathFile)
{
	bool done = this->fileParser->parse(pathFile);
	Document* document = NULL;
	if(done)
	{
		document = this->fileParser->getDocument();
	}
	return document;
}


bool Mediator::addDocument(Document *document)
{
	bool done = false;
	if(document != NULL)
	{
		int ID = this->dataDocumentManager->addDocument(document->Serialize());
		if(ID>0)
			done = true;
	}
	return done;
}

bool Mediator::removeDocument(int ID)
{
	bool done = false;
	if(ID>0)
	{
		ByteString byteString = this->dataDocumentManager->getDocument(ID);
		if(!byteString.isEmpty())
		{
			bool isRemoved = this->dataDocumentManager->removeRecord(ID);
			if(isRemoved)
			{
				Document document;
				document.Hidratate(byteString);
				this->indexWrapper->removeFromAll(document);
				done = true;
			}
		}
	}
	return done;
}

string Mediator::getListOfTakenDocuments()
{
	string listOfTakenDocuments = this->dataDocumentManager->getlistOfTakenDocuments();
	return listOfTakenDocuments;
}

IndexWrapper* Mediator::getIndexWrapper()
{
	return this->indexWrapper;
}



void Mediator::initializeIndex(IndexWrapper::indexItem item, string filepath,int maxSize)
{
	//cout << "Start Index Initilizer: " << Utility::getDate();
	this->indexWrapper->createIndex(filepath,item,maxSize);
	//cout << "End Index Initilizer: " << Utility::getDate() << endl;
}

void Mediator::closeIndex(IndexWrapper::indexItem item)
{
	//cout << "Start Index Closing: " << Utility::getDate();
	this->indexWrapper->closeIndex(item);
	//cout << "End Index Closing: " << Utility::getDate() << endl;
}

bool Mediator::indexingListOnIndex(IndexWrapper::indexItem item)
{
	bool done = false;
	List<int>* flagsLs = this->dataDocumentManager->getListToFileFlag(item);
	if(!flagsLs->isEmpty())
	{
		cout << "Process Started: " << Utility::getDate() << endl;
		ListIterator<int> it = flagsLs->getIterator();
		int ID;
		while(it.hasNext())
		{
			ID = it.next();
			ByteString bs = this->dataDocumentManager->getDocument(ID);
			Document document;
			document.Hidratate(bs);

			string clave;
			Record* record;
			ByteString bsIdDocument;
			ByteString* bsId;
			bsIdDocument.insertLast(&ID,sizeof(int));
			Key* key;
			switch(item){
			case IndexWrapper::AUTOR :
										clave = document.getAuthor();
										cout << "Document begin process: " << document.getTitle() << " - Author:" << clave << " - Start:" << Utility::getDate();
										key = new Key(clave);
										bsId = new ByteString(bsIdDocument);
										record = new Record(key,bsId);
										done = this->indexWrapper->add(record,item);
										break;
			case IndexWrapper::TITULO :
										clave = document.gettitulo();
										clave = codeTittle(clave);
										cout << "Document begin process: " << document.getTitle() << " - titulo:" << clave << " - Start:" << Utility::getDate();

										key = new Key(clave);
										bsId = new ByteString(bsIdDocument);
										record = new Record(key,bsId);
										done = this->indexWrapper->add(record,item);
										break;
			case IndexWrapper::IDENTIFICADOR :
										clave = document.getTitle();
										cout << "Document begin process: " << document.getTitle() << " - Start:" << Utility::getDate();

										key = new Key(clave);
										bsId = new ByteString(bsIdDocument);
										record = new Record(key,bsId);
										done = this->indexWrapper->add(record,item);
										break;

			case IndexWrapper::FECHA:
														clave = Utility::intToString(document.getWordCount());
														cout << "Document begin process: " << document.getTitle() << " - Fecha:" << clave << " - Start:" << Utility::getDate()<<endl;

														key = new Key(clave);
														bsId = new ByteString(bsIdDocument);
														record = new Record(key,bsId);
														done = this->indexWrapper->add(record,item);
														break;




			};
			cout << "Document processed: " << document.getTitle() << " - Finish:" << Utility::getDate() << endl;
		}
		cout << "Process Ended: " << Utility::getDate();
	}

	//delete poddo
	if (flagsLs != NULL)
		delete flagsLs;

	return done;
}

ByteString Mediator::getDocumentFromFile(int id)
{
	ByteString bs;
	bs = this->dataDocumentManager->getDocument(id);
	return bs;
}

void Mediator::exportToPlainText(IndexWrapper::indexItem index, string toFile){
	IndexWrapper::getInstanceOfIndexWrapper()->exportIndex(toFile, index);
}

void Mediator::addtitulo(string titulo)
{
	this->fileParser->addtitulo(titulo);
}


/**************************QUERYS*****************************/

string Mediator::search(IndexWrapper::indexItem index, string toSearch)
{
	Key* k = NULL;
	string toReturn;
	list<int>* documentIds;
	//string separator;
	//list<string> wordsToFind;
	FileParser* fp;
	map<string, Term*> wordsToFind;
	list<Word*> words;
	list<int>* coincidenceDocs = NULL;
	list<int>* notCoincidenceDocs = NULL;
	list<DocumentGlobalWeight*>* documentByGlobalWeight = NULL;
	bool doSearch = false;
	int totalDocuments;

	switch(index){
		case IndexWrapper::TITULO:
			k = new Key(codeTittle(toSearch));
			documentIds = this->indexWrapper->searchAllIds(k, index);
			break;
		case IndexWrapper::AUTOR:
			k = new Key(toSearch);
			documentIds = this->indexWrapper->searchAllIds(k, index);
			break;
		case IndexWrapper::IDENTIFICADOR:
			k = new Key(toSearch);
			documentIds = this->indexWrapper->searchAllIds(k, index);
			break;
		case IndexWrapper::FECHA:
			k = new Key(toSearch);
			documentIds = this->indexWrapper->searchAllIds(k, index);
			break;

		case IndexWrapper::PALABRAS:
		{
			documentIds = new list<int>();

			//separator = " ";
			//wordsToFind = Utility::split(toSearch, separator);

			fp = new FileParser();
			fp->setWords(toSearch);
			wordsToFind = fp->getTerms();
			delete fp;

			// Se recorre todas las palabras de las busqueda, y se buscan en el hash
			for(map<string, Term*>::iterator it = wordsToFind.begin(); it != wordsToFind.end(); ++it)
			{
				Key* k = new Key((*it).first);
				list<int>* documentsWords = this->indexWrapper->searchAllIds(k, index);
				Word* newWord = new Word((*it).first, documentsWords);
				words.push_back(newWord);
				if (newWord->getDocuments()->size() > 0)
					doSearch = true;
				delete k;
			}

			if (wordsToFind.size()>0 && doSearch)
			{
				//Obtiene los documentos en que se encuentran en todas las palabras
				coincidenceDocs = this->findCoincidences(words);
				if (coincidenceDocs->size() > 0)
				{
					list<RankingDocument>* listRanking;
					listRanking = findTermProxIdDocuments(coincidenceDocs,words);
					list<RankingDocument>::iterator itRanking;

					cout << "Ids document Encontrados por Terminos Proximos: ";
					for(itRanking=listRanking->begin();itRanking!=listRanking->end();++itRanking)
					{
						int idDocument = (*itRanking).getIdDocument();
						cout << idDocument << " ";
						documentIds->push_back(idDocument);
					}
					cout << endl;
				}

				//Obtiene los documentos que se encuentran solo algunas de las palabras
				notCoincidenceDocs = this->findNotCoincidences(words, coincidenceDocs);
				if (notCoincidenceDocs->size() > 0)
				{
					//Ordena los documentos que no tienen coincidencia total en los terminos segun el peso global
					totalDocuments = coincidenceDocs->size() + notCoincidenceDocs->size();
					documentByGlobalWeight = this->orderDocumentByGlobalWeight(notCoincidenceDocs, words, totalDocuments);

					cout << "Ids document Encontrados por Peso Global: ";
					for(list<DocumentGlobalWeight*>::iterator itGW = documentByGlobalWeight->begin(); itGW!=documentByGlobalWeight->end(); ++itGW)
					{
						int idDocument = (*itGW)->getIdDocument();
						cout << idDocument << " ";
						documentIds->push_back(idDocument);
					}
					cout << endl;
				}
			}

			break;
		}
		default:
			break;
	}

	if(documentIds->size() > 0){
		for (list<int>::iterator it = documentIds->begin(); it != documentIds->end(); ++it)
		{
			ByteString bs = this->dataDocumentManager->getDocument(*it);
			Document b;
			b.Hidratate(bs);
			toReturn.append(b.getDocumentFormatedText());
		}
	}else
		toReturn = "La busqueda no arrojo resultados";


	delete k;
	return toReturn;
}

list<int>* Mediator::findCoincidences(list<Word*> words)
{
	bool foundIt;
	bool foundItInAll;
	list<int>* coincidenceDocs = new list<int>();

	//si hay mas de una palabra para buscar, si hay solo una se compara por peso global
	if (words.size() > 1)
	{
		//Comparamos la lista de documentos de cada palabra
		Word* firstWord = words.front();
		for (list<int>::iterator it = firstWord->getDocuments()->begin(); it != firstWord->getDocuments()->end(); ++it)
		{
			int idDocument = *it;
			foundItInAll = true;

			//Compara contra el resto de palabras
			for (list<Word*>::iterator it2 = words.begin(); it2 != words.end(); ++it2)
			{
				foundIt = false;
				for (list<int>::iterator it3 = (*it2)->getDocuments()->begin(); it3 != (*it2)->getDocuments()->end(); ++it3)
				{
					int idDocument2 = *it3;

					if (idDocument == idDocument2)
						foundIt = true;
				}

				if (!foundIt)
					foundItInAll = false;

			}

			if (foundItInAll)
				coincidenceDocs->push_back(idDocument);

		}
	}

	return coincidenceDocs;
}

list<int>* Mediator::findNotCoincidences(list<Word*> words, list<int>* coincidenceDocs)
{
	bool foundIt;
	bool exists;
	list<int>* notCoincidenceDocs = new list<int>();

	//Compara contra cada una de las palabras
	for (list<Word*>::iterator it = words.begin(); it != words.end(); ++it)
	{
		//compara con la lista de documentos de cada una de la palabras
		for (list<int>::iterator it1 = (*it)->getDocuments()->begin(); it1 != (*it)->getDocuments()->end(); ++it1)
		{
			int idDocument = *it1;

			//verifica que no se encuentre enla lista de total coincidencia
			foundIt = false;
			for (list<int>::iterator it2 = coincidenceDocs->begin(); it2 != coincidenceDocs->end(); ++it2)
			{
				int idDocument2 = *it2;
				if (idDocument == idDocument2)
					foundIt = true;
			}

			if (!foundIt)
			{
				//verifica que no se encuentre en la lista de no coincidencias
				exists = false;
				for (list<int>::iterator it3 = notCoincidenceDocs->begin(); it3 != notCoincidenceDocs->end(); ++it3)
				{
					int idDocument3 = *it3;
					if (idDocument == idDocument3)
						exists = true;
				}

				if (!exists)
					notCoincidenceDocs->push_back(idDocument);
			}
		}

	}

	return notCoincidenceDocs;
}

list<RankingDocument>* Mediator::findTermProxIdDocuments(list<int>* idDocuments,list<Word*> idTerms)
{
	// si llega aca, estamos seguros que hay al menos 2 terminos a buscar
	list<int>::iterator itIdDocuments;
	list<Word*>::iterator itIdTerms;
	list<ListOfPositions> listOfListOfPositions;
	list<RankingDocument>* listRankingDocument = new list<RankingDocument>;
	for(itIdDocuments=idDocuments->begin();itIdDocuments!=idDocuments->end();++itIdDocuments)
	{
		int idB = *itIdDocuments;
		for(itIdTerms=idTerms.begin();itIdTerms!=idTerms.end();++itIdTerms)
		{
			int idDocument = *itIdDocuments;
			int idTerm = (*itIdTerms)->getIdWord();

			list<int>* listOfPos = getListOfPositions(idDocument,idTerm);
			ListOfPositions listOfPositions(listOfPos,idTerm);
			listOfListOfPositions.push_back(listOfPositions);
		}
		list<ListOfPositions>::iterator itLOfPos;
		int minimTotal = 0;

		itLOfPos = listOfListOfPositions.begin();
		ListOfPositions previous = (*itLOfPos);
		itLOfPos++;
		ListOfPositions current = (*itLOfPos);

		for( ; itLOfPos != listOfListOfPositions.end(); ++itLOfPos)
		{
			current = *itLOfPos;
			minimTotal += current.minDistance(previous.getListOfPositions());
			previous = current;
		}
		RankingDocument ranking;
		ranking.setIdDocument(idB);
		ranking.setDistanceTerms(minimTotal);
		bool insertado = false;
		if (listRankingDocument->size() > 0)
		{
			for (list<RankingDocument>::iterator it = listRankingDocument->begin(); it != listRankingDocument->end(); ++it)
			{
				if(!insertado)
				{
					if ((*it).getDistanceTerms() <= ranking.getDistanceTerms())
					{
						--it;
						listRankingDocument->insert(it, ranking);
						insertado = true;
					}
				}
			}
			if(!insertado)
				listRankingDocument->push_back(ranking);
		}
		else
			listRankingDocument->push_back(ranking);

		listOfListOfPositions.clear();
	}
	return listRankingDocument;
}

list<DocumentGlobalWeight*>* Mediator::orderDocumentByGlobalWeight(list<int>* idDocuments, list<Word*> words, int totalDocuments)
{
	list<DocumentGlobalWeight*>* orderDocs = new list<DocumentGlobalWeight*>();
	list<int>* listOfPos;
	bool insertado =  false;
	int actualIdDocument;
	int actualIdTerm;
	int infinityNorm;
	int numberDocsWithWord;
	int repetition;
	Word* word;
	double actualDocumentGlobalWeight;
	DocumentGlobalWeight* documentGlobalWeight;

	//recorre cada uno de los documentos para calcular su peso global
	for (list<int>::iterator it = idDocuments->begin(); it != idDocuments->end(); ++it)
	{
		actualIdDocument = *it;
		actualDocumentGlobalWeight = 0;

		//obtiene la norma infinito para el documento actual
		infinityNorm = this->getInfinityNorm(actualIdDocument);

		//recorre todas las palabras para ver si estan en el documento actual
		for (list<Word*>::iterator it1 = words.begin(); it1 != words.end(); ++it1)
		{
			word = *it1;
			actualIdTerm = word->getIdWord();

			//obtiene la cantidad de documentos donde aparece la palabra
			numberDocsWithWord = word->getDocuments()->size();

			//recorre cada uno de los documentos de la palabra para ver si existe el documento actual
			for (list<int>::iterator it2 = word->getDocuments()->begin(); it2 != word->getDocuments()->end(); ++it2)
			{
				if (actualIdDocument == *it2)
				{
					listOfPos = this->getListOfPositions(actualIdDocument, actualIdTerm);
					repetition = listOfPos->size();
					delete listOfPos;

					//calcula el peso total del termino en el documento
					actualDocumentGlobalWeight += this->calculateDocumentGlobalWeight(actualIdDocument, infinityNorm, totalDocuments, numberDocsWithWord, repetition);
				}
			}
		}

		//hay que guardar ordenandolo por peso global
		insertado = false;
		documentGlobalWeight = new DocumentGlobalWeight(actualIdDocument, actualDocumentGlobalWeight);
		//recorre y inserta ordernado
		if (orderDocs->size() > 0)
		{
			for (list<DocumentGlobalWeight*>::iterator it3 = orderDocs->begin(); it3 != orderDocs->end(); ++it3)
			{
				if(!insertado)
				{
					if ((*it3)->getGlobalWeight() > documentGlobalWeight->getGlobalWeight())
					{
						orderDocs->insert(it3, documentGlobalWeight);
						insertado = true;
					}
				}
			}
		}

		if (!insertado)
		{
			orderDocs->push_back(documentGlobalWeight);
			insertado = true;
		}
	}

	return orderDocs;
}

double Mediator::calculateDocumentGlobalWeight(int idDocument, int infinityNorm, int totalDocuments, int numberDocsWithWord, int repetition)
{
	//p(ti, dj) = ( aij * log(N/n) ) / norma infinito
	//ti: termino i
	//dj: documento j
	//aij; cant de apariciones del termino i en el documento j
	//N: cant. docuemtnos Totales
	//ni: cantidad de docuemtnos donde aparece el termino i

	double division = (double)totalDocuments / (double)numberDocsWithWord;
	double logaritm = log(division);
	double result = (double)(repetition * logaritm) / (double)infinityNorm;

	return result;
}

int Mediator::getInfinityNorm(int idDocument)
{
	//busca la norma infinito
	ByteString* bsId = new ByteString();
	bsId->insertLast(&idDocument,sizeof(int));
	string clave = bsId->toString();
	delete bsId;

	Key* keyIdDocument = new Key(clave);
	pair<Record*, BPlusTreeIterator*> searchResult = this->infinityNormIndex->search(*keyIdDocument);
	int infinityNorm = searchResult.first->getData()->readAsInt(0);
	delete searchResult.first;
	delete searchResult.second;
	delete keyIdDocument;

	return infinityNorm;
}

list<int>* Mediator::getListOfPositions(int idDocument, int idTerm)
{
	//busca la lista de ocurrencias de un termino en un documento
	ByteString* bs = new ByteString();
	bs->insertLast(&idDocument,sizeof(int));
	bs->insertLast(&idTerm,sizeof(int));
	string clave = bs->toString();
	delete bs;

	Key* k = new Key(clave);
	list<int>* listOfPos = this->ocurrenceTree->searchAllIds(*k);
	delete k;

	return listOfPos;
}

void Mediator::exportInvertedList(string pathname)
{
	this->indexWrapper->exportInvertedList(pathname);
}

void Mediator::exportToTXT(fileItem item,string filepath)
{
	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.export");
	switch (item) {
	case LISTAS:
		this->exportInvertedList(indexPath+filepath);break;
	case NORMA_INFINITO:
			this->infinityNormIndex->exportToPlainText(indexPath+filepath,false,false);break;
	case TERMINOS:
		this->vocabularyIndex->exportToPlainText(indexPath+filepath,false,true);break;
	case OCURRENCIA_POSICIONAL:
		this->ocurrenceTree->exportToPlainText(indexPath+filepath,false,false);break;
	}
}

string Mediator::codeTittle (string clave)
{
	string code;

	// Una sola palabra -> hashtag
	if ( clave.find(' ', 0) == string::npos){
		if (clave.length() > 25){
			code = clave.substr(0,25);
		}else{
			code = clave;
		}
	// Varias palabras -> Titulo de noticia
	}else{
		code = clave.substr(0,1);
		size_t pos = clave.find(' ', 0);

		while ( pos != string::npos){
			clave = clave.substr(pos+1,clave.length());
			code.append(clave.substr(0,1));
			if ( !clave.empty() ){
				pos = clave.find(' ', 0);
			}else{
				pos = string::npos;
			}
		}
	}

	return code;
}

string  Mediator::searchInternalID(string identificator){
	Key* k = NULL;
	string toReturn;
	list<int>* documentIds;

	k = new Key(identificator);
	documentIds = this->indexWrapper->searchAllIds(k, IndexWrapper::IDENTIFICADOR);

	return Utility::intToString(documentIds->front());
}

