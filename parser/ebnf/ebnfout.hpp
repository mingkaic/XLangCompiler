//
//  ebnfout.hpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-27.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#ifndef ebnfout_hpp
#define ebnfout_hpp

#include <string>
#include <fstream>
#include <streambuf>
#include <map>
#include "ebnfparser.hpp"

namespace ebnf {

void compile (std::string tokenfile, std::string grammarfile, std::string infile);

void serialize (std::string tokenfile, std::string grammarfile);

}

namespace compiler {

struct token {
    signed t = -1;
    std::string content;
};

class lexer {
    private:
        matcher& match;
        std::queue<token> result;
    
    public:
        lexer (matcher& match) : match(match) {}
        lexer (matcher& match, std::string in) : match(match) { lex(in); }
        void lex (std::string in);

        token getToken (void) {
            token t;
            t.t = -1;
            if (false == result.empty()) {
                t = result.front();
                result.pop();
            }
            return t;
        }
};

}

#endif /* ebnfout_hpp */
