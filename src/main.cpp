#include <cstring>
#include <iostream>

#include "Commands.hpp"

//! Program main
/*!
 * Receives arguments and calls the appropriate functions for the task.
 *
 * Remember to `upload` first before using the other commands.
 *
 * Program usage:
 *
 * ```
 * $ <exec-name> upload <input-file : string>
 * $ <exec-name> findrec <id : int>
 * $ <exec-name> seek1 <id : int>
 * $ <exec-name> seek2 <title : string>
 * ```
 * @param argc Argument count
 * @param argv Argument values
 */
int main(int argc, char **argv) {
	auto usageExamples = [] {
		std::cout << "Usage:\n";
		std::cout << "$ <program> upload  <input-file>\n";
		std::cout << "$ <program> findrec <id>\n";
		std::cout << "$ <program> seek1   <id>\n";
		std::cout << "$ <program> seek2   <title>" << std::endl;
	};

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
			std::cout << "Unknown command: " << command << '\n';
			usageExamples();
		}
	}
	else if (argc < 3) {
		std::cout << "Too few arguments." << '\n';
		usageExamples();
	}
	else {
		std::cout << "Too many arguments." << '\n';
		usageExamples();
	}
}
