#include "Commands.hpp"

#include "Block.hpp"
#include "BTree.hpp"
#include "Entry.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>

// --- //

//! Caminho do local onde os arquivos do banco de dados serão criados.
/*!
  Este é um define auxiliar que é usado apenas na definição de outros defines.
  
  Por padrão, os arquivos serã criados na mesma pasta que o executável.
 */
#define FILEPATH "./"

//! Nome do arquivo que guardará os índices do arquivo de índice primário.
#define ID_TREE_FILENAME "bd-idtree.bin"
//! Caminho completo pro arquivo que guardará os índices do arquivo de índice primário.
#define ID_TREE_FILEPATH FILEPATH ID_TREE_FILENAME

//! Nome do arquivo que guardará os índices do arquivo de índice secundário.
#define TITLE_TREE_FILENAME "bd-titletree.bin"
//! Caminho completo pro arquivo que guardará os índices do arquivo de índice secundário.
#define TITLE_TREE_FILEPATH FILEPATH TITLE_TREE_FILENAME

//! Nome do arquivo de hashing que guardará os registros.
#define HASHFILE_FILENAME "bd-hashfile.bin"
//! Caminho completo pro arquivo de hashing.
#define HASHFILE_FILEPATH FILEPATH HASHFILE_FILENAME

// --- //

//! Struct auxiliar para guardar os índices primários.
struct IdPointer {
	int id; //!< Id do registro.
	long offset; //!< Offset do registro no arquivo de hashing.
	
	//! Comparador de < para o struct auxiliar, assim poderá ser usado na BTree.
	bool operator< (const IdPointer& that) const {
		return id < that.id;
	}
};

//! Struct auxiliar para guardar os índices secundários.
struct TitlePointer {
	char title[TITLE_CHAR_MAX]; //!< String com o título do registro.
	long offset; //!< Offset do registro no arquivo de hashing.
	
	//! Comparador de < para o struct auxiliar, assim poderá ser usado na BTree.
	bool operator< (const TitlePointer& that) const {
		return strcmp(title, that.title) < 0;
	}
};

//! Macro para definição da ordem máxima do nó de um BTree, baseado no tipo de dados que ele guardará.
/*!
  O cálculo é feito a partir do tamanho de BLOCK_SIZE e determina a ordem
  máxima que o nó pode ter sem ultrapassar o tamanho do bloco, considerando
  que ele estará guardando dados de um tipo cujo tamanho em bytes é T_SIZE.
  
  A fórmula do macro foi obtido a partir desta fórmula inicial:
  
  `BLOCK_SIZE = sizeof(long) + sizeof(bool) + sizeof(int) + (2 * M + 1) * sizeof(T) + (2 * M + 2) * sizeof(long)`
  
  Ele é baseado nos campos padrões da estrutura BNode, interna à classe BTree.
 */
#define BTREE_ORDER(T_SIZE) ((BLOCK_SIZE - 3 * sizeof(long) - sizeof(bool) + sizeof(int) - (T_SIZE)) / (2 * ((T_SIZE) + sizeof(long))))

//! Macro com a ordem máxima que um nó de id's pode ter sem ultrapassar o tamanho do bloco.
#define ID_ORDER BTREE_ORDER(sizeof(int))
//! Macro com a ordem máxima que um nó de títulos pode ter sem ultrapassar o tamanho do bloco.
#define TITLE_ORDER BTREE_ORDER(sizeof(char) * TITLE_CHAR_MAX)

//! Typedef de uma árvore B para índices.
typedef BTree<IdPointer, ID_ORDER> IdBTree;
//! Typedef de uma árvore B para títulos.
typedef BTree<TitlePointer, TITLE_ORDER> TitleBTree;

// --- //

//! Struct do cabeçalho do arquivo de hashing.
struct HashfileHeader {
	int blockCount;
};

// --- //

//! Lê a coluna de um registro a partir do arquivo CSV.
/*!
	Essa função é chamada para a leitura do arquivo. Nela são tratadas todas as possíveis exceções que podem ocorrer na leitura, essas exceções sao a presença no local
	incorreto de diversos símbolos descritos a seguir. 

	Os símbolos são: ; (ponto e vírgula), `\n`(contra-barra n), \r(contra-barra r), "(aspas), `NULL`.

	No caso das aspas é necessário ser verificado se elas realmente se encontram em um local inválido, ou se elas são o final de uma coluna. Para isso fazemos uma etapa de verificação
	a mais nas aspas, nessa etapa verificamos se as aspas são seguidas de `\r`, `;`, `\n`, EOF. Se ela for seguida por algum desses significa que essas aspas estão demarcando o final
	de uma coluna.

	\param field Ponteiro para o vetor de caracteres onde a coluna lida será guardada.
	\param file O arquivo sendo lido.
	\param fieldSize O tamanho máximo, em caracteres, que a string do campo pode ter.

	\author Oscar Othon
*/
static void readStringField(char *field, std::FILE *file, int fieldSize) {
	static char buffer[1024 * 2];
	
	char previous = ';';
	char current = std::fgetc(file);
	int index = 0;

	switch (current) {
	// Caso em que o campo não tem caracteres: ;;
	case ';':
		break;
	
	// Caso em que o último campo não tem caracteres, então sobra apenas uma quebra de linha (estilo LF).
	case '\n':
		break;
	
	// Caso em que o último campo não tem caracteres, mas sobra uma quebra de linha estilo CRLF.
	case '\r':
		std::fgetc(file); // '\n'
		break;
	
	// Caso em que ao invés de um campo há apenas NULL.
	case 'N':
		std::fgetc(file); // 'U'
		std::fgetc(file); // 'L'
		std::fgetc(file); // 'L'
		if (std::fgetc(file) == '\r') std::fgetc(file); // Pula a quebra de linha, seja ela LF or CRLF
		break;
	
	// Caso em que há uma string na coluna.
	case '"':
		while (true) {
			previous = current;
			current = fgetc(file);
			
			if (previous == '"') {
				bool fieldEnded = false;
				
				// Se as aspas forem seguidas desses elementos a seguir, então é porque é o final de um campo.
				switch (current) {
				case '\r':
					std::fgetc(file); // \n
				case ';':
				case '\n':
				case EOF:
					--index;
					fieldEnded = true;
					break;
				}
				
				if (fieldEnded) break;
			}
			
			if (current >= 0)
				buffer[index++] = current;
		}
		
		break;
	}
	
	if (index >= fieldSize) {
		index = fieldSize - 1;
	}
	
	buffer[index] = '\0';
	
	std::memcpy(field, buffer, index + 1);
}

//! Lê por inteiro, campo a campo, um registro do arquivo CSV.
/*!
	Recebendo o arquivo pelo parâmetro std::FILE *file, essa função percorre por completa uma linha de instância de registro no arquivo, dividindo esse artigo em seus campos de título, ano, autor,
	citações, atualização e snippet. Cada um desses campos é separado ou por meio da utilização da função readField(), ou utilizando-se fscanf().

	\param e O registro do registro a ter seus campos lidos.
	\param file O arquivo sendo lido.

	\returns Se o registro foi lido com sucesso ou não, dará falso ao chegar no final do arquivo pois não haverão mais registros para ler.

	\author Oscar Othon
*/
static bool readEntry(Entry& e, std::FILE *file) {
	int endChecker = std::fscanf(file, "\"%d\";", &e.id);
	
	if (endChecker < 0) {
		e.valid = false;
		return false;
	}

	readStringField(e.title, file, TITLE_CHAR_MAX);
	std::fscanf(file, "\"%d\";", &e.year);
	readStringField(e.authors, file, AUTHORS_CHAR_MAX);
	std::fscanf(file, "\"%d\";", &e.citations);
	readStringField(e.updateTimestamp, file, TIMESTAMP_CHAR_MAX);
	readStringField(e.snippet, file, SNIPPET_CHAR_MAX);
	
	e.valid = true;
	return true;
}

//! Imprime um registro campo por campo.
/*!	
	\param e O registro a ser impresso.

	\author Oscar Othon
 */
static void printEntry(const Entry& e) {
	std::cout << "id        : " << e.id << '\n';
	std::cout << "title     : " << e.title << '\n';
	std::cout << "year      : " << e.year << '\n';
	std::cout << "authors   : " << e.authors << '\n';
	std::cout << "citations : " << e.citations << '\n';
	std::cout << "timestamp : " << e.updateTimestamp << '\n';
	std::cout << "snippet   : " << e.snippet << '\n' << std::endl;
}

// ---

void upload(const char* filePath) {
	// Registro "fantasma" que será usado para preencher os espaços entre os id's
	static auto phantomEntry = []{
			Block<Entry> pe;
			pe.var.valid = false;
			return pe;
		}();
	
	#ifdef DEBUG
	std::cout << "[DEBUG]\n";
	#endif
	
	std::cout << "Tamanho do bloco sendo considerado: " << BLOCK_SIZE << " bytes\n";
	std::cout << "Ordem da árvore de id's: " << ID_ORDER << '\n';
	std::cout << "Ordem da árvore de títulos: " << TITLE_ORDER << "\n\n";
	
	std::cout << "Abrindo os arquivos...\n\n";

	std::FILE *input = std::fopen(filePath, "rb");
	if (!input) {
		std::cout << "Não foi possível abrir o arquivo de leitura.\n";
		std::cout << "Caminho: \"" << filePath << "\"\n";
		std::cout << "Abortando." << std::endl;
		return;
	}
	
	IdBTree idTree;
	if (!idTree.create(ID_TREE_FILEPATH)) {
		std::cout << "Falha na criação do arquivo para a árvore B de id's.\n";
		std::cout << "Caminho: \"" << ID_TREE_FILEPATH << "\"\n";
		std::cout << "Abortando." << std::endl;
		return;
	}
	
	std::cout << "Arquivo para a árvore B de id's criado em " << ID_TREE_FILEPATH << '\n';
	
	TitleBTree titleTree;
	if (!titleTree.create(TITLE_TREE_FILEPATH)) {
		std::cout << "Falha na criação do arquivo para a árvore B de títulos.\n";
		std::cout << "Caminho: \"" << TITLE_TREE_FILEPATH << "\"\n";
		std::cout << "Abortando." << std::endl;
		return;
	}
	
	std::cout << "Arquivo para a árvore B de títulos criado em " << TITLE_TREE_FILEPATH << '\n';
	
	std::FILE *output = std::fopen(HASHFILE_FILEPATH, "wb");
	if (!output) {
		std::cout << "Não foi possível criar o arquivo de saída.\n";
		std::cout << "Caminho: \"" << HASHFILE_FILEPATH << "\"\n";
		std::cout << "Abortando." << std::endl;
		return;
	}
	
	std::cout << "Arquivo de hashing criado em " << HASHFILE_FILEPATH << "\n\n";
	
	std::cout << "Iniciando a atualização...\n\n";
	
	Block<HashfileHeader> header;
	header.var.blockCount = 1;
	std::fwrite(reinterpret_cast<const char*>(&header), sizeof(header), 1, output);
	
	Block<Entry> e;
	int lastId = -1;
	unsigned int entriesFound = 0;
	
	while (readEntry(e.var, input)) {
		if (++entriesFound % 100000 == 0) {
			std::cout << entriesFound << " registros já foram lidos, paciência.\n";
		}
		
		int idDifference = e.var.id - lastId - 1;
		
		for (int i = 0; i < idDifference; ++i) {
			std::fwrite(reinterpret_cast<const char*>(&phantomEntry), sizeof(phantomEntry), 1, output);
		}
		
		header.var.blockCount += idDifference;
		
		auto offset = std::ftell(output);
		std::fwrite(reinterpret_cast<const char*>(&e), sizeof(e), 1, output);
		++header.var.blockCount;
		
		IdPointer idPointer;
		idPointer.id = e.var.id;
		idPointer.offset = offset;
		idTree.insert(idPointer);
		
		TitlePointer titlePointer;
		std::memcpy(titlePointer.title, e.var.title, TITLE_CHAR_MAX);
		titlePointer.offset = offset;
		titleTree.insert(titlePointer);
		
		// titleTree.traverse(true);
		
		lastId = e.var.id;
	}
	
	titleTree.finishInsertions();
	idTree.finishInsertions();
	
	std::fseek(output, 0, SEEK_SET);
	std::fwrite(reinterpret_cast<const char*>(&header), sizeof(header), 1, output);
	
	std::fclose(output);
	std::fclose(input);
	
	auto idStats = idTree.getStatistics();
	auto titleStats = titleTree.getStatistics();
	
	if (entriesFound >= 1000) std::cout << '\n';
	
	std::cout << "Atualização dos arquivos finalizada.\n";
	std::cout << entriesFound << " registro(s) lido(s) no total.\n\n";
	
	std::cout << "Arquivo de hashing:           " << header.var.blockCount << " blocos.\n";
	std::cout << "Arquivo de índice primário:   " << idStats.blocksCreated << " blocos.\n";
	std::cout << "Arquivo de índice secundário: " << titleStats.blocksCreated << " blocos." << std::endl;
}

//! Função que mostra que um registro foi encontrado, quantos blocos foram lidos para encontrá-lo e quantos blocos o arquivo possui no momento.
/*!

	\param e Um registro do arquivo.
	\param blocksRead O número de blocos lidos até chegar no registro.
	\param blockCount A contagem do número total de blocos no arquivo.

	\author Oscar Othon
*/
static void foundEntryMessage(const Entry& e, std::size_t blocksRead, std::size_t blockCount) {
	std::cout << "Registro encontrado:\n\n";
	printEntry(e);
	std::cout << blocksRead << " bloco" << (blocksRead > 1? "s foram lidos" : " foi lido") << " pra encontrá-lo.\n";
	std::cout << "O arquivo está no momento com " << blockCount << " blocos totais." << std::endl;
}

//! Função que acha um registro no arquivo e printa se a busca foi completa ou não, ou se ocorreu algum erro de leitura.
/*!

	Essa função recebe o arquivo já aberto, se dirige até o offset e, então, lê o registro.

	\param *hashfile Ponteiro do arquivo pro arquivo de hashing.
	\param	offset O offset do registro no arquivo.
	\param	blocksReadSoFar Quantos blocos foram lidos até o momento.
	\param	blockCount A contagem do número total de blocos no arquivo.

	\author Oscar Othon
*/
static bool findEntryAndPrint(std::FILE *hashfile, long offset, std::size_t blocksReadSoFar, std::size_t blockCount) {
	std::cout << "Lendo o registro no offset " << offset << '\n';
	
	if (!std::fseek(hashfile, offset, SEEK_SET)) {
		Block<Entry> e;
		
		if (std::fread(reinterpret_cast<char*>(&e), sizeof(e), 1, hashfile) && e.var.valid) {
			++blocksReadSoFar; // +1 porque o bloco do registro foi lido.
			
			foundEntryMessage(e.var, blocksReadSoFar, blockCount);
			return true;
		}
		else {
			std::cout << "Falha na leitura. ";
		}
	}
	else {
		std::cout << "Falha no seek (" << offset << "). ";
	}
	
	return false;
}

void findrec(long id) {
	std::FILE *hashfile = std::fopen(HASHFILE_FILEPATH, "rb");
	
	if (!hashfile) {
		std::cout << "Sem arquivo de hashing para a consulta." << std::endl;
		return;
	}
	
	long offset = BLOCK_SIZE /* header */ + BLOCK_SIZE * id;
	
	// A leitura deste bloco de cabeçalho não é contada pois ela não
	// influencia na busca pelo registro. O cabeçalho é usado apenas
	// para saber a quantidade de blocos no arquivo.
	Block<HashfileHeader> header;
	std::fseek(hashfile, 0, SEEK_SET);
	std::fread(reinterpret_cast<char*>(&header), sizeof(header), 1, hashfile);
	
	if (!findEntryAndPrint(hashfile, offset, 0, header.var.blockCount)) {
		std::cout << "O registro com o id " << id << " não foi encontrado." << std::endl;
	}
	
	std::fclose(hashfile);
}

void seek1(long id) {
	std::FILE *hashfile = std::fopen(HASHFILE_FILEPATH, "rb");
	
	if (!hashfile) {
		std::cout << "Sem arquivo de hashing para a consulta." << std::endl;
		return;
	}
	
	IdBTree tree;
	
	if (!tree.load(ID_TREE_FILEPATH)) {
		std::cout << "Sem arquivo de índice primário para a consulta." << std::endl;
		return;
	}
	
	IdPointer ip;
	ip.id = id;
	ip.offset = -1;
	
	auto found = tree.seek(ip);
	
	if (found) {
		auto stats = tree.getStatistics(true);
		
		if (!findEntryAndPrint(hashfile, found->offset, stats.blocksRead, stats.blocksInDisk)) {
			std::cout << "O registro com o id " << id << " (offset=" << found->offset
				<< ") não foi encontrado no arquivo de hashing." << std::endl;
		}
	}
	else {
		std::cout << "O registro com o id " << id
			<< " não foi encontrado no arquivo de índice primário." << std::endl;
	}
	
	std::fclose(hashfile);
}

void seek2(const char* title) {
	std::FILE *hashfile = std::fopen(HASHFILE_FILEPATH, "rb");
	
	if (!hashfile) {
		std::cout << "Sem arquivo de hashing para a consulta." << std::endl;
		return;
	}
	
	TitleBTree tree;
	
	if (!tree.load(TITLE_TREE_FILEPATH)) {
		std::cout << "Sem arquivo de índice secundário para a consulta." << std::endl;
		return;
	}
	
	TitlePointer tp;
	std::strcpy(tp.title, title);
	tp.offset = -1;
	
	auto found = tree.seek(tp);
	
	if (found) {
		auto stats = tree.getStatistics(true);
		
		if (!findEntryAndPrint(hashfile, found->offset, stats.blocksRead, stats.blocksInDisk)) {
			std::cout << "O registro com o título \"" << title
				<< "\" (offset=" << found->offset
				<< ") não foi encontrado no arquivo de hashing." << std::endl;
		}
	}
	else {
		std::cout << "O registro com o título \"" << title
			<< "\" não foi encontrado no arquivo de índice secundário." << std::endl;
	}
	
	std::fclose(hashfile);
}
