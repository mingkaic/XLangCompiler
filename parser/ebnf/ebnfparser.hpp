//
//  ebnfparser.hpp
//  ebnf
//  generates a map of non-deterministic finite state automatas
//  basically a thompson construct
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

namespace ebnf {

#define StateMap std::map<std::string, state*>

void parse (lexer& lex,
    StateMap& states,
    std::vector<std::string>& order);

}

#endif /* ebnfparser_hpp */