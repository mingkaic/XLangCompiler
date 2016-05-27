//
//  ebnfparser.hpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-25.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#pragma once

#ifndef ebnfparser_hpp
#define ebnfparser_hpp

#include <string>
#include <map>
#include <vector>
#include <exception>
#include "ebnftree.hpp"

void parse(lexer& lex, std::map<std::string, state*>& states);

#endif /* ebnfparser_hpp */