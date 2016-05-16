#include "symboltable.hpp"

static std::regex tokenregex("\\s*(\\S+)\\s+(.+)");
static std::map<std::string, std::string> tokenTable;

void populateTokenMap(std::string tokenfname) {
	std::ifstream fstr;
	fstr.open(tokenfname);
  	std::smatch sm; 

	std::string line;
	std::string token;
	std::string pattern;
	while (std::getline(fstr, line)) {
		std::regex_match(line, sm, tokenregex);
		token = sm[1];
		pattern = sm[2];
		tokenTable.insert(std::pair<std::string, std::string>(token, pattern));
	}

	fstr.close();
}

std::string getPattern(std::string tokenname) {
	return tokenTable.at(tokenname);
}

void mapAccess(std::function<void(std::string, std::string)> perform) {
	for (std::map<std::string, std::string>::iterator it = tokenTable.begin(); 
		it!=tokenTable.end(); 
		++it) {
		perform(it->first, it->second);
	}
}