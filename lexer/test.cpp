#include <sstream>
#include "scanner.cpp"

#define TOKENFILE "decafTokens.txt"

int main() {
	std::ifstream testfile;
	populateTokenMap(TOKENFILE);
	testfile.open("test.txt");
	std::stringstream gatherer;
	scan(testfile, gatherer);
	std::string content = gatherer.str();
	lex(content);
	testfile.close();

	return 0;
}