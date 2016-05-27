//
//  ebnflexer.hpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-25.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#pragma once

#ifndef ebnflexer_hpp
#define ebnflexer_hpp

#include <string>
#include <queue>

enum E_TOKEN {
    SYM,
    ASSIGN,
    TERM,
    GROUP,
    OPT,
    REP,
    REP_PLUS,
    CONCAT,
    UNION,
    NOT,
    QUOTE,
    DOUBLE_QUOTE,
    COMMENT,
    HOLDER,
};

struct eToken {
    E_TOKEN t = HOLDER;
    std::string content;
};

class lexer {
    private:
        std::queue<eToken> result;

        static std::string walkNIgnore(std::string in, char ignore, char end, size_t& counter);
        static std::string walkNCount(std::string in, char begin, char end, size_t& counter, bool contentOblivious = false);
        static std::string readWord(std::string in, size_t& counter);
    public:
        lexer() {}
        
        lexer(std::string in);
    
        void lex(std::string in);
    
        eToken getToken() {
            eToken t;
            t.t = HOLDER;
            if (false == result.empty()) {
                t = result.front();
                result.pop();
            }
            return t;
        }
};

#endif /* ebnflexer_hpp */
