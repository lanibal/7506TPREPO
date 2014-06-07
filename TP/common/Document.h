#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include "../common/ByteString.h"
#include "../common/Serializer.h"
#include "../Utility/Utility.h"

using namespace std;

class Document : public Serializer{
private:
	string author;
	string identificador;
	string titulo;
	string text;
	int fakeId;
	int fecha;

public:
	Document();
	virtual ~Document();

	void setAuthor(string author);
	void setIdentificador(string identificador);
	void setTitulo(string titulo);
	void setDescription(string text);
	void setFecha(int count);
	void setFakeId(int id);

	//Devuelve el documento serializada
	ByteString Serialize();

	//Carga el objeto a partir de uno serializado
	bool Hidratate(ByteString & documentByte);

	string getAuthor();
	string getTitle();
	string gettitulo();
	string getText();
	string getDocumentFormatedText();
	int getWordCount();
	int getId();
};

#endif /* DOCUMENT_H_ */
