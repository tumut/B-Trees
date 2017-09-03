#ifndef _BTREE_HPP_INCLUDED_
#define _BTREE_HPP_INCLUDED_

#include "Block.hpp"

#include <cstdio>
#include <memory>

//! Classe de árvore B.
/*!
  Classe de árvore B multi-uso usada para guardar índices e títulos no trabalho.
  
  O parâmetro de template TKey determina o tipo de dado que será guardado nos nós
  da árvore. TKey deve ser um tipo POD (Plain Old Data type) para poder ser
  sequenciado no arquivo quando o nó for escrito.
  
  O parâmetro de template M determina a ordem da árvore. Em condições normais,
  um nó pode ter no máximo 2M dados e 2M+1 apontadores de nó. Em estado de
  overflow, ele pode temporariamente ter 2M+1 dados e 2M+2 apontadores de nó.
  
  Antes de usar uma BTree deve sempre se chamar o método load (em caso de apenas
  leitura) ou create (em caso de escrita de um novo arquivo).
  
  No fim da inserção, o método finishInsertions() deve ser chamado para a
  quantidade de blocos do arquivo ser atualizada no cabeçalho.
  
  Exemplo de uso:
  \code
  BTree<int, 2> arvore;
  arvore.create("nomedoarquivo.bin");
  arvore.insert(1);
  arvore.finishInsertions();
  
  arvore.load("nomedoarquivo.bin");
  auto valor = arvore.seek(1);
  if (valor) f(*valor); // Fazer algo com o valor, se encontrado.
  \endcode
 */
template<class TKey, std::size_t M>
class BTree {
public:
	//! Construtor padrão.
	/*!
	  Inicializa com valores 0 as estatísticas e nulifica o ponteiro
	  de arquivo da BTree.
	  
	  \author Timóteo Fonseca
	 */
	BTree();
	
	//! Destrutor padrão
	/*! Fecha o arquivo se ele estiver aberto. */
	~BTree();
	
	//! Método de inicialização da árvore, para escrita.
	/*!
	  Deve sempre ser chamado antes de começar a inserção na árvore, mas também
	  permite a leitura.
	  
	  Se já houver um arquivo em filepath, ele será sobreescrito.
	  
	  O arquivo novo será criado com um cabeçalho e um nó raiz vazio.
	  
	  \param filepath Caminho do arquivo onde os dados da árvore serão escritos.
	  
	  \return Se foi possível criar o arquivo em filepath.
	  
	  \author Timóteo Fonseca
	 */
	bool create(const char* filepath);
	
	//! Método de inicialização da árvore, para leitura.
	/*!
	  Deve sempre ser chamado antes de começar a leitura da árvore. Inserções
	  não serão possíveis.
	  
	  O arquivo deve existir previamente, de preferência criado por outra
	  execução da BTree após uma chamada de create().
	  
	  \param filepath Caminho pro arquivo onde se encontram os dados da árvore.
	  
	  \return Se foi possível abrir o arquivo em filepath.
	  
	  \author Timóteo Fonseca
	 */
	bool load(const char* filepath);
	
	//! Insere um dado dentro da árvore.
	/*!
	  \param key O dado a ser inserido.
	  
	  \author Timóteo Fonseca
	 */
	void insert(const TKey& key);
	
	//! Busca um dado que seja equivalente ao dado fornecido.
	/*!
	  Não há garantias que o dado realmente será buscado. Caso não seja
	  encontrado, a função retorna um ponteiro nulo.
	  
	  \param key O dado que se está buscando.
	  
	  \return Ponteiro com o dado ou nulo.
	  
	  \author Timóteo Fonseca
	 */
	std::unique_ptr<TKey> seek(const TKey& key);
	
	//! Estrutura usada pela árvore para guardar métricas de sua execução.
	/*! \author Timóteo Fonseca */
	struct Statistics {
		unsigned int blocksRead; //!< Quantidade de blocos lidos.
		unsigned int blocksCreated; //!< Quantidade de blocos criados.
		unsigned int blocksInDisk; //!< Quantidade de blocos que já foram inseridos em disco.
	};
	
	//! Retorna as estatísticas da árvore até então.
	/*!
	  \return Estatísticas.
	  \author Timóteo Fonseca
	 */
	Statistics getStatistics(bool includeFileBlockCount = false) const;
	
	//! Atribui valor 0 a todos os campos das estatísticas atuais.
	/*! \author Timóteo Fonseca */
	void resetStatistics();
	
	//! Atualiza o cabeçalho com o total de blocos no arquivo.
	/*!
	  Muito importante de se usar no final da utilização de uma árvore após
	  ela ser inicializada com create().
	  
	  \author Timóteo Fonseca
	 */
	void finishInsertions();
	
private:
	struct FileHeader {
		long rootAddress;
		unsigned int blockCount;
	};
	
	struct BNode {
		long offset; // Address on disk
		bool isLeaf;
		std::size_t keysCount;
		
		TKey keys[2 * M + 1]; // +1 extra space for overflow
		long children[2 * M + 2]; // +1 extra space for overflow
		
		// Sets an invalid value for offset;
		// Is a method instead of a constructor for BNode to be usable in 
		// the Block union (union requires only POD members).
		void initialize(bool isLeaf = true, int keysCount = 0);
		
		bool isFull() const;
		
		std::unique_ptr<TKey> seek(const TKey& key, BTree& tree) const;
	};
	
	std::FILE *m_file;
	Block<BNode> m_root;
	Statistics m_stats;
	
	Block<BNode> readFromDisk(long offset);
	void writeToDisk(Block<BNode>& node); // Updates the node's offset if not yet defined
	
	Block<FileHeader> readHeader() const;
	void writeHeader(const Block<FileHeader>& header);
	
	struct OverflowResult {
		TKey middle;
		long rightNode;
	};
	
	std::unique_ptr<OverflowResult> insert(Block<BNode>& node, TKey key, long rightNodeOffset = -1);
};

#include "BTree.inl"

#endif // _BTREE_HPP_INCLUDED_
