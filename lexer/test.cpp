#include <sstream>
#include "scanner.cpp"

#define TOKENFILE "decafTokens.txt"

int main() {
	std::ifstream testfile;
	populateTokenMap(TOKENFILE);
	testfile.open("test.txt");
	std::stringstream gatherer;
	// performed in series... consider parallel for speedup
	scan(testfile, gatherer);
	lex(gatherer);
	testfile.close();

	return 0;
}