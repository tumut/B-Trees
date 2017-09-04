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
		std::size_t keysCount; //!< Quantidade de dados dentro do nó, no momento.
		
		//! Os dados do nó.
		/*!
		  Na prática, um nó só costuma utilizar as 2M primeiras posições deste
		  vetor. O espaço adicional de 1 dado é usado temporariamente para
		  lidar com overflows.
		 */
		TKey keys[2 * M + 1];
		
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
		bool isFull() const;
		
		//! Busca um dado que seja equivalente ao dado fornecido.
		/*!
		  Realiza a busca propriamente dita pelo dado, usando a instância de
		  tree passado por parâmetro para poder realizar leituras no arquivo.
		  
		  Retorna um ponteiro nulo se não encontrar o dado.
		  
		  \return Ponteiro com o dado ou nulo.
		  
		  \author Timóteo Fonseca
		 */
		std::unique_ptr<TKey> seek(const TKey& key, BTree& tree) const;
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
	void writeToDisk(Block<BNode>& node); // Updates the node's offset if not yet defined
	
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
		TKey middle; //!< O dado que, após o split de nós, é o valor do meio e deverá ser inserido no nó pai.
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
	std::unique_ptr<OverflowResult> insert(Block<BNode>& node, TKey key, long rightNodeOffset = -1);
};

#include "BTree.inl"

#endif // _BTREE_HPP_INCLUDED_
