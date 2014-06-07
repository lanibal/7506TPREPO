#include "DataDocumentManager.h"
#include "../common/Resource.h"
#include "../Utility/Utility.h"

DataDocumentManager::DataDocumentManager(string fileNameVR,int maxKeySizeBPTree)
{
	this->fileVariableRecord = new FileVariableRecord(fileNameVR);
	this->fileFlagsTitulo = new FileFlags("Titulo");
	this->fileFlagsAutor = new FileFlags("Autor");
	this->fileFlagsIdentificadores = new FileFlags("Identificadores");
	this->fileFlagsPalabras = new FileFlags("Palabras");
	this->fileFlagsFechas = new FileFlags("Fechas");

	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.index");
	this->principalIndex = new BPlusTree(indexPath + "indexPrincipal",maxKeySizeBPTree);
	//FIXME

	string incrementPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control") + "IntAutoInc.inc";
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

	incrementPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control") + "RssAutoInc.inc";
	this->rssAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
	open = this->rssAutoIncInteger.is_open();
	if(!open)
	{
		fopen(incrementPath.c_str(),"w+");
		this->rssAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
		this->rssAutoIncInteger.seekp(0);
		int cero = 0;
		this->rssAutoIncInteger.write((char*)&cero,sizeof(int));
	}

	incrementPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control") + "TwtAutoInc.inc";
	this->twtAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
	open = this->twtAutoIncInteger.is_open();
	if(!open)
	{
		fopen(incrementPath.c_str(),"w+");
		this->twtAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
		this->twtAutoIncInteger.seekp(0);
		int cero = 0;
		this->twtAutoIncInteger.write((char*)&cero,sizeof(int));
	}

	this->fileAutoIncInteger.seekg(0);
	int incInteger;
	this->fileAutoIncInteger.read((char*)&incInteger,sizeof(int));
	this->autoIncInteger = incInteger;

	this->twtAutoIncInteger.seekg(0);
	this->twtAutoIncInteger.read((char*)&incInteger,sizeof(int));
	this->twtautoIncInteger = incInteger;

	this->rssAutoIncInteger.seekg(0);
	this->rssAutoIncInteger.read((char*)&incInteger,sizeof(int));
	this->rssautoIncInteger = incInteger;


	if(!this->fileVariableRecord->openFile()){
		this->fileVariableRecord->createFile();
		this->fileVariableRecord->openFile();
	}
}

DataDocumentManager::~DataDocumentManager()
{
	this->fileVariableRecord->closeFile();
	delete this->fileVariableRecord;
	delete this->fileFlagsTitulo;
	delete this->fileFlagsAutor;
	delete this->fileFlagsIdentificadores;
	delete this->fileFlagsPalabras;
	delete this->fileFlagsFechas;

	this->fileAutoIncInteger.seekp(0);
	int lastId = this->autoIncInteger;
	this->fileAutoIncInteger.write((char*)&lastId,sizeof(int));
	this->fileAutoIncInteger.close();

	this->rssAutoIncInteger.seekp(0);
	lastId = this->rssautoIncInteger;
	this->rssAutoIncInteger.write((char*)&lastId,sizeof(int));
	this->rssAutoIncInteger.close();

	this->twtAutoIncInteger.seekp(0);
	lastId = this->twtautoIncInteger;
	this->twtAutoIncInteger.write((char*)&lastId,sizeof(int));
	this->twtAutoIncInteger.close();

	delete this->principalIndex;
}

bool DataDocumentManager::addIDtoFileFlags(IndexWrapper::indexItem item,int ID)
{
	bool added = false;
	switch(item){
	case IndexWrapper::AUTOR :
								added = fileFlagsAutor->addID(ID);
								break;
	case IndexWrapper::TITULO :
								added = fileFlagsTitulo->addID(ID);
								break;
	case IndexWrapper::IDENTIFICADOR :
								added = fileFlagsIdentificadores->addID(ID);
								break;
	case IndexWrapper::PALABRAS :
								added = fileFlagsPalabras->addID(ID);
								break;

	case IndexWrapper::FECHA:
						added = fileFlagsFechas->addID(ID);
	}
	return added;
}

bool DataDocumentManager::removeIDtoFileFlags(IndexWrapper::indexItem item,int ID)
{
	bool removed = false;
	switch(item){
	case IndexWrapper::AUTOR :
								removed = fileFlagsAutor->removeID(ID);
								break;
	case IndexWrapper::TITULO :
								removed = fileFlagsTitulo->removeID(ID);
								break;
	case IndexWrapper::IDENTIFICADOR :
								removed = fileFlagsIdentificadores->removeID(ID);
								break;
	case IndexWrapper::PALABRAS :
								removed = fileFlagsPalabras->removeID(ID);
								break;
	case IndexWrapper::FECHA :
							removed = fileFlagsFechas->removeID(ID);
							break;
	}
	return removed;
}

List<int>* DataDocumentManager::getListToFileFlag(IndexWrapper::indexItem item)
{
	List<int>* list;
	switch(item){
	case IndexWrapper::AUTOR :
								list = fileFlagsAutor->getListID();
								break;
	case IndexWrapper::TITULO :
								list = fileFlagsTitulo->getListID();
								break;
	case IndexWrapper::IDENTIFICADOR :
								list = fileFlagsIdentificadores->getListID();
								break;
	case IndexWrapper::PALABRAS :
								list = fileFlagsPalabras->getListID();
								break;
	case IndexWrapper::FECHA :
							list = fileFlagsFechas->getListID();
							break;
	}
	return list;
}

bool DataDocumentManager::addDocument(ByteString byteString)
{
	bool saved = false;
	int realID = this->autoIncInteger + 1;
	int documentID;
	string sid = Utility::intToString(realID);
	string did;

	Document document;
	document.Hidratate(byteString);
	document.setFakeId(realID);
	if(document.getTitle()[0] == 'T')
		documentID = this->twtautoIncInteger + 1;
	else if(document.getTitle()[0] == 'N')
		documentID = this->rssautoIncInteger + 1;
	did = Utility::intToString(documentID);
	document.setIdentificador(Utility::concat(document.getTitle(), did));

	int position = this->fileVariableRecord->addDocument(document.Serialize());
	if(position > -1)
	{
		this->autoIncInteger++;
		if(document.getTitle()[0] == 'N')
			rssautoIncInteger++;
		else if(document.getTitle()[0] == 'T')
			twtautoIncInteger++;
		ByteString idDocument;
		idDocument.insertLast(&realID, sizeof(int));
		Key* key = new Key(idDocument.toString());
		ByteString offset;
		offset.insertLast(&position, sizeof(int));
		Record* recPrincipalInd = new Record(key,&offset);
		this->principalIndex->add(recPrincipalInd);
		addIDtoFileFlags(IndexWrapper::AUTOR,idDocument.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::TITULO,idDocument.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::PALABRAS,idDocument.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::IDENTIFICADOR,idDocument.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::FECHA,idDocument.readAsInt(0));

		saved = true;
	}
	return saved;
}

ByteString DataDocumentManager::getDocument(int ID)
{
	ByteString recordString;
	ByteString byteString;
	byteString.insertLast(&ID,sizeof(int));
	string idString = byteString.toString();
	Key key(idString);
	pair<Record*, BPlusTreeIterator*> searchResult = this->principalIndex->search(key);
	if(searchResult.first != NULL)
	{
		int position = searchResult.first->getData()->readAsInt(0);
		if(position > -1)
		{
			recordString = this->fileVariableRecord->readDocument(position);
		}

		//delete poddo
		delete searchResult.first;
	}

	//delete poddo
	if (searchResult.second != NULL)
		delete searchResult.second;

	return recordString;
}

bool DataDocumentManager::removeRecord(int ID)
{
	bool deleted = false;
	ByteString byteString;
	byteString.insertLast(&ID,sizeof(int));
	string idString = byteString.toString();
	Key key(idString);
	pair<Record*, BPlusTreeIterator*> searchResult = this->principalIndex->search(key);
	if(searchResult.first != NULL)
	{
		int position = searchResult.first->getData()->readAsInt(0);
		if(position > -1)
		{
			this->fileVariableRecord->deleteDocument(position);
			this->principalIndex->removeKey(key);
			ByteString bs(key.toString());
			removeIDtoFileFlags(IndexWrapper::AUTOR,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::TITULO,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::PALABRAS,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::IDENTIFICADOR,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::FECHA,bs.readAsInt(0));
			deleted = true;
		}
	}
	return deleted;
}

string DataDocumentManager::getlistOfTakenDocuments()
{
	ByteString bs = this->fileVariableRecord->readNext();
	string out = "Id  |     Autor    | Titulo  |  Fecha\n";

	while(!bs.isEmpty())
	{

		Document document;
		document.Hidratate(bs);

		out.append(document.getDocumentFormatedText());

		bs = this->fileVariableRecord->readNext();
	}
	return out;
}


