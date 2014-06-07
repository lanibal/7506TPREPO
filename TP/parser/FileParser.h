#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <string>
#include <list>
#include <set>
#include <map>
#include "../common/Document.h"
#include "../common/Resource.h"
#include "../common/ByteString.h"
#include "../Utility/Utility.h"
#include "Term.h"
#include <fstream>

using namespace std;

class FileParser {
private:
	ifstream* file;
	list<string> knowntitulos;
	set<string> stopwords;
	set<string> words;
	int lines;
	Document* document;
	bool endfile;

	map<string, Term*> terms;

	streampos filepos;
	string lastline;

	void processTWT(string path);
	void processRSS(string path);

	bool findtitulo(string line);
	bool isStopword(string word);
	void settitulos(string fileName);
	list<string> gettitulos(string fileName);
	void addTerm(string word, unsigned int position);
	void addWord(string word, unsigned int &position);
	ByteString processDescription(ByteString input);


public:

	FileParser();
	virtual ~FileParser();

	bool parse(string path);
	bool parseTWT(string path);
	bool parseRSS(string path);

	set<string> getWords();
	void setWords(string text);
	int getWordCount();
	Document* getDocument();
	void addtitulo(string titulo);
	unsigned int getInfinityNorm();

	void MostrarTeminos();
    map<string,Term*> getTerms() const;
    void setTerms(map<string,Term*> terms);
};

#endif /* FILEPARSER_H_ */
