#include "CommandParser.h"

CommandParser::CommandParser()
{
	this->option = "";
	this->fileId = "";
	this->pathFile = "";
	this->queryTerm = "";
	this->exportIndexStructure = false;
}

CommandParser::~CommandParser()
{
}

bool CommandParser::validArguments(int count, char** params)
{
	string option;
	bool valid = false;
	int i;

	if (count > 1)
	{
		// Se obtiene la opcion elegida
		option = params[1];

		if ((Utility::equals(option, TOMAR_TEXTO)||(this->isQueryArguments(option))) && (count >= 3))
		{
			string secondParameter = "";

			for(i=2; i<count; i+=1)
			{
				secondParameter = Utility::concat(secondParameter, params[i]);
				secondParameter = Utility::concat(secondParameter, SPACE);
			}

			if (Utility::equals(option, TOMAR_TEXTO))
				this->pathFile = Utility::trim(secondParameter);
			else
				this->queryTerm = Utility::trim(secondParameter);

			valid = true;
		}
		else if ((this->isIndexArguments(option) || Utility::equals(option, LISTAR_ARCHIVOS)) && (count == 2))
			valid = true;
		else if ((Utility::equals(option, OBTENER_ARCHIVO) ||
				Utility::equals(option, QUITAR_ARCHIVO)) && (count == 3))
		{
			// Se carga el ID del archivo
			this->fileId = params[2];
			valid = true;
		}
		else if (Utility::equals(option, VER_ESTRUCTURA) && (count >= 4))
		{
			if (this->isIndexArguments(params[2]))
			{
				valid = true;
				this->exportIndexStructure = true;

				if(Utility::equals(params[2], PROCESAR_titulo))
					this->indexId = IndexWrapper::TITULO;
				if (Utility::equals(params[2], PROCESAR_AUTOR))
					this->indexId = IndexWrapper::AUTOR;
				if (Utility::equals(params[2], PROCESAR_TITULO))
					this->indexId = IndexWrapper::IDENTIFICADOR;
				if (Utility::equals(params[2], PROCESAR_PALABRAS))
					this->indexId = IndexWrapper::PALABRAS;
				if (Utility::equals(params[2], PROCESAR_FECHA))
					this->indexId = IndexWrapper::FECHA;
			}
			else if (this->isStructureArguments(params[2]))
			{
				valid = true;
				if (Utility::equals(params[2], ARCHIVO_TERMINOS))
					this->IdFile = Mediator::TERMINOS;
				if (Utility::equals(params[2], ARCHIVO_NORMA_INFINITO))
					this->IdFile = Mediator::NORMA_INFINITO;
				if (Utility::equals(params[2], ARCHIVO_OCURRENCIA_POSICIONAL))
					this->IdFile = Mediator::OCURRENCIA_POSICIONAL;
				if (Utility::equals(params[2], LISTAS_INVERTIDAS))
					this->IdFile = Mediator::LISTAS;
			}

			if (valid)
			{
				for(i=3; i<count; i+=1)
				{
					this->pathFile = Utility::concat(this->pathFile, params[i]);
					this->pathFile = Utility::concat(this->pathFile, SPACE);
				}

				this->pathFile = Utility::trim(this->pathFile);
			}
		}
		else if (Utility::equals(option, AYUDA) && (count == 2))
		{
				valid = true;
		}
	}

	if (valid)
		this->option = option;

	return valid;
}

string CommandParser::printHelp()
{

	/*
	* Obtener ayuda : 			./miBiografia -h

* Obtener nuevos Twitts:  		./miBiografia –i nombreArchivo.twt

* Obtener nuevos RSSs:  		./miBiografia –i nombreArchivo.rss

* Listar Documentos ingresados:	./miBiografia -l

	* Buscar por Autor:			./miBiografia -qa autor

	* Buscar por Titulo:			./miBiografia -qt titulo

	* Buscar por Fecha:			./miBiografia -qf fecha

	* Buscar por Identificador:		./miBiografia -qi id

	* Obtener Documento por Identificador: 	./miBiografia -o id

	* Eliminar todo lo contenido:			./miBiografia -et

	* Eliminar documento por Identificador:	./miBiografia -e id

	* Eliminar documento por Fecha:          	./miBiografia -ef fecha

	* Eliminar documento por Menor a Fecha:	./miBiografia -efm fecha

	* Eliminar documento por Mayor a Fecha:	./miBiografia -ef+ fecha

	* Ver estructura:			./miBiografia -v [-a|-t|-f|-i] nombreArchivoSalida.txt

	 *
	 * */
	string help;
	help = '\n';
	help += "************************************************************\n";
	help += "		Comandos Aplicacion MiBiografia\n";
	help += "************************************************************\n";
	help += "Obtener ayuda :                        '-h'\n";
	help += "Obtener nuevos Twitts:                 '-i nombreArchivo.twt'\n";
	help += "Obtener nuevos RSSs:                   '-i nombreArchivo.rss'\n";
	help += "Listar documentos ingresados:          '-l'\n";
	help += "Buscar por Autor:                      '-qa autor'\n";
	help += "Buscar por Titulo:                     '-qt titulo'\n";
	help += "Buscar por Fecha:                      '-qf fecha'\n";
	help += "Buscar por Identificador:              '-qi id'\n";
	help += "Obtener Documento por identificador:   '-o id'\n";
	help += "Eliminar todo lo contenido:            '-et'\n";
	help += "Eliminar Documento por identificador:  '-e id'\n";
	help += "Eliminar Documento por Fecha:          '-ef fecha'\n";
	help += "Eliminar Documento por menor a Fecha:  '-efm fecha'\n";
	help += "Eliminar Documento por mayor a Fecha:  '-ef+ fecha'\n";
	help += "Ver estructura:                        '-v [-a|-t|-f|-i] nombreArchivoSalida.txt'\n";
	help += '\n';
	return help;
}

bool CommandParser::isIndexArguments(string option)
{
	return (Utility::equals(option, PROCESAR_titulo)||
			Utility::equals(option, PROCESAR_AUTOR)||
			Utility::equals(option, PROCESAR_TITULO)||
			Utility::equals(option, PROCESAR_PALABRAS)||
			Utility::equals(option,PROCESAR_FECHA));
}

bool CommandParser::isQueryArguments(string option)
{
	return (Utility::equals(option, CONSULTAR_titulo)||
			Utility::equals(option, CONSULTAR_AUTOR)||
			Utility::equals(option, CONSULTAR_TITULO)||
			Utility::equals(option, CONSULTAR_PALABRAS)||
			Utility::equals(option, CONSULTAR_FECHA));
}

bool CommandParser::isStructureArguments(string option)
{
	return (Utility::equals(option, ARCHIVO_TERMINOS)||
			Utility::equals(option, ARCHIVO_NORMA_INFINITO)||
			Utility::equals(option, ARCHIVO_OCURRENCIA_POSICIONAL)||
			Utility::equals(option, LISTAS_INVERTIDAS));
}

CommandOption CommandParser::getOption()
{
	CommandOption enumOption;

	if (Utility::equals(option, TOMAR_TEXTO))
		enumOption = TomarTexto;
	else if (Utility::equals(option, PROCESAR_titulo))
		enumOption = ProcesarTitulo;
	else if (Utility::equals(option, PROCESAR_AUTOR))
		enumOption = ProcesarAutor;
	else if (Utility::equals(option, PROCESAR_TITULO))
		enumOption = ProcesarIdentificador;
	else if (Utility::equals(option, PROCESAR_PALABRAS))
		enumOption = ProcesarPalabras;
	else if (Utility::equals(option, LISTAR_ARCHIVOS))
		enumOption = ListarArchivos;
	else if (Utility::equals(option, OBTENER_ARCHIVO))
		enumOption = ObtenerArchivo;
	else if (Utility::equals(option, QUITAR_ARCHIVO))
		enumOption = QuitarArchivo;
	else if (Utility::equals(option,QUITAR_ARCHIVO_FECHA))
		enumOption = QuitarArchivoPorFecha;
	else if (Utility::equals(option, CONSULTAR_titulo))
		enumOption = Consultartitulo;
	else if (Utility::equals(option, CONSULTAR_AUTOR))
		enumOption = ConsultarAutor;
	else if (Utility::equals(option, CONSULTAR_TITULO))
		enumOption = ConsultarTitulo;
	else if (Utility::equals(option, CONSULTAR_PALABRAS))
		enumOption = ConsultarPalabras;
	else if (Utility::equals(option, VER_ESTRUCTURA))
	{
		if (this->exportIndexStructure)
			enumOption = VerEstructuraIndices;
		else
			enumOption = VerEstructuraArchivos;
	}
	else if (Utility::equals(option, AYUDA))
		enumOption = Ayuda;
	else if (Utility::equals(option,CONSULTAR_FECHA))
		enumOption = ConsultarFecha;
	else if (Utility::equals(option,PROCESAR_FECHA))
		enumOption = ProcesarFecha;
	return enumOption;
}

string CommandParser::getPathFile()
{
	return this->pathFile;
}

string CommandParser::getFileId()
{
	return this->fileId;
}

string CommandParser::getQueryTerm()
{
	return this->queryTerm;
}

IndexWrapper::indexItem CommandParser::getIndexId()
{
	return this->indexId;
}

Mediator::fileItem CommandParser::getIdFile()
{
	return this->IdFile;
}



