#include <cstring>
#include <iostream>

#include "Commands.hpp"

int main(int argc, char **argv) {
	if (argc == 3) {
		char *command = argv[1];
		char *arg = argv[2];
		
		if (strcmp(command, "update") == 0) {
			update(arg);
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
