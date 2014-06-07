#include "UserInterface/CommandParser.h"
#include "parser/FileParser.h"
#include "UserInterface/Mediator.h"
#include "Utility/Utility.h"
#include <iostream>

using namespace std;

bool existsConfigurationFile(string path)
{
	ifstream* file;
	bool exists = true;

	file = new ifstream(path.c_str());

	if (!*file)
		exists = false;

	delete(file);

	return exists;
}

string getIndexInternalName(IndexWrapper::indexItem indexId){
	switch(indexId){
	case IndexWrapper::AUTOR:
		return "IndiceAutor";
	case IndexWrapper::TITULO:
		return "Indicetitulo";
	case IndexWrapper::IDENTIFICADOR:
		return "IndiceTitulo";
	case IndexWrapper::PALABRAS:
		return "IndicePalabras";
	case IndexWrapper::FECHA:
		return "IndiceFecha";
	}
	return "";
}

void takeText(string path)
{
	int cant = 0;
	string documentos = "documentos";
	string type = path.substr(path.length()-3, path.length());
	type = Utility::toLower(type);
	if(type.compare("twt")==0 ){
		documentos = "twitts";
	} else if(type.compare("rss")==0 ){
		documentos = "noticias";
	}
	Mediator* mediator = new Mediator();
	Document* document;
	string titulo;
	document = mediator->getDocument(path);


	if (document == NULL){
		cout << "No se ha podido parsear el archivo ingresado." << endl;
	}else{
		while (document != NULL){
			if (mediator->addDocument(document) == false){
				cout << "Hubo un error al agregar " << documentos << " al sistema." << endl;
			} else {
				cant++;
			}
			document = mediator->getDocument(path);

		}
		cout << "Se incorporaron " << cant << " " << documentos << " al sistema en forma exitosa!" << endl;
	}

	delete mediator;

}

void processtitulo()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::TITULO, "Indicetitulo", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::TITULO))
		cout << "No se pudo indexar por titulo." << endl;

	delete mediator;
}

void processAuthor()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::AUTOR, "IndiceAutor", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::AUTOR))
		cout << "No se pudo indexar por Autor." << endl;

	delete mediator;
}

void processFecha()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::FECHA, "IndiceFecha", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::FECHA))
		cout << "No se pudo indexar por fecha." << endl;

	delete mediator;
}


void processTitle()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::IDENTIFICADOR, "IndiceTitulo", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::IDENTIFICADOR))
		cout << "No se pudo indexar por Titulo." << endl;
	mediator->closeIndex(IndexWrapper::IDENTIFICADOR);
	delete mediator;
}

void processWords()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::PALABRAS))
		cout << "No se pudo indexar por Palabras." << endl;
	mediator->closeIndex(IndexWrapper::PALABRAS);
	delete mediator;
}

void listFiles()
{
	Mediator* mediator = new Mediator();
	string listOfTakenDocuments = mediator->getListOfTakenDocuments();
	cout<<endl;
	cout<<listOfTakenDocuments<<endl;
	delete mediator;
}

void getFiles(string fileId)
{
	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.export");
	Mediator* mediator = new Mediator();
	//string nfileId = fileId.substr(1,fileId.length()-1);


	mediator->initializeIndex(IndexWrapper::AUTOR, "IndiceAutor", 50);
	mediator->initializeIndex(IndexWrapper::TITULO, "Indicetitulo", 50);
	mediator->initializeIndex(IndexWrapper::IDENTIFICADOR, "IndiceTitulo", 50);
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	mediator->initializeIndex(IndexWrapper::FECHA, "IndiceFecha", 50);

	Key* key = new Key(fileId);
	list<int>* documentIds;
	documentIds = mediator->getIndexWrapper()->searchAllIds(key, IndexWrapper::IDENTIFICADOR);
	list<int>::iterator it = documentIds->begin();
	int id = *it;


	//int id = Utility::stringToInt(fileId);
	ByteString bs = mediator->getDocumentFromFile(id);
	if(!bs.isEmpty())
	{
		Document document;
		document.Hidratate(bs);
		string fileName = indexPath + document.getAuthor() + "-" + document.getTitle() + ".txt";
		FILE* result = fopen(fileName.c_str(),"w+");
		if(result)
		{
			ofstream file;
			file.open(fileName.c_str(), ios::in | ios::out | ios::binary);
			if(file.is_open())
			{
				file.seekp(0);
				string text = document.getText();
				file.write(text.c_str(),text.size());
				cout << "Archivo exportado satisfactoriamente a: " << fileName << endl;
			}
		}
	}
	else
		cout << "El documento solicitado no existe"<< endl;

	delete mediator;

	delete key;
}

void deleteFiles(string fileId)
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::AUTOR, "IndiceAutor", 50);
	mediator->initializeIndex(IndexWrapper::TITULO, "Indicetitulo", 50);
	mediator->initializeIndex(IndexWrapper::IDENTIFICADOR, "IndiceTitulo", 50);
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	mediator->initializeIndex(IndexWrapper::FECHA, "IndiceFecha", 50);

	//string nfileId = fileId.substr(1,fileId.length()-1);
	Key* key = new Key(fileId);
	list<int>* documentIds;
	documentIds = mediator->getIndexWrapper()->searchAllIds(key, IndexWrapper::IDENTIFICADOR);
	list<int>::iterator it = documentIds->begin();
	int id = *it;
	//int id = Utility::stringToInt(fileId);

	bool deleted = mediator->removeDocument(id);
	if(deleted)
		cout<<"Se ha borrado el archivo Exitosamente"<<endl;
	else
		cout<<"No se ha podido borrar el archivo"<<endl;

	delete mediator;

	delete key;
}

void deleteFilesFecha(string fileId)
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::AUTOR, "IndiceAutor", 50);
	mediator->initializeIndex(IndexWrapper::TITULO, "Indicetitulo", 50);
	mediator->initializeIndex(IndexWrapper::IDENTIFICADOR, "IndiceTitulo", 50);
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	mediator->initializeIndex(IndexWrapper::FECHA, "IndiceFecha", 50);
	int id = Utility::stringToInt(fileId);
	bool deleted = mediator->removeDocument(id);
	if(deleted)
		cout<<"Se ha borrado el archivo Exitosamente"<<endl;
	else
		cout<<"No se ha podido borrar el archivo"<<endl;

	delete mediator;
}


void seeStructure(IndexWrapper::indexItem indexId, string pathFile){

	Mediator* m = new Mediator();
	m->initializeIndex(indexId, getIndexInternalName(indexId), 50);
	m->exportToPlainText(indexId, pathFile);
	cout << "Archivo exportado satisfactoriamente a: " << pathFile << endl;
	delete m;
}

void querytitulo(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::TITULO, getIndexInternalName(IndexWrapper::TITULO), 50);
	cout << m->search(IndexWrapper::TITULO, toSearch) << endl;
	m->closeIndex(IndexWrapper::TITULO);
	delete m;
}

void queryAuthor(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::AUTOR, getIndexInternalName(IndexWrapper::AUTOR), 50);
	cout << m->search(IndexWrapper::AUTOR, toSearch) << endl;
	m->closeIndex(IndexWrapper::AUTOR);
	delete m;
}

void queryTitle(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::IDENTIFICADOR, getIndexInternalName(IndexWrapper::IDENTIFICADOR), 50);
	cout << m->search(IndexWrapper::IDENTIFICADOR, toSearch) << endl;
	m->closeIndex(IndexWrapper::IDENTIFICADOR);
	delete m;
}

void queryFecha(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::FECHA, getIndexInternalName(IndexWrapper::FECHA), 50);
	cout << m->search(IndexWrapper::FECHA, toSearch) << endl;
	m->closeIndex(IndexWrapper::FECHA);
	delete m;
}


void queryTerms(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::PALABRAS, getIndexInternalName(IndexWrapper::PALABRAS), 50);
	cout << m->search(IndexWrapper::PALABRAS, toSearch) << endl;
	m->closeIndex(IndexWrapper::PALABRAS);
	delete m;
}

void seeStructureFile(Mediator::fileItem IdFile, string pathFile)
{
	Mediator* m = new Mediator();
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	m->exportToTXT(IdFile,pathFile);
	delete m;
}

void printHelp(string msg)
{
	cout << msg;
}


int main(int argc, char** argv)
{	cout << "\nBienvenido a la aplicacion MiBiografia !!!" << endl;
	//Se guarda el application path en la variable static
	Utility::setApplicationPath(argv[0]);

	if (existsConfigurationFile(Utility::aplicationPath + "miBiografia.config"))
	{
		CommandParser* commandParser = new CommandParser();
		if (commandParser->validArguments(argc, argv))
		{

			switch (commandParser->getOption())
			{

				case TomarTexto:
					takeText(commandParser->getPathFile());
					processtitulo();
					processAuthor();
					processTitle();
					processFecha();
					break;
				case ListarArchivos:
					listFiles();
					break;
				case ObtenerArchivo:
					getFiles(commandParser->getFileId());
					break;
				case QuitarArchivo:
					deleteFiles(commandParser->getFileId());
					break;
				case QuitarArchivoPorFecha:
				//	deleteFilesfecha();
					break;
				case VerEstructuraIndices:
					seeStructure(commandParser->getIndexId(), commandParser->getPathFile());
					break;

				case VerEstructuraArchivos:
					cout << "Opcion: Ver Estructura Archivos" << endl;
					cout << "Archivo Salida: '" << commandParser->getPathFile() << "'" << endl;
					seeStructureFile(commandParser->getIdFile(), commandParser->getPathFile());
					break;
				case Consultartitulo:
					cout << "Opcion: Consultar titulo" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					querytitulo(commandParser->getQueryTerm());
					break;
				case ConsultarAutor:
					cout << "Opcion: Consultar Autor" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryAuthor(commandParser->getQueryTerm());
					break;
				case ConsultarTitulo:
					cout << "Opcion: Consultar Titulo" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryTitle(commandParser->getQueryTerm());
					break;
				case ConsultarPalabras:
					cout << "Opcion: Consultar Palabras" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryTerms(commandParser->getQueryTerm());
					break;
				case ConsultarFecha:
					cout << "Opcion: Consultar Fecha" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryFecha(commandParser->getQueryTerm());
					break;

				case Ayuda:
					printHelp(commandParser->printHelp());
					break;
			};

		}
		else
			cout << "Los argumentos son invalidos" << endl;

		delete commandParser;
	}
	else
		cout << "No se ha podido ejecutar la aplicacion: no se encontró el archivo de configuracion 'miBiografia.config'" << endl;

	return 0;
}


