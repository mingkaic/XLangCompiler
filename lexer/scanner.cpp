#include "scanner.hpp"

void scan(std::istream& in, std::ostream& out) {
	std::string* commentstr = getPattern(T_COMMENT);
	if (NULL == commentstr) {}
	else {
		std::regex comments(*commentstr);

		std::string line;
		while (std::getline(in, line)) {
			std::string no_comment;
			std::regex_replace(std::back_inserter(no_comment), 
				line.begin(), line.end(), comments, "");
			out << no_comment << "\n";
		}
		delete commentstr;
	}
	out << std::endl;
}

std::vector<token>* lex(std::string lexeme) {
	std::map<size_t, token> tokenmap;
	mapAccess([lexeme, &tokenmap](TOKEN tok, std::string patStr) {
		std::smatch matcher;
		std::regex pattern(patStr);
		std::string lexemeCpy = lexeme;
		size_t pos = 0;
		token t;
		t.tok = tok;

		while (std::regex_search(lexemeCpy, matcher, pattern)) {
			pos += matcher.position(1);
			t.lexeme = matcher[1];
			std::map<size_t, token>::iterator prev = tokenmap.find(pos);

			if (prev == tokenmap.end() || 
				prev->second.lexeme.size() < t.lexeme.size() ||
				(prev->second.lexeme.size() == t.lexeme.size() 
					&& t.tok > prev->second.tok)) {
				if (prev != tokenmap.end()) {
					tokenmap.erase(pos);
				}
				tokenmap.insert(std::pair<size_t, token>(pos, t));
			}

			pos += matcher[1].length(); // additional length from current match
			lexemeCpy = matcher.suffix().str();
		}
	});
	// filter out overlapping tokens
	std::vector<token>* tokenlist = new std::vector<token>;
	size_t lastPos = 0;
    for (typename std::map<size_t, token>::iterator it = tokenmap.begin();
    	it != tokenmap.end();
    	it++) {
    	// take biggest token
    	if (it->first >= lastPos) {
    		if (it->first > lastPos) {
	    		std::string oddity = lexeme.substr(lastPos, it->first-lastPos);
	    		std::cout << "WARNING! " << oddity << " does not match any token\n";
	    		lastPos = it->first;
	    	}

    		tokenlist->push_back(it->second);
    		lastPos += it->second.lexeme.size();
    	}
    }
    return tokenlist;
}