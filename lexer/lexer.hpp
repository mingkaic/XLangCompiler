/*
 *  description: main lexical analyzer header
 */

#pragma once

#include "scanner.hpp"

struct lexer {

	std::vector<token>* tokens;

	lexer (std::string fname) {
		std::ifstream tokenf;
		populateTokenMap();
		tokenf.open(fname);
		if (tokenf.is_open()) {
			std::stringstream gatherer;
			scan(tokenf, gatherer);
			std::string content = gatherer.str();
			tokens = lex(content);
			tokenf.close();
		}
	}

	~lexer () {
		delete tokens;
	}
};