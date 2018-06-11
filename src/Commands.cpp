#include "Commands.hpp"

#include <cstring>
#include <iostream>

#include "IdealBTree.hpp"
#include "Entry.hpp"

// --- //

//! File path to the folder where database files will be created
#define ROOT "./"

//! Primary index data filename
#define ID_TREE_FILENAME "bd-idtree.bin"
//! Full filepath to the primary index file
#define ID_TREE_FILEPATH ROOT ID_TREE_FILENAME

//! Secondary index data filename
#define TITLE_TREE_FILENAME "bd-titletree.bin"
//! Full filepath to the secondary index file
#define TITLE_TREE_FILEPATH ROOT TITLE_TREE_FILENAME

//! Hashing file filename
#define HASHFILE_FILENAME "bd-hashfile.bin"
//! Full filepath to the hashing file
#define HASHFILE_FILEPATH ROOT HASHFILE_FILENAME

//! Block size in bytes to be used in the hashing file
#define HASHFILE_BLOCK_SIZE BLOCK_SIZE

//! Show how many entries have already been read and indexed every once in a while
#define PATIENCE_STEP 10000

// --- //

//! Helper struct to store primary indexes
struct IdIndex {
	int id; //!< Entry id
	long offset; //!< Entry offset in the hashfile
	
	//! Less-than comparator so that IdIndex can be used in BTree
	bool operator< (const IdIndex& that) const {
		return id < that.id;
	}
};

//! Less-than comparator between IdIndex::id and int
bool operator< (const IdIndex& index, int i) {
	return index.id < i;
}

//! Less-than comparator between int and IdIndex::id
bool operator< (int i, const IdIndex& index) {
	return i < index.id;
}

//! Helper struct to store secondary indexes
struct TitleIndex {
	char title[TITLE_CHAR_MAX]; //!< Entry title
	long offset; //!< Entry offset in the hashfile

	//! Less-than comparator so that TitleIndex can be used in BTree
	bool operator< (const TitleIndex& that) const {
		return std::strcmp(title, that.title) < 0;
	}
};

//! Less-than comparator between TitleIndex::title and string
bool operator< (const TitleIndex& index, const char* title) {
	return std::strcmp(index.title, title) < 0;
}

//! Less-than comparator between string and TitleIndex::title
bool operator< (const char* title, const TitleIndex& index) {
	return std::strcmp(title, index.title) < 0;
}

//! Primary index B-tree
typedef IdealBTree<IdIndex> IdBTree;

//! Secondary index B-tree
typedef IdealBTree<TitleIndex> TitleBTree;

// --- //

//! Header data for the hashfile
struct HashfileHeader {
	int blockCount;
};

//! HashfileHeader block
typedef Block<HashfileHeader, HASHFILE_BLOCK_SIZE> HashfileHeaderBlock;

//! Entry block
typedef Block<Entry, HASHFILE_BLOCK_SIZE> EntryBlock;

// --- //

//! Reads a string field from a line in the CSV file
/*!
 * Will read the quotes-enclosed value from a column in the `;`-delimited CSV
 * file and put it into a char* buffer.
 *
 * Allows the column to be left blank or as `NULL` (no quotes).
 *
 * @param field Buffer to store the read string (must be at least as big as `fieldSize`)
 * @param file File where the contents will be read
 * @param fieldSize Maximum field size in characters
 */
static void readStringField(char *field, std::FILE *file, int fieldSize) {
	char previous = ';';
	char current = std::fgetc(file);
	int index = 0;

	switch (current) {
	// Case in which the field has no characters: ;;
	case ';':
		break;
	
	// Case in which the last field has no characters, so there's only an LF line break
	case '\n':
		break;
	
	// Case in which the last field has no characters, but there's still a CRLF line break
	case '\r':
		std::fgetc(file); // '\n'
		break;
	
	// Case in which instead of a field there's only NULL
	case 'N':
		std::fgetc(file); // 'U'
		std::fgetc(file); // 'L'
		std::fgetc(file); // 'L'
		if (std::fgetc(file) == '\r') std::fgetc(file); // Skips the line break, be it LF or CRLF
		break;
	
	// Case in which there's a string in the field
	case '"':
		while (true) {
			previous = current;
			current = fgetc(file);
			
			if (previous == '"') {
				bool fieldEnded = false;
				
				// If the quotes are followed by either of these elements, then we've reached the end of the field
				switch (current) {
				case '\r':
					std::fgetc(file); // '\n'
				case ';':
				case '\n':
				case EOF:
					--index;
					fieldEnded = true;
					break;
				}
				
				if (fieldEnded) break;
			}
			
			if (current >= 0) {
				// Only insert the character in the buffer if there's still
				// space left (taking into account the '\0' end character).
				// We'll keep reading the file until the end of the column.
				if (index < fieldSize - 1) {
					field[index] = current;
				}

				++index;
			}
		}
		
		break;
	}
	
	if (index >= fieldSize) {
		index = fieldSize - 1;
	}
	
	field[index] = '\0';
}

//! Reads a whole line, field by field, from the CSV file
/*!
 * @param e Entry to read
 * @param file File with contents
 *
 * @return True if the entry was successfully read, false if we've reached the end of the file
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

//! Prints an entry field by field
/*!
 * @param e Entry to print
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
	// Phantom entry that'll be used to pad the spaces between id's
	static auto phantomEntry = []{
			EntryBlock pe;
			pe.var.valid = false;
			return pe;
		}();
	
	#ifdef DEBUG
	std::cout << "[DEBUG]\n";
	#endif
	
	std::cout << "Tamanho do bloco sendo considerado: " << BLOCK_SIZE << " bytes\n";
	std::cout << "Ordem da árvore de id's: " << IdBTree::Order << '\n';
	std::cout << "Ordem da árvore de títulos: " << TitleBTree::Order << "\n\n";
	
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
	
	HashfileHeaderBlock header;
	header.var.blockCount = 1;
	std::fwrite(reinterpret_cast<const char*>(&header), sizeof(header), 1, output);
	
	EntryBlock e;
	int lastId = -1;
	unsigned int entriesFound = 0;
	
	while (readEntry(e.var, input)) {
		if (++entriesFound % PATIENCE_STEP == 0) {
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
		
		IdIndex idPointer;
		idPointer.id = e.var.id;
		idPointer.offset = offset;
		idTree.insert(idPointer);
		
		TitleIndex titlePointer;
		std::memcpy(titlePointer.title, e.var.title, TITLE_CHAR_MAX);
		titlePointer.offset = offset;
		titleTree.insert(titlePointer);
		
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

//! Function that prints a found entry and associated data
/*!
 * Prints how many blocks were read to find it and how many blocks the file
 * currently contains as a whole
 *
 * @param e Found entry
 * @param blocksRead Blocks read
 * @param blockCount Blocks in the file
 */
static void foundEntryMessage(const Entry& e, std::size_t blocksRead, std::size_t blockCount) {
	std::cout << "Registro encontrado:\n\n";
	printEntry(e);
	std::cout << blocksRead << " bloco" << (blocksRead > 1? "s foram lidos" : " foi lido") << " pra encontrá-lo.\n";
	std::cout << "O arquivo está no momento com " << blockCount << " blocos totais." << std::endl;
}

//! Function that seeks an entry by offset in the hashfile and prints it if successful
/*!
 * In case of failure, it'll inform you
 *
 * @param hashfile File pointer to the hashfile
 * @param offset Entry offset
 * @param blocksReadSoFar Blocks read so far
 * @param blockCount Blocks in the file
 *
 * @return True in case of success, false otherwise
 */
static bool findEntryAndPrint(std::FILE *hashfile, long offset, std::size_t blocksReadSoFar, std::size_t blockCount) {
	std::cout << "Lendo o registro no offset " << offset << '\n';
	
	if (!std::fseek(hashfile, offset, SEEK_SET)) {
		Block<Entry> e;
		
		if (std::fread(reinterpret_cast<char*>(&e), sizeof(e), 1, hashfile) && e.var.valid) {
			++blocksReadSoFar; // +1 because the entry block has been read
			
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
	
	// The block read for the header doesn't count because it's not used in the
	// entry search. The header is only used to provide the total blocks in the
	// file.
	HashfileHeaderBlock header;
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
	
	IdIndex ip;
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
	
	TitleIndex tp;
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
