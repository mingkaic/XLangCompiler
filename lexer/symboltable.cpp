#include "symboltable.hpp"

static std::regex tokenregex("\\s*\\S+\\s*,\\s*//\\s*(.+)");
static std::vector<std::string> patterns;

void populateTokenMap() {
	std::ifstream fstr;
	fstr.open(TOKENFILE);
  	std::smatch sm; 

	std::string line;
	std::string pat;
	while (std::getline(fstr, line)) {
		if (std::regex_match(line, sm, tokenregex)) {
			patterns.push_back(sm[1]);
		}
	}

	fstr.close();
}

std::string* getPattern(TOKEN tok) {
	if (tok < patterns.size()) {
		return new std::string(patterns[tok]);
	}
	return NULL;
}

void mapAccess(std::function<void(TOKEN, std::string)> perform) {
	size_t end = patterns.size();
	for (size_t i = 0; i < end; i++) {
		perform((TOKEN) i, patterns[i]);
	}
}