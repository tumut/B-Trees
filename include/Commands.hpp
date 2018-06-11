#ifndef _COMMANDS_HPP_INCLUDED_
#define _COMMANDS_HPP_INCLUDED_

//! Receives a CSV file and creates a database based on its contents
/*!
 * Will load the data in the provided CSV file and, one by one, upload them
 * into a database while indexing each entry by id and title.
 *
 * Each entry should be represented as a line in the CSV file with the
 * following fields, one in each column:
 *
 * | Field | Type | Description |
 * | --- | --- | --- |
 * | ID | Integer | Identifier of the article |
 * | Title | Alpha 300 | Title of the article |
 * | Year | Integer | Publication year |
 * | Authors | Alpha 1024 | List of the authors |
 * | Citations | Integer | Times the article has been cited |
 * | Update | Date time | Time of the last time the article was updated (`YYYY-MM-DD HH:mm:SS`) |
 * | Snippet | Alpha 1024 | Text summary of the article's contents |
 *
 * Details of the execution will be printed as the program executes.
 *
 * Creates tree files upon completion:
 *
 * - `db-hashfile.bin`: where the entries will be stored;
 * - `db-idindex.bin`: primary index by id;
 * - `db-titleindex.bin`: secondary index by title.
 *
 * The files will be overwritten if they already exist.
 *
 * @param filePath Path to the CSV file with entries
 */
void upload(const char* filePath);

//! Finds an entry in the hashfile based on the entry's id
/*!
 * The hashfile uses perfect hashing. Therefore, the entry's offset in the file
 * can be calculated readily from the id.
 *
 * In case the id doesn't result in a valid offset, the procedure will inform
 * you.
 *
 * @param id Id of the entry to find
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
