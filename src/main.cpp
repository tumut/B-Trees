#include <cstring>
#include <iostream>

#include "Commands.hpp"

//! Entrada main do programa.
/*!
  O procedimento main representa um programa que simplesmente recebe argumentos
  e chama as funções apropriadas para lidar com eles.
  
  Sintaxe do uso:
  
  ```
  $ <nome-do-executavel> upload <caminho-do-arquivo : string>
  $ <nome-do-executavel> findrec <id : inteiro>
  $ <nome-do-executavel> seek1 <id : inteiro>
  $ <nome-do-executavel> seek2 <titulo : string>
  ```
  
  Detalhe importante sobre o comando `upload` e seu argumento `caminho-do-arquivo`:
  o caminho do arquivo a subir precisa estar no formato CSV com as colunas
  pré-definidas de cada registro (seguindo o formato da estrutura Entry, salvo
  o campo `valid` dela). Qualquer outro formato resultará em comportamento
  indefinido do programa.
  
  Detalhe importante sobre o comando `seek2` e seu argumento `titulo`: `titulo`
  não é uma string entre aspas (`"`). Se um título possuir espaços, espaços
  devem ser fornecidos com a contrabarra.
  
  Exemplo:
  
  ```
  $ bd seek2 Título\ com\ espaços
  ```
  
  \param argc Quantidade de argumentos.
  \param argv Valores em string dos argumentos.
  
  \author Timóteo Fonseca
 */
int main(int argc, char **argv) {
	if (argc == 3) {
		char *command = argv[1];
		char *arg = argv[2];
		
		if (strcmp(command, "upload") == 0) {
			upload(arg);
		}
		else if (strcmp(command, "findrec") == 0) {
			long id = atol(arg);
			findrec(id);
		}
		else if (strcmp(command, "seek1") == 0) {
			long id = atol(arg);
			seek1(id);
		}
		else if (strcmp(command, "seek2") == 0) {
			seek2(arg);
		}
		else {
			std::cout << "Comando desconhecido: " << command << std::endl;
		}
	}
	else if (argc < 3) {
		std::cout << "Argumentos insuficientes." << std::endl;
	}
	else {
		std::cout << "Argumentos demais." << std::endl;
	}
}
