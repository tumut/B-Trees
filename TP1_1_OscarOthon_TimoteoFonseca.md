# Documentação do Trabalho Prático 1

* Timóteo Fonseca _21553687_
* Oscar Othon _21551563_

# Arquivos de dados e índices

Eis os formatos dos arquivos gerados pelo programa. Todos se baseiam na unidade
de `bloco`, que neste programa foi definido como consistindo em um bloco de
4096 bytes representando um setor da unidade de armazenamento de dados.

## Arquivo de hashing

Começa sempre com um bloco contendo o cabeçalho do arquivo. Neste cabeçalho
contém o tamanho do arquivo em blocos, contando o cabeçalho.

O arquivo usa a técnica do hashing perfeito, em que existe uma posição pré-definida
para cada id de registro. No caso, cada posição é um bloco reservado para este
registro. É possível que um bloco não esteja ocupado - por exemplo, havendo um registro
de id 1 e outro de 3, mas nenhum de id 2, a posição para o bloco de id 2 continuará
existindo entre eles mesmo não estando ocupada. Os registros têm um campo que
indicam se estão válidos ou não. Se um registro for lido de uma posição não ocupada,
ele virá com o campo inválido.

## Arquivos de índice

Começam sempre com um bloco contendo o cabeçalho do arquivo. Neste cabeçalho
contém o endereço da raiz da árvore e a quantidade de blocos no arquivo, contando
com o cabeçalho.

Cada bloco subsequente representa um nó da árvore.

# Uso do programa

Todas as funções, `upload`, `findrec`, `seek1` e `seek2` são acessadas de um
único programa compilado. Portanto, há um único programa que usa todas as
dependências.

Para mais informações sobre o uso, consultar a documentação da função `main`.

# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`define `[`BLOCK_SIZE`](#_block_8hpp_1ad51ded0bbd705f02f73fc60c0b721ced)            | Definição do tamanho do Bloco.
`define `[`FILEPATH`](#_commands_8cpp_1a70535ccc618237a0c7fce8317c55628e)            | Caminho do local onde os arquivos do banco de dados serão criados.
`define `[`ID_TREE_FILENAME`](#_commands_8cpp_1a030abb644c76a5ccc3ac67269fd89290)            | Nome do arquivo que guardará os índices do arquivo de índice primário.
`define `[`ID_TREE_FILEPATH`](#_commands_8cpp_1ad74aa5691bd23a429f51e7b629c24c78)            | Caminho completo pro arquivo que guardará os índices do arquivo de índice primário.
`define `[`TITLE_TREE_FILENAME`](#_commands_8cpp_1a62caa4daa5d0e0a0b7ba6d435507da5d)            | Nome do arquivo que guardará os índices do arquivo de índice secundário.
`define `[`TITLE_TREE_FILEPATH`](#_commands_8cpp_1a4cda967ab7c0939483b2314111fa3731)            | Caminho completo pro arquivo que guardará os índices do arquivo de índice secundário.
`define `[`HASHFILE_FILENAME`](#_commands_8cpp_1a538d8295e3aa0ab7c6219633ff6ffdba)            | Nome do arquivo de hashing que guardará os registros.
`define `[`HASHFILE_FILEPATH`](#_commands_8cpp_1a00b032dcc1b72c3deaf8029036788809)            | Caminho completo pro arquivo de hashing.
`define `[`BTREE_ORDER`](#_commands_8cpp_1a76d03763b4acc5d33fee808c7b9be2cf)            | Macro para definição da ordem máxima do nó de um [BTree](#class_b_tree), baseado no tipo de dados que ele guardará.
`define `[`ID_ORDER`](#_commands_8cpp_1a0bce3c4f019c8b1e51eff533df71a88c)            | Macro com a ordem máxima que um nó de id's pode ter sem ultrapassar o tamanho do bloco.
`define `[`TITLE_ORDER`](#_commands_8cpp_1ad0926834999cddbc754bcac04cadacd8)            | Macro com a ordem máxima que um nó de títulos pode ter sem ultrapassar o tamanho do bloco.
`define `[`TITLE_CHAR_MAX`](#_entry_8hpp_1a7af6eeb437a5354124968dae124ad5e3)            | Tamanho máximo da cadeia de caracteres do título.
`define `[`AUTHORS_CHAR_MAX`](#_entry_8hpp_1a9959fa3ebdadaa7d1fb0299d0a813187)            | Tamanho máximo da cadeia de caracteres dos nomes de autores.
`define `[`TIMESTAMP_CHAR_MAX`](#_entry_8hpp_1aecd1a507c8070a8a040d28d0feb8e66d)            | Tamanho máximo da cadeia de caracteres do timestamp.
`define `[`SNIPPET_CHAR_MAX`](#_entry_8hpp_1a2e4a52bec8d3200228e62c02df7d4404)            | Tamanho máximo da cadeia de caracteres do resumo.
`public static void `[`readField`](#_commands_8cpp_1aa328beaf684c713a62ed40276a55fd66)`(char * field,std::FILE * file,int fieldSize)`            | Lê a coluna de um registro a partir do arquivo CSV.
`public static bool `[`readEntry`](#_commands_8cpp_1a8a554480ec6e3ab125ec7832ad554708)`(`[`Entry`](#struct_entry)` & e,std::FILE * file)`            | Lê por inteiro, campo a campo, um registro do arquivo CSV.
`public static void `[`printEntry`](#_commands_8cpp_1ae23babeb20988e61438fcca1984b15f7)`(const `[`Entry`](#struct_entry)` & e)`            | Imprime um registro campo por campo.
`public void `[`upload`](#_commands_8cpp_1a5ce6e0677bf7e74965349db42757f017)`(const char * filePath)`            | Recebe um arquivo CSV e cria uma base de dados baseado nele.
`public static void `[`foundEntryMessage`](#_commands_8cpp_1a79a8c447a9e1e156b231770acfd9576f)`(const `[`Entry`](#struct_entry)` & e,std::size_t blocksRead,std::size_t blockCount)`            | Função que mostra que um registro foi encontrado, quantos blocos foram lidos para encontrá-lo e quantos blocos o arquivo possui no momento.
`public static bool `[`findEntryAndPrint`](#_commands_8cpp_1a94e8666d3b1d6a1f85de01a544e7b311)`(std::FILE * hashfile,long offset,std::size_t blocksReadSoFar,std::size_t blockCount)`            | Função que acha um registro no arquivo e printa se a busca foi completa ou não, ou se ocorreu algum erro de leitura.
`public void `[`findrec`](#_commands_8cpp_1ab4c930d120953d492ab0d53475518e3f)`(long id)`            | Busca no arquivo de hashing um registro a partir de seu ID.
`public void `[`seek1`](#_commands_8cpp_1a09d19045f707f9fdb883abe7755f721b)`(long id)`            | Busca um registro a partir de seu ID usando o arquivo de índices primário.
`public void `[`seek2`](#_commands_8cpp_1a590613fba2ff7979a3d35ddf4de00b38)`(const char * title)`            | Busca um registro a partir de seu título usando o arquivo de índices secundário.
`public void `[`upload`](#_commands_8hpp_1a5ce6e0677bf7e74965349db42757f017)`(const char * filePath)`            | Recebe um arquivo CSV e cria uma base de dados baseado nele.
`public void `[`findrec`](#_commands_8hpp_1ab4c930d120953d492ab0d53475518e3f)`(long id)`            | Busca no arquivo de hashing um registro a partir de seu ID.
`public void `[`seek1`](#_commands_8hpp_1a09d19045f707f9fdb883abe7755f721b)`(long id)`            | Busca um registro a partir de seu ID usando o arquivo de índices primário.
`public void `[`seek2`](#_commands_8hpp_1a590613fba2ff7979a3d35ddf4de00b38)`(const char * title)`            | Busca um registro a partir de seu título usando o arquivo de índices secundário.
`public int `[`main`](#main_8cpp_1a3c04138a5bfe5d72780bb7e82a18e627)`(int argc,char ** argv)`            | Entrada main do programa.
`class `[`BTree`](#class_b_tree) | Classe de árvore B.
`struct `[`BTree::BNode`](#struct_b_tree_1_1_b_node) | Nó da árvore B.
`struct `[`Entry`](#struct_entry) | Registro básico do arquivo.
`struct `[`BTree::FileHeader`](#struct_b_tree_1_1_file_header) | Dados do cabeçalho do arquivo.
`struct `[`HashfileHeader`](#struct_hashfile_header) | Struct do cabeçalho do arquivo de hashing.
`struct `[`IdPointer`](#struct_id_pointer) | Struct auxiliar para guardar os índices primários.
`struct `[`BTree::OverflowResult`](#struct_b_tree_1_1_overflow_result) | Registro auxiliar com o resultado de um overflow de inserção.
`struct `[`BTree::Statistics`](#struct_b_tree_1_1_statistics) | Estrutura usada pela árvore para guardar métricas de sua execução.
`struct `[`TitlePointer`](#struct_title_pointer) | Struct auxiliar para guardar os índices secundários.
`union `[`Block`](#union_block) | Union para leitura e escrita, em bloco, de tipos arbitrários.

## Members

#### `define `[`BLOCK_SIZE`](#_block_8hpp_1ad51ded0bbd705f02f73fc60c0b721ced) 

Definição do tamanho do Bloco.

O tamanho foi definido como sendo de tamanho 4096 por ser o tamanho do setor físico indicado através do comando `fdisk -l` na máquina do Timóteo, um Linux Mint 18.1 Cinnamon 64-bit.

#### `define `[`FILEPATH`](#_commands_8cpp_1a70535ccc618237a0c7fce8317c55628e) 

Caminho do local onde os arquivos do banco de dados serão criados.

Este é um define auxiliar que é usado apenas na definição de outros defines.

Por padrão, os arquivos serã criados na mesma pasta que o executável.

#### `define `[`ID_TREE_FILENAME`](#_commands_8cpp_1a030abb644c76a5ccc3ac67269fd89290) 

Nome do arquivo que guardará os índices do arquivo de índice primário.

#### `define `[`ID_TREE_FILEPATH`](#_commands_8cpp_1ad74aa5691bd23a429f51e7b629c24c78) 

Caminho completo pro arquivo que guardará os índices do arquivo de índice primário.

#### `define `[`TITLE_TREE_FILENAME`](#_commands_8cpp_1a62caa4daa5d0e0a0b7ba6d435507da5d) 

Nome do arquivo que guardará os índices do arquivo de índice secundário.

#### `define `[`TITLE_TREE_FILEPATH`](#_commands_8cpp_1a4cda967ab7c0939483b2314111fa3731) 

Caminho completo pro arquivo que guardará os índices do arquivo de índice secundário.

#### `define `[`HASHFILE_FILENAME`](#_commands_8cpp_1a538d8295e3aa0ab7c6219633ff6ffdba) 

Nome do arquivo de hashing que guardará os registros.

#### `define `[`HASHFILE_FILEPATH`](#_commands_8cpp_1a00b032dcc1b72c3deaf8029036788809) 

Caminho completo pro arquivo de hashing.

#### `define `[`BTREE_ORDER`](#_commands_8cpp_1a76d03763b4acc5d33fee808c7b9be2cf) 

Macro para definição da ordem máxima do nó de um [BTree](#class_b_tree), baseado no tipo de dados que ele guardará.

O cálculo é feito a partir do tamanho de BLOCK_SIZE e determina a ordem máxima que o nó pode ter sem ultrapassar o tamanho do bloco, considerando que ele estará guardando dados de um tipo cujo tamanho em bytes é T_SIZE.

A fórmula do macro foi obtido a partir desta fórmula inicial:

`BLOCK_SIZE = sizeof(long) + sizeof(bool) + sizeof(int) + (2 * M + 1) * sizeof(T) + (2 * M + 2) * sizeof(long)`

Ele é baseado nos campos padrões da estrutura BNode, interna à classe [BTree](#class_b_tree).

#### `define `[`ID_ORDER`](#_commands_8cpp_1a0bce3c4f019c8b1e51eff533df71a88c) 

Macro com a ordem máxima que um nó de id's pode ter sem ultrapassar o tamanho do bloco.

#### `define `[`TITLE_ORDER`](#_commands_8cpp_1ad0926834999cddbc754bcac04cadacd8) 

Macro com a ordem máxima que um nó de títulos pode ter sem ultrapassar o tamanho do bloco.

#### `define `[`TITLE_CHAR_MAX`](#_entry_8hpp_1a7af6eeb437a5354124968dae124ad5e3) 

Tamanho máximo da cadeia de caracteres do título.

#### `define `[`AUTHORS_CHAR_MAX`](#_entry_8hpp_1a9959fa3ebdadaa7d1fb0299d0a813187) 

Tamanho máximo da cadeia de caracteres dos nomes de autores.

#### `define `[`TIMESTAMP_CHAR_MAX`](#_entry_8hpp_1aecd1a507c8070a8a040d28d0feb8e66d) 

Tamanho máximo da cadeia de caracteres do timestamp.

#### `define `[`SNIPPET_CHAR_MAX`](#_entry_8hpp_1a2e4a52bec8d3200228e62c02df7d4404) 

Tamanho máximo da cadeia de caracteres do resumo.

#### `public static void `[`readField`](#_commands_8cpp_1aa328beaf684c713a62ed40276a55fd66)`(char * field,std::FILE * file,int fieldSize)` 

Lê a coluna de um registro a partir do arquivo CSV.

Essa função é chamada para a leitura do arquivo. Nela são tratadas todas as possíveis exceções que podem ocorrer na leitura, essas exceções sao a presença no local incorreto de diversos símbolos descritos a seguir.

Os símbolos são: ; (ponto e vírgula), ``(contra-barra n), (contra-barra r), "(aspas), `NULL`.

No caso das aspas é necessário ser verificado se elas realmente se encontram em um local inválido, ou se elas são o final de uma coluna. Para isso fazemos uma etapa de verificação a mais nas aspas, nessa etapa verificamos se as aspas são seguidas de `\r`, `;`, ``, EOF. Se ela for seguida por algum desses significa que essas aspas estão demarcando o final de uma coluna.

#### Parameters
* `field` Ponteiro para o vetor de caracteres onde a coluna lida será guardada. 

* `file` O arquivo sendo lido. 

* `fieldSize` O tamanho máximo, em caracteres, que a string do campo pode ter.

Oscar Othon

#### `public static bool `[`readEntry`](#_commands_8cpp_1a8a554480ec6e3ab125ec7832ad554708)`(`[`Entry`](#struct_entry)` & e,std::FILE * file)` 

Lê por inteiro, campo a campo, um registro do arquivo CSV.

Recebendo o arquivo pelo parâmetro std::FILE *file, essa função percorre por completa uma linha de instância de registro no arquivo, dividindo esse artigo em seus campos de título, ano, autor, citações, atualização e snippet. Cada um desses campos é separado ou por meio da utilização da função [readField()](#_commands_8cpp_1aa328beaf684c713a62ed40276a55fd66), ou utilizando-se fscanf().

#### Parameters
* `e` O registro do registro a ter seus campos lidos. 

* `file` O arquivo sendo lido.

#### Returns
Se o registro foi lido com sucesso ou não, dará falso ao chegar no final do arquivo pois não haverão mais registros para ler.

Oscar Othon

#### `public static void `[`printEntry`](#_commands_8cpp_1ae23babeb20988e61438fcca1984b15f7)`(const `[`Entry`](#struct_entry)` & e)` 

Imprime um registro campo por campo.

#### Parameters
* `e` O registro a ser impresso.

Oscar Othon

#### `public void `[`upload`](#_commands_8cpp_1a5ce6e0677bf7e74965349db42757f017)`(const char * filePath)` 

Recebe um arquivo CSV e cria uma base de dados baseado nele.

Faz a carga inicial da massa de testes para seu banco de dados e cria um arquivo de dados organizado por hashing, um arquivo de índice primário usando B-Tree e outro arquivo de índice de índice secundário usando B-Tree.

A função upload faz várias verificações antes de começar a leitura do arquivo. Os detalhes são exibidos em tela. Ela precisa verificar se é possível criar cada um dos arquivos necessários no local do executável, e apenas caso tudo esteja correto ela procede à leitura dos registros.

No fim de uma execução bem sucedida o programa terá três arquivos gerados, todos com extensão `.bin`. Um deles será o arquivo de hashing, o outro o arquivo de índices primários, e por último o de índices secundários.

É esperado que essa função seja chamada antes de todas as demais.

Apenas arquivos em formato CSV serão aceitos.

#### Parameters
* `filePath` Caminho pro arquivo CSV a ser lido.

Oscar Othon

#### `public static void `[`foundEntryMessage`](#_commands_8cpp_1a79a8c447a9e1e156b231770acfd9576f)`(const `[`Entry`](#struct_entry)` & e,std::size_t blocksRead,std::size_t blockCount)` 

Função que mostra que um registro foi encontrado, quantos blocos foram lidos para encontrá-lo e quantos blocos o arquivo possui no momento.

#### Parameters
* `e` Um registro do arquivo. 

* `blocksRead` O número de blocos lidos até chegar no registro. 

* `blockCount` A contagem do número total de blocos no arquivo.

Oscar Othon

#### `public static bool `[`findEntryAndPrint`](#_commands_8cpp_1a94e8666d3b1d6a1f85de01a544e7b311)`(std::FILE * hashfile,long offset,std::size_t blocksReadSoFar,std::size_t blockCount)` 

Função que acha um registro no arquivo e printa se a busca foi completa ou não, ou se ocorreu algum erro de leitura.

Essa função recebe o arquivo já aberto, se dirige até o offset e, então, lê o registro.

#### Parameters
* `*hashfile` Ponteiro do arquivo pro arquivo de hashing. 

* `offset` O offset do registro no arquivo. 

* `blocksReadSoFar` Quantos blocos foram lidos até o momento. 

* `blockCount` A contagem do número total de blocos no arquivo.

Oscar Othon

#### `public void `[`findrec`](#_commands_8cpp_1ab4c930d120953d492ab0d53475518e3f)`(long id)` 

Busca no arquivo de hashing um registro a partir de seu ID.

O arquivo de hashing usa o hashing perfeito, então a função apenas calcula o offset do arquivo baseado em seu ID e faz a leitura do bloco em que ele se encontra.

Caso não haja nenhum registro válido na posição encontrada através do ID, a função informará.

#### Parameters
* `id` O ID do registro a ser buscado.

Oscar Othon

#### `public void `[`seek1`](#_commands_8cpp_1a09d19045f707f9fdb883abe7755f721b)`(long id)` 

Busca um registro a partir de seu ID usando o arquivo de índices primário.

O seek1 usa o arquivo de índices primário, organizado em índices de uma árvore-B, para encontrar o registro buscado.

Caso ele não seja encontrado, a função informa.

A função costuma sempre precisar ler mais blocos do que a função findrec, porque o arquivo de hashing perfeito tem acesso em O(1).

#### Parameters
* `id` O ID do registro a ser buscado.

Oscar Othon

#### `public void `[`seek2`](#_commands_8cpp_1a590613fba2ff7979a3d35ddf4de00b38)`(const char * title)` 

Busca um registro a partir de seu título usando o arquivo de índices secundário.

Já que o título não é uma chave, e sim um campo qualquer, não existe ordenação entre eles no arquivo de hashing. Então é necessário utilizarmos uma árvore-B para poder fazermos dessa busca algo mais plausível em questão de tempo. Sem a árvore teríamos que fazer uma busca linear pelo arquivo de hashing.

Caso o registro não possa ser encontrado, a função informará nos resultados.

#### Parameters
* `title` Título pelo qual o registro será buscado.

Oscar Othon

#### `public void `[`upload`](#_commands_8hpp_1a5ce6e0677bf7e74965349db42757f017)`(const char * filePath)` 

Recebe um arquivo CSV e cria uma base de dados baseado nele.

Faz a carga inicial da massa de testes para seu banco de dados e cria um arquivo de dados organizado por hashing, um arquivo de índice primário usando B-Tree e outro arquivo de índice de índice secundário usando B-Tree.

A função upload faz várias verificações antes de começar a leitura do arquivo. Os detalhes são exibidos em tela. Ela precisa verificar se é possível criar cada um dos arquivos necessários no local do executável, e apenas caso tudo esteja correto ela procede à leitura dos registros.

No fim de uma execução bem sucedida o programa terá três arquivos gerados, todos com extensão `.bin`. Um deles será o arquivo de hashing, o outro o arquivo de índices primários, e por último o de índices secundários.

É esperado que essa função seja chamada antes de todas as demais.

Apenas arquivos em formato CSV serão aceitos.

#### Parameters
* `filePath` Caminho pro arquivo CSV a ser lido.

Oscar Othon

#### `public void `[`findrec`](#_commands_8hpp_1ab4c930d120953d492ab0d53475518e3f)`(long id)` 

Busca no arquivo de hashing um registro a partir de seu ID.

O arquivo de hashing usa o hashing perfeito, então a função apenas calcula o offset do arquivo baseado em seu ID e faz a leitura do bloco em que ele se encontra.

Caso não haja nenhum registro válido na posição encontrada através do ID, a função informará.

#### Parameters
* `id` O ID do registro a ser buscado.

Oscar Othon

#### `public void `[`seek1`](#_commands_8hpp_1a09d19045f707f9fdb883abe7755f721b)`(long id)` 

Busca um registro a partir de seu ID usando o arquivo de índices primário.

O seek1 usa o arquivo de índices primário, organizado em índices de uma árvore-B, para encontrar o registro buscado.

Caso ele não seja encontrado, a função informa.

A função costuma sempre precisar ler mais blocos do que a função findrec, porque o arquivo de hashing perfeito tem acesso em O(1).

#### Parameters
* `id` O ID do registro a ser buscado.

Oscar Othon

#### `public void `[`seek2`](#_commands_8hpp_1a590613fba2ff7979a3d35ddf4de00b38)`(const char * title)` 

Busca um registro a partir de seu título usando o arquivo de índices secundário.

Já que o título não é uma chave, e sim um campo qualquer, não existe ordenação entre eles no arquivo de hashing. Então é necessário utilizarmos uma árvore-B para poder fazermos dessa busca algo mais plausível em questão de tempo. Sem a árvore teríamos que fazer uma busca linear pelo arquivo de hashing.

Caso o registro não possa ser encontrado, a função informará nos resultados.

#### Parameters
* `title` Título pelo qual o registro será buscado.

Oscar Othon

#### `public int `[`main`](#main_8cpp_1a3c04138a5bfe5d72780bb7e82a18e627)`(int argc,char ** argv)` 

Entrada main do programa.

O procedimento main representa um programa que simplesmente recebe argumentos e chama as funções apropriadas para lidar com eles.

Sintaxe do uso:

```cpp
$ <nome-do-executavel> upload <caminho-do-arquivo : string>
$ <nome-do-executavel> findrec <id : inteiro>
$ <nome-do-executavel> seek1 <id : inteiro>
$ <nome-do-executavel> seek2 <titulo : string>
```

Detalhe importante sobre o comando `upload` e seu argumento `caminho-do-arquivo`: o caminho do arquivo a subir precisa estar no formato CSV com as colunas pré-definidas de cada registro (seguindo o formato da estrutura [Entry](#struct_entry), salvo o campo `valid` dela). Qualquer outro formato resultará em comportamento indefinido do programa.

Detalhe importante sobre o comando `seek2` e seu argumento `titulo`: `titulo` não é uma string entre aspas (`"`). Se um título possuir espaços, espaços devem ser fornecidos com a contrabarra.

Exemplo:

```cpp
$ bd seek2 Título\ com\ espaços
```

#### Parameters
* `argc` Quantidade de argumentos. 

* `argv` Valores em string dos argumentos.

Timóteo Fonseca

# class `BTree` 

Classe de árvore B.

Classe de árvore B multi-uso usada para guardar índices e títulos no trabalho.

O parâmetro de template TKey determina o tipo de dado que será guardado nos nós da árvore. TKey deve ser um tipo POD (Plain Old Data type) para poder ser sequenciado no arquivo quando o nó for escrito.

O parâmetro de template M determina a ordem da árvore. Em condições normais, um nó pode ter no máximo 2M dados e 2M+1 apontadores de nó. Em estado de overflow, ele pode temporariamente ter 2M+1 dados e 2M+2 apontadores de nó.

Antes de usar uma [BTree](#class_b_tree) deve sempre se chamar o método load (em caso de apenas leitura) ou create (em caso de escrita de um novo arquivo).

No fim da inserção, o método [finishInsertions()](#class_b_tree_1a4d5c0ac3e9ade90c7a830406ecb26f45) deve ser chamado para a quantidade de blocos do arquivo ser atualizada no cabeçalho.

Exemplo de uso: 
```cpp
BTree<int, 2> arvore;
arvore.create("nomedoarquivo.bin");
arvore.insert(1);
arvore.finishInsertions();

arvore.load("nomedoarquivo.bin");
auto valor = arvore.seek(1);
if (valor) f(*valor); // Fazer algo com o valor, se encontrado.
```

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`BTree`](#class_b_tree_1ace2795117ce7ba73e15b52c72b3a10b3)`()` | Construtor padrão.
`public  `[`~BTree`](#class_b_tree_1afbb2c2201c1c7b3ebf0719c21c3fc866)`()` | Destrutor padrão.
`public bool `[`create`](#class_b_tree_1aab8e5c6367eed5e552b4fdfe51aad61b)`(const char * filepath)` | Método de inicialização da árvore, para escrita.
`public bool `[`load`](#class_b_tree_1a3f314aa35d198aa82d96d030b2ba4404)`(const char * filepath)` | Método de inicialização da árvore, para leitura.
`public void `[`insert`](#class_b_tree_1a2dcaee02bde80ebe5e1634be84357900)`(const TKey & key)` | Insere um dado dentro da árvore.
`public std::unique_ptr< TKey > `[`seek`](#class_b_tree_1ac39de078a8dfb556c4f665c5791ba6ac)`(const TKey & key)` | Busca um dado que seja equivalente ao dado fornecido.
`public `[`Statistics`](#struct_b_tree_1_1_statistics)` `[`getStatistics`](#class_b_tree_1a7b36fa86b3b3e9b4566ba3b622e107e3)`(bool includeFileBlockCount) const` | Retorna as estatísticas da árvore até então.
`public void `[`resetStatistics`](#class_b_tree_1aa8e20ba0b14f93b20302fcb5bd4932dd)`()` | Atribui valor 0 a todos os campos das estatísticas atuais.
`public void `[`finishInsertions`](#class_b_tree_1a4d5c0ac3e9ade90c7a830406ecb26f45)`()` | Atualiza o cabeçalho com o total de blocos no arquivo.

## Members

#### `public  `[`BTree`](#class_b_tree_1ace2795117ce7ba73e15b52c72b3a10b3)`()` 

Construtor padrão.

Inicializa com valores 0 as estatísticas e nulifica o ponteiro de arquivo da [BTree](#class_b_tree).

Timóteo Fonseca

#### `public  `[`~BTree`](#class_b_tree_1afbb2c2201c1c7b3ebf0719c21c3fc866)`()` 

Destrutor padrão.

Fecha o arquivo se ele estiver aberto.

#### `public bool `[`create`](#class_b_tree_1aab8e5c6367eed5e552b4fdfe51aad61b)`(const char * filepath)` 

Método de inicialização da árvore, para escrita.

Deve sempre ser chamado antes de começar a inserção na árvore, mas também permite a leitura.

Se já houver um arquivo em filepath, ele será sobreescrito.

O arquivo novo será criado com um cabeçalho e um nó raiz vazio.

#### Parameters
* `filepath` Caminho do arquivo onde os dados da árvore serão escritos.

#### Returns
Se foi possível criar o arquivo em filepath.

Timóteo Fonseca

#### `public bool `[`load`](#class_b_tree_1a3f314aa35d198aa82d96d030b2ba4404)`(const char * filepath)` 

Método de inicialização da árvore, para leitura.

Deve sempre ser chamado antes de começar a leitura da árvore. Inserções não serão possíveis.

O arquivo deve existir previamente, de preferência criado por outra execução da [BTree](#class_b_tree) após uma chamada de [create()](#class_b_tree_1aab8e5c6367eed5e552b4fdfe51aad61b).

#### Parameters
* `filepath` Caminho pro arquivo onde se encontram os dados da árvore.

#### Returns
Se foi possível abrir o arquivo em filepath.

Timóteo Fonseca

#### `public void `[`insert`](#class_b_tree_1a2dcaee02bde80ebe5e1634be84357900)`(const TKey & key)` 

Insere um dado dentro da árvore.

#### Parameters
* `key` O dado a ser inserido.

Timóteo Fonseca

#### `public std::unique_ptr< TKey > `[`seek`](#class_b_tree_1ac39de078a8dfb556c4f665c5791ba6ac)`(const TKey & key)` 

Busca um dado que seja equivalente ao dado fornecido.

Não há garantias que o dado realmente será buscado. Caso não seja encontrado, a função retorna um ponteiro nulo.

#### Parameters
* `key` O dado que se está buscando.

#### Returns
Ponteiro com o dado ou nulo.

Timóteo Fonseca

#### `public `[`Statistics`](#struct_b_tree_1_1_statistics)` `[`getStatistics`](#class_b_tree_1a7b36fa86b3b3e9b4566ba3b622e107e3)`(bool includeFileBlockCount) const` 

Retorna as estatísticas da árvore até então.

A inclusão da quantidade de blocos no arquivo, nas estatísticas, é opcional pois a árvore vai ter que fazer uma leitura do arquivo para poder obter este dado, o que é uma operação custosa.

#### Parameters
* `includeFileBlockCount` Se deve incluir (true) ou não (false) a quantidade de blocos do arquivo nas estatísticas.

#### Returns
Estatísticas.

Timóteo Fonseca

#### `public void `[`resetStatistics`](#class_b_tree_1aa8e20ba0b14f93b20302fcb5bd4932dd)`()` 

Atribui valor 0 a todos os campos das estatísticas atuais.

Timóteo Fonseca

#### `public void `[`finishInsertions`](#class_b_tree_1a4d5c0ac3e9ade90c7a830406ecb26f45)`()` 

Atualiza o cabeçalho com o total de blocos no arquivo.

Muito importante de se usar no final da utilização de uma árvore após ela ser inicializada com [create()](#class_b_tree_1aab8e5c6367eed5e552b4fdfe51aad61b).

Timóteo Fonseca

# struct `BTree::BNode` 

Nó da árvore B.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public long `[`offset`](#struct_b_tree_1_1_b_node_1a8e0f5e06540af73b1a2f271faefe85ab) | Endereço no disco.
`public bool `[`isLeaf`](#struct_b_tree_1_1_b_node_1ad112117785c2e7b8e5169f31db5dfe59) | Booleano indicando se o nó é folha (true) ou nó interno (false).
`public std::size_t `[`keysCount`](#struct_b_tree_1_1_b_node_1a7da57720fb1c5fd4279a30deb1dc6697) | Quantidade de dados dentro do nó, no momento.
`public TKey `[`keys`](#struct_b_tree_1_1_b_node_1a07e26ca52d27eb4b41bbca04e0f3b9ec) | Os dados do nó.
`public long `[`children`](#struct_b_tree_1_1_b_node_1a16759b3a85e23fcf83e9a4e2a92c0b93) | Os apontadores de nó.
`public void `[`initialize`](#struct_b_tree_1_1_b_node_1a27994df173046454415dccf8274b54e3)`(bool isLeaf,int keysCount)` | Inicializa o nó.
`public bool `[`isFull`](#struct_b_tree_1_1_b_node_1ae7c7e81cad32c0843addc3711d70d0f7)`() const` | Verifica se o nó está em sua capacidade máxima (acima de 2M dados).
`public std::unique_ptr< TKey > `[`seek`](#struct_b_tree_1_1_b_node_1a8f68bb4c490164e23dcf06e96483d0e1)`(const TKey & key,`[`BTree`](#class_b_tree)` & tree) const` | Busca um dado que seja equivalente ao dado fornecido.

## Members

#### `public long `[`offset`](#struct_b_tree_1_1_b_node_1a8e0f5e06540af73b1a2f271faefe85ab) 

Endereço no disco.

O ideal é que o nó, antes de ser escrito no registro pela primeira vez, esteja com o offset de valor -1. Após a primeira escrita, o offset passa a realmente possuir seu endereço no disco.

#### `public bool `[`isLeaf`](#struct_b_tree_1_1_b_node_1ad112117785c2e7b8e5169f31db5dfe59) 

Booleano indicando se o nó é folha (true) ou nó interno (false).

#### `public std::size_t `[`keysCount`](#struct_b_tree_1_1_b_node_1a7da57720fb1c5fd4279a30deb1dc6697) 

Quantidade de dados dentro do nó, no momento.

#### `public TKey `[`keys`](#struct_b_tree_1_1_b_node_1a07e26ca52d27eb4b41bbca04e0f3b9ec) 

Os dados do nó.

Na prática, um nó só costuma utilizar as 2M primeiras posições deste vetor. O espaço adicional de 1 dado é usado temporariamente para lidar com overflows.

#### `public long `[`children`](#struct_b_tree_1_1_b_node_1a16759b3a85e23fcf83e9a4e2a92c0b93) 

Os apontadores de nó.

Na prática, um nó só costuma utilizar as 2M+1 primeiras posições deste vetor. O espaço adicional de 1 dado é usado temporariamente para lidar com overflows.

#### `public void `[`initialize`](#struct_b_tree_1_1_b_node_1a27994df173046454415dccf8274b54e3)`(bool isLeaf,int keysCount)` 

Inicializa o nó.

O valor do campo offset do nó não é fornecido: ele é sempre inicializado como -1, para permitir que se saiba se ele já foi escrito no arquivo ou não.

Se um nó não for lido através de um readFromDisk da [BTree](#class_b_tree), é obrigatório que esse método seja chamado, pois ele funciona como um construtor do [BNode](#struct_b_tree_1_1_b_node).

A inicialização foi implementada como um método ao invés de um construtor pois é necessário que o [BNode](#struct_b_tree_1_1_b_node) seja uma classe de tipo POD (Plain Old Data type) para poder ser lido e escrito no arquivo, além de para também poder ser usado como argumento de template para o Block<T>.

#### Parameters
* `isLeaf` Se o nó vai ser inicializado como folha (true) ou não (false). 

* `keysCount` Quantidade inicial de dados no nó.

Timóteo Fonseca

#### `public bool `[`isFull`](#struct_b_tree_1_1_b_node_1ae7c7e81cad32c0843addc3711d70d0f7)`() const` 

Verifica se o nó está em sua capacidade máxima (acima de 2M dados).

Timóteo Fonseca

#### `public std::unique_ptr< TKey > `[`seek`](#struct_b_tree_1_1_b_node_1a8f68bb4c490164e23dcf06e96483d0e1)`(const TKey & key,`[`BTree`](#class_b_tree)` & tree) const` 

Busca um dado que seja equivalente ao dado fornecido.

Realiza a busca propriamente dita pelo dado, usando a instância de tree passado por parâmetro para poder realizar leituras no arquivo.

Retorna um ponteiro nulo se não encontrar o dado.

#### Returns
Ponteiro com o dado ou nulo.

Timóteo Fonseca

# struct `Entry` 

Registro básico do arquivo.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public bool `[`valid`](#struct_entry_1a652816a5955ed5164038d32671c851b4) | Se o registro constitui um registro válido no arquivo.
`public int `[`id`](#struct_entry_1aa5d205a77ff70e820e536617e4ee480d) | Identificador do registro.
`public char `[`title`](#struct_entry_1a26935b949c65094ed044b1d4cadb54aa) | Título do livro.
`public int `[`year`](#struct_entry_1a1a393c4206138f4a2c801ce43058e837) | Ano de publicação do livro.
`public char `[`authors`](#struct_entry_1a7c84be89510bc6c4332b9b5a344eefb9) | Autores do livro.
`public int `[`citations`](#struct_entry_1af3c7ba96953787b54f0d49ff21d7d2b7) | Quantidade de citações do livro.
`public char `[`updateTimestamp`](#struct_entry_1a650c6431d85f79b2bec978744caefedd) | Timestamp de atualização do livro.
`public char `[`snippet`](#struct_entry_1a7ac60a5be36b28e48ac0efa4be08c7da) | Resumo do livro.

## Members

#### `public bool `[`valid`](#struct_entry_1a652816a5955ed5164038d32671c851b4) 

Se o registro constitui um registro válido no arquivo.

#### `public int `[`id`](#struct_entry_1aa5d205a77ff70e820e536617e4ee480d) 

Identificador do registro.

#### `public char `[`title`](#struct_entry_1a26935b949c65094ed044b1d4cadb54aa) 

Título do livro.

#### `public int `[`year`](#struct_entry_1a1a393c4206138f4a2c801ce43058e837) 

Ano de publicação do livro.

#### `public char `[`authors`](#struct_entry_1a7c84be89510bc6c4332b9b5a344eefb9) 

Autores do livro.

#### `public int `[`citations`](#struct_entry_1af3c7ba96953787b54f0d49ff21d7d2b7) 

Quantidade de citações do livro.

#### `public char `[`updateTimestamp`](#struct_entry_1a650c6431d85f79b2bec978744caefedd) 

Timestamp de atualização do livro.

#### `public char `[`snippet`](#struct_entry_1a7ac60a5be36b28e48ac0efa4be08c7da) 

Resumo do livro.

# struct `BTree::FileHeader` 

Dados do cabeçalho do arquivo.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public long `[`rootAddress`](#struct_b_tree_1_1_file_header_1a31311da126e8c4c24892bc94be578a0c) | 
`public unsigned int `[`blockCount`](#struct_b_tree_1_1_file_header_1a7b665b927c85653445cc391fa6e8d740) | 

## Members

#### `public long `[`rootAddress`](#struct_b_tree_1_1_file_header_1a31311da126e8c4c24892bc94be578a0c) 

#### `public unsigned int `[`blockCount`](#struct_b_tree_1_1_file_header_1a7b665b927c85653445cc391fa6e8d740) 

# struct `HashfileHeader` 

Struct do cabeçalho do arquivo de hashing.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public int `[`blockCount`](#struct_hashfile_header_1a46bc188b9282360f810099193e2e0095) | 

## Members

#### `public int `[`blockCount`](#struct_hashfile_header_1a46bc188b9282360f810099193e2e0095) 

# struct `IdPointer` 

Struct auxiliar para guardar os índices primários.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public int `[`id`](#struct_id_pointer_1a538774c9116225b1e6d2f9d22a4b10c7) | Id do registro.
`public long `[`offset`](#struct_id_pointer_1a090367d92c7b75fd8b6d80fd407a293f) | Offset do registro no arquivo de hashing.
`public inline bool `[`operator<`](#struct_id_pointer_1ac37a4b78700cccb3e36b911873e60b7b)`(const `[`IdPointer`](#struct_id_pointer)` & that) const` | Comparador de < para o struct auxiliar, assim poderá ser usado na [BTree](#class_b_tree).

## Members

#### `public int `[`id`](#struct_id_pointer_1a538774c9116225b1e6d2f9d22a4b10c7) 

Id do registro.

#### `public long `[`offset`](#struct_id_pointer_1a090367d92c7b75fd8b6d80fd407a293f) 

Offset do registro no arquivo de hashing.

#### `public inline bool `[`operator<`](#struct_id_pointer_1ac37a4b78700cccb3e36b911873e60b7b)`(const `[`IdPointer`](#struct_id_pointer)` & that) const` 

Comparador de < para o struct auxiliar, assim poderá ser usado na [BTree](#class_b_tree).

# struct `BTree::OverflowResult` 

Registro auxiliar com o resultado de um overflow de inserção.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public TKey `[`middle`](#struct_b_tree_1_1_overflow_result_1af9ece362bc328b1a031f78b44c50768a) | O dado que, após o split de nós, é o valor do meio e deverá ser inserido no nó pai.
`public long `[`rightNode`](#struct_b_tree_1_1_overflow_result_1a49c924a0543041577a16e0ae6a3729d6) | Offset de nó que vai precisar estar no apontador à direita de onde `middle` for inserido, no nó pai.

## Members

#### `public TKey `[`middle`](#struct_b_tree_1_1_overflow_result_1af9ece362bc328b1a031f78b44c50768a) 

O dado que, após o split de nós, é o valor do meio e deverá ser inserido no nó pai.

#### `public long `[`rightNode`](#struct_b_tree_1_1_overflow_result_1a49c924a0543041577a16e0ae6a3729d6) 

Offset de nó que vai precisar estar no apontador à direita de onde `middle` for inserido, no nó pai.

# struct `BTree::Statistics` 

Estrutura usada pela árvore para guardar métricas de sua execução.

Timóteo Fonseca

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public unsigned int `[`blocksRead`](#struct_b_tree_1_1_statistics_1a3f72564e0a0b63c9d1ab54dfe88c800d) | Quantidade de blocos lidos.
`public unsigned int `[`blocksCreated`](#struct_b_tree_1_1_statistics_1a7912636750c818aa997a4192e1ebbf07) | Quantidade de blocos criados.
`public unsigned int `[`blocksInDisk`](#struct_b_tree_1_1_statistics_1a2a013d0b06d2092733b2ceb8f4c05897) | Quantidade de blocos que já foram inseridos em disco.

## Members

#### `public unsigned int `[`blocksRead`](#struct_b_tree_1_1_statistics_1a3f72564e0a0b63c9d1ab54dfe88c800d) 

Quantidade de blocos lidos.

#### `public unsigned int `[`blocksCreated`](#struct_b_tree_1_1_statistics_1a7912636750c818aa997a4192e1ebbf07) 

Quantidade de blocos criados.

#### `public unsigned int `[`blocksInDisk`](#struct_b_tree_1_1_statistics_1a2a013d0b06d2092733b2ceb8f4c05897) 

Quantidade de blocos que já foram inseridos em disco.

# struct `TitlePointer` 

Struct auxiliar para guardar os índices secundários.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public char `[`title`](#struct_title_pointer_1a14ee251cade63270a361efef543bc532) | String com o título do registro.
`public long `[`offset`](#struct_title_pointer_1a4562336754a2c64ee9962a8cba7204cd) | Offset do registro no arquivo de hashing.
`public inline bool `[`operator<`](#struct_title_pointer_1ad2a2f792d19c7426fec2cd5e4ca7c52f)`(const `[`TitlePointer`](#struct_title_pointer)` & that) const` | Comparador de < para o struct auxiliar, assim poderá ser usado na [BTree](#class_b_tree).

## Members

#### `public char `[`title`](#struct_title_pointer_1a14ee251cade63270a361efef543bc532) 

String com o título do registro.

#### `public long `[`offset`](#struct_title_pointer_1a4562336754a2c64ee9962a8cba7204cd) 

Offset do registro no arquivo de hashing.

#### `public inline bool `[`operator<`](#struct_title_pointer_1ad2a2f792d19c7426fec2cd5e4ca7c52f)`(const `[`TitlePointer`](#struct_title_pointer)` & that) const` 

Comparador de < para o struct auxiliar, assim poderá ser usado na [BTree](#class_b_tree).

Generated by [Moxygen](https://sourcey.com/moxygen)
