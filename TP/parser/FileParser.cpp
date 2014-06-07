#include "FileParser.h"

FileParser::FileParser()
{
	this->lines = Utility::stringToInt(Resource::getConfigProperties()->get("miBiografia.titulo.cantlineas"));
	string controlPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control");
	this->knowntitulos = gettitulos(controlPath + "tituloes.config");
	this->stopwords = Utility::splitToSet(Resource::getConfigProperties()->get("miBiografia.stopwords"), ";");
	document = new Document();
	endfile = false;

	filepos = 0;
	lastline.clear();
}

FileParser::~FileParser() {
	string controlPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("miBiografia.directorio.archivos.control");
	settitulos(controlPath + "tituloes.config");

	delete(document);

	for(map<string,Term*>::iterator it = this->terms.begin(); it != this->terms.end(); ++it)
		delete (*it).second;
}


bool FileParser::parse(string path)
{
	string fileName;
	string authorData;
	string type;


	bool validPath = true;
	bool end = false;
	int index = -1;

	// Se intenta abrir el archivo
	this->file = new ifstream(path.c_str());

	if (!*this->file)
	{
		delete(this->file);
		validPath = false;
	}

	if (validPath)
	{
		/* Se obtiene solo el nombre del archivo */
		fileName = Utility::normalizeString(path);
		while(!end)
		{
			index = fileName.find('/', 0);
			if (index >= 0)
				fileName = fileName.substr((fileName.find('/', 0)+1), fileName.length());
			else
				end = true;
		}

		/* Del comienzo hasta la primer "." es el AUTOR */
		authorData = fileName.substr(0, fileName.length()-4);

		type = fileName.substr(fileName.length()-3, fileName.length());
		type = Utility::toLower(type);

		document->setAuthor(Utility::trim(authorData));


		if(type.compare("twt")==0 ){
			/* Se procesa el archivo */
			this->processTWT(path);
			document->setIdentificador("T");
		} else if(type.compare("rss")==0 ){
			this->processRSS(path);
			document->setIdentificador("N");
		} else {
			delete(this->file);
			validPath = false;
		}

		delete(this->file);

	}
	return (validPath && end && !endfile);
}



ByteString FileParser::processDescription(ByteString input){
	int index = 0;
	ByteString output;
	string word;
	size_t end;

	while (end == string::npos){
		end = input.toString().find(' ',index);
		if(end == string::npos)
			word = input.toString().substr(index,input.toString().length());
		else
			word = input.toString().substr(index,end);

		if(!(isStopword(word)))
    		output.insertLast(Utility::toLower(word));

		index =  input.toString().find(' ',index)+1;
	}

	return input;
}


void FileParser::processTWT(string path)
{
	ByteString text;
	string titleData;
	string dateData;
	string textData;

	string line;

	this->file = new ifstream(path.c_str());

	this->file->seekg(filepos);

	getline(*this->file, line);

	if (this->file->eof() || this->file->bad())
		endfile = true;

	if (!endfile && line != "")
	{
		dateData = line.substr(0,(line.find('@', 0)-1));
		dateData = Utility::dateFormat(dateData);
		document->setFecha(Utility::stringToInt(dateData));

		textData = line.substr(line.find("says: ")+6,line.length());
		text.clean();
		text.insertLast(Utility::trim(textData));
		document->setDescription(processDescription(text).toString());

		if (line.find('#', 0) != string::npos){
			titleData = line.substr(line.find('#', 0)+1,line.length());
			titleData = titleData.substr(0,titleData.find(' ', 0));
			document->setTitulo(titleData);
		}else{
			document->setTitulo("n/a");
		}

		filepos = this->file->tellg();
	}

}



void FileParser::processRSS(string path)
{
	ByteString text;
	string titleData;
	string dateData;
	string textData;

	string line;

	this->file = new ifstream(path.c_str());
	this->file->seekg(filepos);

	getline(*this->file, line);

	// Avanzo hasta encontrar el titulo. Alli comienza el RSS
	while(  line.find("Title: ",0) == string::npos && !this->file->eof() && !this->file->bad() )
	{
		getline(*this->file, line);
	}

	if (this->file->eof() || this->file->bad()){
		endfile = true;
	}else{
		// Guardo el titulo
		titleData = line.substr(7,line.length());
		document->setTitulo(Utility::trim(titleData));

		// Avanzo hasta encontrar la descripcion.
		while(  line.find("Description: ",0) == string::npos && !this->file->eof() )
		{
			getline(*this->file, line);
		}

		if (!this->file->eof()){
			//Guardo la descripcion
			textData = line.substr(13,line.length());
			document->setDescription(processDescription(textData).toString());
		}

		// Avanzo hasta encontrar la fecha.
		while(  line.find("Pub.date: ",0) == string::npos && !this->file->eof() )
		{
			getline(*this->file, line);
		}

		if (!this->file->eof()){
			//Guardo la fecha
			dateData = line.substr(10,line.length());
			dateData = Utility::dateFormat(Utility::trim(dateData));
			document->setFecha(Utility::stringToInt(dateData));
		}

		if (this->file->eof()){
			endfile = true;
		}else{
			filepos = this->file->tellg();
		}
	}

}


bool FileParser::findtitulo(string line)
{
	list<string>::iterator it;
	string normalizedLine;
	string titulo;
	bool findIt = false;

	normalizedLine = Utility::normalizeString(line);

	for(it = this->knowntitulos.begin(); it != this->knowntitulos.end(); ++it)
	{
		titulo = (*it);
		titulo = Utility::normalizeString(titulo);

		if (normalizedLine.find(titulo) != string::npos)
		{
			document->setTitulo((*it));
			findIt = true;
			break;
		}
	}
	return findIt;
}

void FileParser::setWords(string text)
{
	this->words.clear();
	this->terms.clear();
	int initial = 0; // indice que apunta al primer caracter valido
	int final = 0;   // indice que apunta al primer caracter invalido
	char chr;
	string word;
	unsigned int index = 0;
	unsigned int position = 0;

	while (index < text.size())
	{
		chr = text[index];

		if (chr == '\303')
		{
			// Caso particular para un caracter extendido --> tienen dos caracteres relacionados
			// Si lo es    --> se toma como parte de la palabra
			// Si no lo es --> se ignora y se continua con el proximo caracter
			char nextChar = text[index + 1];

			if (Utility::getNormalizedChar(nextChar) != 0x00)
			{
				index += 2;
				final += 2;
			}
		}
		else
		{
			if ((chr >= (char)65 && chr <= (char)90) || (chr >= (char)97 && chr <= (char)122) || (chr >= (char)48 && chr <= (char)57))
			{
				// El caracter es una letra o un numero
				final += 1;
			}
			else
			{
				// Se corta la secuencia y se toma la palabra
				word = text.substr(initial, final - initial);

				this->addWord(word, position);
//
//				if (!Utility::empty(word))
//				{
//					// Se actualiza la posicion de la palabra en el texto
//					position += 1;
//
//					// Se normaliza la palabra
//					word = Utility::normalizeString(word);
//
//					// Se incorpora a la lista de palabras, si no es stopword y si ya no existe en la lista
//					if(!(this->isStopword(word)) && !(this->words.count(word) > 0))
//					{
//						this->words.insert(word);
//					}
//
//					// ----------------------------------- //
//					// Se incorpora a la lista de terminos
//					// ----------------------------------- //
//					if(!(this->isStopword(word)))
//						this->addTerm(word, position);
//					// ----------------------------------- //
//				}

				initial = index + 1;

				final = initial;
			}

			index += 1;
		}
	}

	// Se incorpora la ultima palabra
	word = text.substr(initial, final - initial);
	this->addWord(word, position);

	//this->MostrarTeminos();
}

bool FileParser::isStopword(string word)
{
	return (this->stopwords.count(word) > 0);
}

set<string> FileParser::getWords()
{
	return this->words;
}

int FileParser::getWordCount()
{
	return this->words.size();
}

Document* FileParser::getDocument()
{
	return this->document;
}


list<string> FileParser::gettitulos(string fileName)
{
	ifstream file;
	file.open(fileName.c_str(), ios::in | ios::out | ios::binary);
	string titulos;
	getline(file,titulos);
	file.close();
	return Utility::split(titulos,";");
}

void FileParser::settitulos(string fileName)
{
	list<string>::iterator it;
	string titulo;
	ofstream file;

	remove(fileName.c_str());

	//FILE* result = fopen(fileName.c_str(),"w+");

	file.open(fileName.c_str(), ios::in | ios::out | ios::binary);
	if(file.is_open())
	{
		file.seekp(0);
		string titulos;
		for(it = this->knowntitulos.begin(); it != this->knowntitulos.end(); ++it)
		{
			titulo = (*it);
			titulo += ";";
			titulos += titulo;
		}
		file<<titulos<<endl;
		file.close();
	}
}

void FileParser::addtitulo(string titulo)
{
	list<string>::iterator it;
	bool findIt;
	titulo = Utility::normalizeString(titulo);
	string actualtitulo;
	for(it = this->knowntitulos.begin(); it != this->knowntitulos.end(); ++it)
	{
		actualtitulo = (*it);
		actualtitulo = Utility::normalizeString(actualtitulo);

		if (strcmp(actualtitulo.c_str(),titulo.c_str()) == 0)
		{
			findIt = true;
			break;
		}
	}
	if(!findIt)
		this->knowntitulos.push_back(titulo);
}

void FileParser::addWord(string word, unsigned int &position)
{
	if (!Utility::empty(word))
	{
		// Se actualiza la posicion de la palabra en el texto
		position += 1;

		// Se normaliza la palabra
		word = Utility::normalizeString(word);

		// Se incorpora a la lista de palabras, si no es stopword y si ya no existe en la lista
		if(!(this->isStopword(word)) && !(this->words.count(word) > 0))
		{
			this->words.insert(word);
		}
		// ----------------------------------- //
		// Se incorpora a la lista de terminos
		// ----------------------------------- //
		if(!(this->isStopword(word)))
			this->addTerm(word, position);
		// ----------------------------------- //
	}
}


void FileParser::addTerm(string word, unsigned int position)
{
	if (this->terms.count(word) == 0)
	{
		Term* newTerm = new Term(word, position);
		this->terms.insert(pair<string,Term*>(word, newTerm));
	}
	else
	{
		map<string,Term*>::iterator it = this->terms.find(word);

		if (it != this->terms.end())
		{
			//Term* term = it->second;
			//term->addPosition(position);
			it->second->addPosition(position);
		}
	}
}

unsigned int FileParser::getInfinityNorm()
{
	unsigned int maximumFrequency = 0;

	for(map<string,Term*>::iterator it = this->terms.begin(); it != this->terms.end(); ++it)
	{
		if (maximumFrequency < (*it).second->getFrecuency())
			maximumFrequency = (*it).second->getFrecuency();
	}

	return maximumFrequency;
}

map<string,Term*> FileParser::getTerms() const
{
    return terms;
}

void FileParser::setTerms(map<string,Term*> terms)
{
    this->terms = terms;
}

void FileParser::MostrarTeminos()
{
	// Se recorre todos los terminos del diccionario
	for(map<string,Term*>::iterator it = this->terms.begin(); it != this->terms.end(); ++it)
	{
		cout << "Palabra: '" << (*it).second->getWord() << "'" << endl;
		cout << "Frecuencia: " << (*it).second->getFrecuency() << " apariciones" << endl;
		cout << "Posiciones: " << endl;

		list<unsigned int> positions = (*it).second->getPositions();

		// Se recorre todas las posiciones de la lista
		for (list<unsigned int>::iterator it2 = positions.begin(); it2 != positions.end(); ++it2)
		{
			cout << (*it2) << endl;
		}
		cout << endl;
	}

	cout << "Norma Infinito: '" << this->getInfinityNorm() << "'" << endl;
}
