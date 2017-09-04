#ifndef _COMMANDS_HPP_INCLUDED_
#define _COMMANDS_HPP_INCLUDED_


//! 
/*!

	\author Oscar Othon
*/
void upload(const char* filePath);


//! Função que lê um registro e mostra se ele foi ou não encontrado.
/*!
	Essa função age de maneira diferente da findEntryAndPrint(), pois ela tem que abrir o arquivo e calcular o offset. Porém , essa função consegue
	encontrar um regisstro em somente uma leitura de bloco. O arquivo é tratado como se fosse um vetor, e a busca e feita de maneira que é possível 
	se encontrar um registro em um tempo O(1).

	\param id O ID onde se é buscado o registro.

	\author Oscar Othon
*/
void findrec(long id);

//! Devolve o registro com ID igual ao informado.
/*!
	O seek1() utiliza uma árvore-B para buscar pelo índice, evitando casos em que o arquivo de hashing tem buckets com muitos elementos já que nesses casos 
	a função teria que fazer uma busca linear pelo bucket, o que não seria viável.

	A função mostra caso não se consiga achar um arquivo de hashing para a consulta, ou um arquivo de índice primário, ou o próprio registro de acordo om o ID
	informado. Ele se utiliza do findEntryAndPrint() para encontrar um registro no arquivo de hashing.

	\param id O ID onde se é buscado o registro.

	\author Oscar Othon
*/
void seek1(long id);

//! Função que lê um registro a partir de um título informado.
/*!
	Já que o título não é uma chave, e sim um campo qualquer, não existe ordenação entre eles no arquivo de hashing. Então é necessário utilizarmos uma árvore-B
    para poder fazermos dessa busca algo mais plausível em questão de tempo. Já que sem a árvore teríamos que fazer uma busca linear.

    A função mostra caso não se consiga achar um arquivo de hashing para a consulta, ou um arquivo de índice secundário, ou o próprio registro de acordo om o ID
	informado. Ele se utiliza do findEntryAndPrint() para encontrar um registro no arquivo de hashing.
	
	\param title Título pelo qual o registro será buscado.

	\author Oscar Othon
*/
void seek2(const char* title);

#endif
