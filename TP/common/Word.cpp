#include "Word.h"

Word::Word() {
	this->word = "";
	this->documents = new list<int>();
}

Word::Word(string word, list<int>* document) {
	this->word = word;

	if (document->size() >= 2)
	{
		this->idWord = document->back() * (-1);
		//Elimina el id de termino contenido en la list
		document->remove(this->idWord * (-1));
		this->documents = document;
	}
	else
	{
		this->idWord = 0;
		this->documents = new list<int>();
	}
}

Word::~Word() {
	delete (this->documents);
}

string Word::getWord()
{
	return this->word;
}

int Word::getIdWord()
{
	return this->idWord;
}

list<int>* Word::getDocuments()
{
	return this->documents;
}
