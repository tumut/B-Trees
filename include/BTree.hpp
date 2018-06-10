#ifndef _BTREE_HPP_INCLUDED_
#define _BTREE_HPP_INCLUDED_

#include "Block.hpp"

#include <cstdio>
#include <memory>

//! B-tree class.
/*!
 * Used to store indices in files.
 *
 * Use the method BTree::create before inserting values. Use BTree::load
 * before reading them.
 *
 * After you've finished writing, don't forget to call BTree::finishInsertions
 * to update header data.
 *
 * Example usage:
 * \code
 * BTree<int, 2> tree;
 *
 * // Insert data
 * tree.create("filename.bin");
 * tree.insert(1);
 * tree.finishInsertions();
 *
 * // Read data
 * tree.load("filename.bin");
 * int *x = tree.seek(1);
 * if (x) f(*x); // Do something to x if found
 * \endcode
 *
 * @tparam T Data type that will be stored. Must be "less-than comparable" and
 * a POD (Plain Old Data type) so that it can be serialized in the written
 * file.
 *
 * @tparam M Tree order. Each node will store up to 2M values and have up to
 * 2M + 1 children.
 */
template<typename T, std::size_t M>
class BTree {
public:
	//! Default constructor.
	/*!
	 * Initializes the statistics with 0 values and makes the file pointer
	 * point to null.
	 */
	BTree();
	
	//! Default destructor.
	/*! Closes the file if it's open. */
	~BTree();
	
	//! Initializes the B-tree for writing.
	/*!
	 * Opens the file in "wb+" mode. Must be called before inserting values in
	 * the tree. Also allows reading through BTree::seek.
	 *
	 * If there's already a file in the filepath, it'll be overwritten.
	 *
	 * The new file will be created in the provided filepath with a header
	 * and an empty root node.
	 *
	 * @param filepath Path to the file where the tree data will be written.
	 *
	 * @return True if the file was created successfully.
	*/
	bool create(const char* filepath);
	
	//! Initializes the B-tree for reading only.
	/*!
	 * Opens the file in "rb" mode. Must be before reading values form a file
	 * where values have already been inserted. Insertions won't be possible.
	 *
	 * The file must have been previously created by a BTree which successfully
	 * called BTree::create and BTree::finishInsertions.
	 *
	 * @param filepath Path to the file where tree data can be found.
	 *
	 * @return If it was possible to open the file in filepath.
	 */
	bool load(const char* filepath);
	
	//! Inserts a value in the tree.
	/*!
	 * @param value The value to be inserted.
	 */
	void insert(const T& value);
	
	//! Busca um dado que seja equivalente ao dado fornecido.
	/*!
	  Não há garantias que o dado realmente será buscado. Caso não seja
	  encontrado, a função retorna um ponteiro nulo.
	  
	  \param key O dado que se está buscando.
	  
	  \return Ponteiro com o dado ou nulo.
	  
	  \author Timóteo Fonseca
	 */
	std::unique_ptr<T> seek(const T& key);
	
	//! Estrutura usada pela árvore para guardar métricas de sua execução.
	/*! \author Timóteo Fonseca */
	struct Statistics {
		unsigned int blocksRead; //!< Quantidade de blocos lidos.
		unsigned int blocksCreated; //!< Quantidade de blocos criados.
		unsigned int blocksInDisk; //!< Quantidade de blocos que já foram inseridos em disco.
	};
	
	//! Retorna as estatísticas da árvore até então.
	/*!
	  A inclusão da quantidade de blocos no arquivo, nas estatísticas, é
	  opcional pois a árvore vai ter que fazer uma leitura do arquivo para
	  poder obter este dado, o que é uma operação custosa.
	  
	  \param includeFileBlockCount Se deve incluir (true) ou não (false) a
	   quantidade de blocos do arquivo nas estatísticas.
	  
	  \return Estatísticas.
	  
	  \author Timóteo Fonseca
	 */
	Statistics getStatistics(bool includeFileBlockCount = false) const;
	
	//! Atribui valor 0 a todos os campos das estatísticas atuais.
	/*!
	  \author Timóteo Fonseca
	  */
	void resetStatistics();
	
	//! Atualiza o cabeçalho com o total de blocos no arquivo.
	/*!
	  Muito importante de se usar no final da utilização de uma árvore após
	  ela ser inicializada com create().
	  
	  \author Timóteo Fonseca
	 */
	void finishInsertions();
	
private:
	//! Dados do cabeçalho do arquivo.
	struct FileHeader {
		long rootAddress;
		unsigned int blockCount;
	};
	
	//! Nó da árvore B.
	struct BNode {
		//! Endereço no disco.
		/*!
		  O ideal é que o nó, antes de ser escrito no registro pela primeira
		  vez, esteja com o offset de valor -1. Após a primeira escrita, o
		  offset passa a realmente possuir seu endereço no disco.
		 */
		long offset;
		bool isLeaf; //!< Booleano indicando se o nó é folha (true) ou nó interno (false).
		std::size_t size; //!< Quantidade de dados dentro do nó, no momento.
		
		//! Os dados do nó.
		/*!
		  Na prática, um nó só costuma utilizar as 2M primeiras posições deste
		  vetor. O espaço adicional de 1 dado é usado temporariamente para
		  lidar com overflows.
		 */
		T values[2 * M + 1];
		
		//! Os apontadores de nó.
		/*!
		  Na prática, um nó só costuma utilizar as 2M+1 primeiras posições deste
		  vetor. O espaço adicional de 1 dado é usado temporariamente para
		  lidar com overflows.
		 */
		long children[2 * M + 2];
		
		//! Inicializa o nó.
		/*!
		  O valor do campo offset do nó não é fornecido: ele é sempre
		  inicializado como -1, para permitir que se saiba se ele já foi
		  escrito no arquivo ou não.
		  
		  Se um nó não for lido através de um readFromDisk da BTree, é
		  obrigatório que esse método seja chamado, pois ele funciona como um
		  construtor do BNode.
		  
		  A inicialização foi implementada como um método ao invés de um
		  construtor pois é necessário que o BNode seja uma classe de tipo POD
		  (Plain Old Data type) para poder ser lido e escrito no arquivo, além
		  de para também poder ser usado como argumento de template para o
		  Block<T>.
		  
		  \param isLeaf Se o nó vai ser inicializado como folha (true) ou não (false).
		  \param keysCount Quantidade inicial de dados no nó.
		  
		  \autor Timóteo Fonseca
		 */
		void initialize(bool isLeaf = true, int keysCount = 0);
		
		//! Verifica se o nó está em sua capacidade máxima (acima de 2M dados).
		/*!
		  \author Timóteo Fonseca
		 */
		bool isFull() const;
		
		//! Busca um dado que seja equivalente ao dado fornecido.
		/*!
		  Realiza a busca propriamente dita pelo dado, usando a instância de
		  tree passado por parâmetro para poder realizar leituras no arquivo.
		  
		  Retorna um ponteiro nulo se não encontrar o dado.
		  
		  \return Ponteiro com o dado ou nulo.
		  
		  \author Timóteo Fonseca
		 */
		std::unique_ptr<T> seek(const T& key, BTree& tree) const;
	};
	
	std::FILE *m_file; //!< Ponteiro de arquivo para o arquivo em que se encontram os dados da árvore.
	Block<BNode> m_root; //!< Nó raiz da árvore.
	mutable Statistics m_stats; //!< Onde a BTree guarda suas estatísticas de leitura e escrita de blocos.
	
	//! Retorna um bloco com o nó lido no offset fornecido.
	/*!
	  O resultado da função é indefinido se for fornecido um offset inválido.
	  Presume-se que apenas offsets válidos, com arquivos válidos, serão
	  fornecidos para esta função.
	  
	  Esta função incrementa o valor de blocos lidos, nas estatísticas.
	  
	  \param offset O offset do nó no arquivo.
	  
	  \return Um bloco com o nó no offset.
	  
	  \author Timóteo Fonseca
	 */
	Block<BNode> readFromDisk(long offset);
	
	//! Atualiza em disco o nó fornecido.
	/*!
	  Caso o nó ainda não tenha sido escrito em disco, ou seja, possua offset
	  inválido (-1), o nó é concatenado ao fim do arquivo e seu offset atualizado
	  com sua nova posição.
	  
	  Esta função incrementa o valor de blocos criados, nas estatísticas, no
	  caso descrito anteriormente.
	  
	  Caso o nó já tenha sido escrito, ou seja, possua offset válido, o nó
	  é simplesmente atualizado.
	  
	  \param node O nó a ser escrito.
	  
	  \author Timóteo Fonseca
	 */
	void writeToDisk(Block<BNode>& node);
	
	//! Lê em disco o cabeçalho do arquivo da árvore.
	/*!
	  Esta função tem retorno indefinido se chamada enquanto a árvore
	  ainda não estiver sido inicializada e o arquivo não estiver aberto.
	  Presume-se que ela sempre será chamada em condições satisfatórias.
	  
	  Esta função incrementa a quantidade de blocos lidos nas estatísticas.
	  
	  \return O bloco com o cabeçalho do arquivo.
	  
	  \author Timóteo Fonseca
	 */
	Block<FileHeader> readHeader() const;
	
	//! Atualiza o cabeçalho do arquivo da arvore.
	/*!
	  \param header Bloco com instância de cabeçalho possuindo os novos valores
	   do cabeçalho do arquivo. 
	  
	  \author Timóteo Fonseca
	 */
	void writeHeader(const Block<FileHeader>& header);
	
	//! Registro auxiliar com o resultado de um overflow de inserção.
	struct OverflowResult {
		T middle; //!< O dado que, após o split de nós, é o valor do meio e deverá ser inserido no nó pai.
		long rightNode; //!< Offset de nó que vai precisar estar no apontador à direita de onde `middle` for inserido, no nó pai.
	};
	
	//! Método interno de inserção na árvore.
	/*!
	  Caso o nó seja uma folha, ele busca o apontador de nó para o nó em que o
	  dado deve ser inserido e tenta novamente de forma recursiva. Caso não seja
	  uma folha, ele faz a inserção normalmente.
	  
	  Em caso de overflow, a função irá realizar um split no nó fornecido por
	  parâmetro e irá retornar dados para o nó pai lidar com o overflow. Ele irá
	  tentar inserir novamente, dessa vez em si mesmo.
	  
	  O valor de rightNodeOffset só será diferente de -1 se a função estiver
	  lidando com um overflow previamente encontrado. No caso, node será um nó
	  pai que precisa tentar inserir o dado que restou no último split.
	  
	  \param node O nó em que o dado está sendo inserido.
	  \param key O dado a ser inserido.
	  \param rightNodeOffset O offset do nó à direita do dado a ser inserido; caso seja -1, ele não existe.
	  
	  \return Dados de overflow caso tenha houvido um, ou nulo.
	  
	  \author Timóteo Fonseca
	 */
	std::unique_ptr<OverflowResult> insert(Block<BNode>& node, T key, long rightNodeOffset = -1);
};

#include "BTree.inl"

#endif // _BTREE_HPP_INCLUDED_
