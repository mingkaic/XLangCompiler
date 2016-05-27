#include <sstream>
#include "lexer.hpp"

int main(int argc, char* argv[]) {
	std::string test = "test.txt"; // default text
	if (argc > 1) {
		test.erase();
		test.insert(0, argv[1]);
	}

	lexer l(test);
	for (std::vector<token>::iterator it = l.tokens->begin();
		it != l.tokens->end(); 
		it++) {
		std::cout << getTokName((*it).tok) << " " << (*it).lexeme << "\n";
	}
	std::cout << "\n";

	return 0;
}