#include "symboltable.hpp"

struct tokenData {
	std::string pattern;
	size_t priority;
};

static std::regex tokenregex("\\s*(\\S+)\\s+(.+)");
static std::map<std::string, tokenData> tokenTable;

void populateTokenMap(std::string tokenfname) {
	std::ifstream fstr;
	fstr.open(tokenfname);
  	std::smatch sm; 

	std::string line;
	std::string token;
	tokenData dbuff;
	size_t lineN = 0;
	while (std::getline(fstr, line)) {
		std::regex_match(line, sm, tokenregex);
		token = sm[1];
		dbuff.pattern = sm[2];
		dbuff.priority = lineN;
		tokenTable.insert(std::pair<std::string, tokenData>(token, dbuff));
		lineN++;
	}

	fstr.close();
}

std::string getPattern(std::string tokenname) {
	return tokenTable.at(tokenname).pattern;
}

void mapAccess(std::function<void(std::string, std::string, size_t)> perform) {
	for (typename std::map<std::string, tokenData>::iterator it = tokenTable.begin(); 
		it!=tokenTable.end(); 
		++it) {
		perform(it->first, it->second.pattern, it->second.priority);
	}
}