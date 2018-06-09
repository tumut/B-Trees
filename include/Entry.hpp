#ifndef _ENTRY_HPP_INCLUDED_
#define _ENTRY_HPP_INCLUDED_

//! Tamanho máximo da cadeia de caracteres do título.
#define TITLE_CHAR_MAX 300
//! Tamanho máximo da cadeia de caracteres dos nomes de autores.
#define AUTHORS_CHAR_MAX 1024
//! Tamanho máximo da cadeia de caracteres do timestamp.
#define TIMESTAMP_CHAR_MAX 20
//! Tamanho máximo da cadeia de caracteres do resumo.
#define SNIPPET_CHAR_MAX 1024

//! Registro básico do arquivo.
struct Entry {
	bool valid; //!< Se o registro constitui um registro válido no arquivo.
	int id; //!< Identificador do registro.
	char title[TITLE_CHAR_MAX]; //!< Título do livro.
	int year; //!< Ano de publicação do livro.
	char authors[AUTHORS_CHAR_MAX]; //!< Autores do livro.
	int citations; //!< Quantidade de citações do livro.
	char updateTimestamp[TIMESTAMP_CHAR_MAX]; //!< Timestamp de atualização do livro.
	char snippet[SNIPPET_CHAR_MAX]; //!< Resumo do livro.
};

#endif // _ENTRY_HPP_INCLUDED_
