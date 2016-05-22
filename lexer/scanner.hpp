#pragma once

#include <map>
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include "symboltable.hpp"

struct token {
	TOKEN tok;
	std::string lexeme;
};

// remove T_COMMENT tokens
void scan(std::istream& in, std::ostream& out);

// get list of tokens
std::vector<token>* lex(std::string lexeme);