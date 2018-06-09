#ifndef _BLOCK_HPP_INCLUDED_
#define _BLOCK_HPP_INCLUDED_

//! Definição do tamanho do Bloco.
/*!
  O tamanho foi definido como sendo de tamanho 4096 por ser o tamanho do setor
  físico indicado através do comando `fdisk -l` na máquina do Timóteo, um
  Linux Mint 18.1 Cinnamon 64-bit.
 */
#define BLOCK_SIZE 4096

//! Union para leitura e escrita, em bloco, de tipos arbitrários.
/*!
  É essencial que `sizeof(T) <= BLOCK_SIZE`. Caso esta condição seja violada,
  o tamanho da instância de union será maior do que BLOCK_SIZE e o programa
  estará sujeito a comportamento não definido.
  
  Esta union serve para garantir que, ao escrever um tipo em arquivo, ele ocupe
  o espaço de exatamente um bloco. Os bytes que restarem além do tamanho do tipo
  terão valores indefinidos no arquivo. Para realizar a leitura, é prático usar
  novamente uma variável de Block<T>.
  
  T deve ser um tipo POD (Plain Old Data type) para poder ser sequenciado no
  arquivo, além de unions apenas permitirem membros que satisfaçam tal requerimento.
 */
template <class T>
union Block {
	char padding[BLOCK_SIZE]; //!< Vetor de caracteres usado para garantir que a instância tenha no mínimo BLOCK_SIZE bytes.
	T var; //!< Membro usado para acesso ao valor empacotado no bloco.
};

#endif // _BLOCK_HPP_INCLUDED_

/** @file */
