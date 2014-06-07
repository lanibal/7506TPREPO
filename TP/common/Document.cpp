#include "Document.h"

using namespace std;

Document::Document() {
	this->author = "";
	this->titulo = "";
	this->identificador = "";
	this->text = "";
	this->fakeId = -1;
}

Document::~Document() {
}

void Document::setAuthor(string author){
	this->author = author;
}

void Document::setTitulo(string titulo){
	this->titulo = titulo;
}

void Document::setFakeId(int id){
	this->fakeId = id;
}

void Document::setIdentificador(string identificador){
	this->identificador = identificador;
}

void Document::setDescription(string text){
	this->text = text;
}

void Document::setFecha(int fecha){
	this->fecha = fecha;
}

ByteString Document::Serialize(){
	ByteString bs;
	bs.insertLast(&(this->fakeId), sizeof(int));
	size_t authorSize = this->author.size();
	bs.insertLast(&(authorSize), sizeof(size_t));
	bs.insertLast(this->author);
	size_t tituloSize = this->titulo.size();
	bs.insertLast(&(tituloSize), sizeof(size_t));
	bs.insertLast(this->titulo);
	size_t titleSize = this->identificador.size();
	bs.insertLast(&(titleSize), sizeof(size_t));
	bs.insertLast(this->identificador);
	size_t textSize = this->text.size();
	bs.insertLast(&(textSize), sizeof(size_t));
	bs.insertLast(this->text);
	bs.insertLast(&(this->fecha), sizeof(int));
	return bs;
}


bool Document::Hidratate(ByteString & documentByte){
	int position = 0;

	// id
	this->fakeId = documentByte.readAsInt(position);
	position += sizeof(int);

	//  author
	size_t authorSize = documentByte.readAsInt(position);
	position += sizeof(size_t);
	this->author = (documentByte.read(position, authorSize)).toString();
	position += authorSize;

	// titulo
	size_t tituloSize = documentByte.readAsInt(position);
	position += sizeof(size_t);
	this->titulo = (documentByte.read(position, tituloSize)).toString();
	position += tituloSize;

	// title
	size_t titleSize = documentByte.readAsInt(position);
	position += sizeof(size_t);
	this->identificador = (documentByte.read(position, titleSize)).toString();
	position += titleSize;

	// text
	size_t textSize = documentByte.readAsInt(position);
	position += sizeof(size_t);
	this->text = (documentByte.read(position, textSize)).toString();
	position += textSize;

	// wordcount
	this->fecha = documentByte.readAsInt(position);

	return true;
}

string Document::gettitulo(){
	return this->titulo;
}



string Document::getText(){
	return this->text;
}



string Document::getAuthor(){
	return this->author;
}



string Document::getTitle(){
	return this->identificador;
}



int Document::getWordCount(){
	return this->fecha;
}



int Document::getId(){
	return this->fakeId;
}



string Document::getDocumentFormatedText(){

	string out = "";

	out.append(this->getTitle());
	out.append("  |  ");

	out.append(this->getAuthor());
	out.append("  |  ");

	out.append(this->gettitulo());
	out.append("  |  ");

	out.append(Utility::intToString(this->getWordCount()));
	out.append("\n");

	return out;
}


