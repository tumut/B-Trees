#ifndef _COMMANDS_HPP_INCLUDED_
#define _COMMANDS_HPP_INCLUDED_


//! Recebe um arquivo CSV e cria uma base de dados baseado nele.
/*!
	Faz a carga inicial da massa de testes para seu banco de dados e cria um arquivo de dados organizado por hashing, um arquivo de índice primário
	usando B-Tree e outro arquivo de índice de índice secundário usando B-Tree.
	
	A função upload faz várias verificações antes de começar a leitura do arquivo. Os detalhes são exibidos em tela. Ela precisa verificar se
	é possível criar cada um dos arquivos necessários no local do executável, e apenas caso tudo esteja correto ela procede à leitura dos registros.
	
	No fim de uma execução bem sucedida o programa terá três arquivos gerados, todos com extensão `.bin`. Um deles será o arquivo de hashing, o
	outro o arquivo de índices primários, e por último o de índices secundários.
	
	É esperado que essa função seja chamada antes de todas as demais.
	
	Apenas arquivos em formato CSV serão aceitos.
	
	\param filePath Caminho pro arquivo CSV a ser lido.
	
	\author Oscar Othon
*/
void upload(const char* filePath);


//! Busca no arquivo de hashing um registro a partir de seu ID.
/*!
	O arquivo de hashing usa o hashing perfeito, então a função apenas calcula o offset do arquivo baseado em seu ID e faz a leitura do bloco em que
	ele se encontra.
	
	Caso não haja nenhum registro válido na posição encontrada através do ID, a função informará.

	\param id O ID do registro a ser buscado.

	\author Oscar Othon
*/
void findrec(long id);

//! Busca um registro a partir de seu ID usando o arquivo de índices primário.
/*!
	O seek1 usa o arquivo de índices primário, organizado em índices de uma árvore-B, para encontrar o registro buscado.
	
	Caso ele não seja encontrado, a função informa.
	
	A função costuma sempre precisar ler mais blocos do que a função findrec, porque o arquivo de hashing perfeito tem
	acesso em O(1).

	\param id O ID do registro a ser buscado.

	\author Oscar Othon
*/
void seek1(long id);

//! Busca um registro a partir de seu título usando o arquivo de índices secundário.
/*!
	Já que o título não é uma chave, e sim um campo qualquer, não existe ordenação entre eles no arquivo de hashing. Então é necessário utilizarmos uma árvore-B
    para poder fazermos dessa busca algo mais plausível em questão de tempo. Sem a árvore teríamos que fazer uma busca linear pelo arquivo de hashing.

    Caso o registro não possa ser encontrado, a função informará nos resultados.
	
	\param title Título pelo qual o registro será buscado.

	\author Oscar Othon
*/
void seek2(const char* title);

#endif
