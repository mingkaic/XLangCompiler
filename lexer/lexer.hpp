/*
 *  description: main lexical analyzer header
 */

#pragma once

#include "scanner.hpp"

struct lexer {

	std::vector<token>* tokens;

	lexer (std::string fname) {
		std::ifstream testfile;
		populateTokenMap();
		testfile.open(fname);
		std::stringstream gatherer;
		scan(testfile, gatherer);
		std::string content = gatherer.str();
		tokens = lex(content);
		testfile.close();
	}

	~lexer () {
		delete tokens;
	}
};