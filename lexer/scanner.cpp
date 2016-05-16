#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include "symboltable.hpp"

struct token {
	std::string tokenname;
	std::string content;
	size_t priority;
};

void scan(std::istream& in, std::ostream& out) {
	std::regex comments(getPattern("T_COMMENT")); // editable

	std::string line;
	while (std::getline(in, line)) {
		std::string no_comment;
		std::regex_replace(std::back_inserter(no_comment), 
			line.begin(), line.end(), comments, "");
		out << no_comment << "\n";
	}
	out << std::endl;
}

void lex(std::string lexeme) {
	std::map<size_t, token> tokenmap;
	mapAccess([lexeme, &tokenmap](std::string tok, std::string patStr, size_t priority) {
		std::smatch matcher;
		std::regex pattern(patStr);
		std::string lexemeCpy = lexeme;
		size_t pos = 0;
		token t;
		t.tokenname = tok;
		t.priority = priority;

		while (std::regex_search(lexemeCpy, matcher, pattern)) {
			pos += matcher.position(1);
			t.content = matcher[1];
			std::map<size_t, token>::iterator prev = tokenmap.find(pos);

			if (prev == tokenmap.end() || 
				prev->second.content.size() < t.content.size() ||
				(prev->second.content.size() == t.content.size() 
					&& t.priority > prev->second.priority)) {
				if (prev != tokenmap.end()) {
					tokenmap.erase(pos);
				}
				tokenmap.insert(std::pair<size_t, token>(pos, t));
			}

			pos += matcher[1].length(); // additional length
			lexemeCpy = matcher.suffix().str();
		}
	});
	std::vector<token> tokenlist;
	size_t lastPos = 0;
    for (typename std::map<size_t, token>::iterator it = tokenmap.begin();
    	it != tokenmap.end();
    	it++) {
    	// take biggest token
    	if (it->first >= lastPos) {
    		tokenlist.push_back(it->second);
    		lastPos += it->second.content.size();
    	}
    }
    for (typename std::vector<token>::iterator it = tokenlist.begin();
    	it != tokenlist.end();
    	it++) {
    	std::cout << (*it).tokenname << "(" << (*it).content << ")" << std::endl; 
    }
}