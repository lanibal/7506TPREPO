#ifndef DATA_BOOK_MANAGER_H_
#define DATA_DOCUMENT_MANAGER_H_

#include "../archivorv/FileVariableRecord.h"
#include "IndexWrapper.h"
#include "../common/FileFlags.h"
#include <iostream>
#include "../common/Document.h"

using namespace std;

class DataDocumentManager {

private:

	FileVariableRecord* fileVariableRecord;
	FileFlags* fileFlagsTitulo;
	FileFlags* fileFlagsAutor;
	FileFlags* fileFlagsIdentificadores;
	FileFlags* fileFlagsPalabras;
	FileFlags* fileFlagsFechas;
	BPlusTree* principalIndex;
	fstream fileAutoIncInteger;
	fstream rssAutoIncInteger;
	fstream twtAutoIncInteger;

	int autoIncInteger;
	int rssautoIncInteger;
	int twtautoIncInteger;

public:

	DataDocumentManager(string fileNameVR,int maxKeySizeBPTree = sizeof(int));
	/*
	 * Agrega al archivo de flags correspondiente el ID siempre que no exista devolviendo true, sino devuelve false
	 * Pre: El archivo debe estar creado y abierto
	 * Post: Devuelve la lista de IDs contenido
	 */
	bool addIDtoFileFlags(IndexWrapper::indexItem item,int ID);
	/*
	 * Vacia el archivo correspondiente al index indicado y devuelve la lista de IDs contenido
	 * Pre: El archivo debe estar creado y abierto
	 * Post: Devuelve la lista de IDs contenido
	 */
	List<int>* getListToFileFlag(IndexWrapper::indexItem item);
	/*
	 * Agrega el record pasado por parametro al archivo de registros variables
	 * Pre: El archivo debe estar creado y abierto
	 * Post: Devuelve la posicion donde se guardo el registro en el archivo de registros variables o -1 si no se agrego
	 */
	bool addDocument(ByteString byteString);

	bool removeRecord(int ID);

	bool removeIDtoFileFlags(IndexWrapper::indexItem item,int ID);

	ByteString getDocument(int ID);

	string getlistOfTakenDocuments();

	~DataDocumentManager();

};

#endif /* DATA_DOCUMENT_MANAGER_H_ */
