#include <sstream>
#include "lexer.hpp"

int main() {
	lexer l("test.txt");
	for (std::vector<token>::iterator it = l.tokens->begin();
		it != l.tokens->end(); 
		it++) {
		std::cout << (*it).tok << "<" << (*it).content << "> ";
	}
	std::cout << "\n";

	return 0;
}