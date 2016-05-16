#include <string>
#include <iostream>
#include <regex>
#include "symboltable.hpp"

struct token {
	std::string tokenname;
	std::string content;
	size_t position;
	token(std::string tokenname, std::string content, size_t pos) :
		tokenname(tokenname), content(content), position(pos) {}
};

void scan(std::istream& in, std::ostream& out) {
	std::regex comments(getPattern("T_COMMENT")); // editable
	std::regex whites(getPattern("T_WHITESPACE")); // whitespaces

	std::string line;
	while (std::getline(in, line)) {
		std::string ostr;
		std::regex_replace(std::back_inserter(ostr), line.begin(), line.end(), whites, " ");
		std::string no_comment;
		std::regex_replace(std::back_inserter(no_comment), 
			ostr.begin(), ostr.end(), comments, "");
		out << no_comment << "\n";
	}
	out << std::endl;
}

void lex(std::stringstream& in) {
	std::string lexeme = in.str();
	std::vector<token> tokenlist;
	mapAccess([lexeme, &tokenlist](std::string tok, std::string patStr) {
		std::smatch matcher;
		std::regex pattern(patStr);
		std::string lexemeCpy = lexeme;
		size_t pos = 0;

		while (std::regex_search(lexemeCpy, matcher, pattern)) {
			token t(tok, matcher[1], pos+matcher.position(1));
			tokenlist.push_back(t);
			pos += matcher.position(1)+matcher[1].length();
			lexemeCpy = matcher.suffix().str();
		}
	});
	std::sort(tokenlist.begin(), tokenlist.end(), [](const token& t1, const token& t2) -> bool {
        return (t1.position < t2.position);
    });
    for (typename std::vector<token>::iterator it = tokenlist.begin();
    	it != tokenlist.end();
    	it++) {
    	std::cout << "@ pos " << (*it).position << ": " << (*it).tokenname << "(" << (*it).content << ")" << std::endl; 
    }
}