//
//  ebnfparser.cpp
//  ebnf
//  generates a map of non-deterministic finite state automatas
//  basically a thompson construct
//
//  Created by Mingkai Chen on 2016-05-25.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "ebnfparser.hpp"

namespace ebnf {

static state* readStatement (lexer& lex, bool termEnd = true) {
    state* root = new state();
    state* laststate = root;
    state* partner = NULL;
    state* groupEnder = NULL;
    state* ender = NULL;
    bool concat = true;
    lexer sublexer;
    eToken tok;
    tok.t = SYM;

    while (HOLDER != tok.t && TERM != tok.t) {
        tok = lex.getToken();
        bool negate = tok.t == NOT; // lives for 1 top-level token
        if (negate) {
            tok = lex.getToken();
        }
        switch (tok.t) {
            case QUOTE:
            case DOUBLE_QUOTE:
                partner = new state(tok);
                if (concat) {
                    laststate->push_back(partner);
                } else {
                    root->push_back(partner);
                    concat = true;
                }
                laststate = partner;
                break;
			case GROUP:
			case OPT:
            case REP:
            case REP_PLUS:
                sublexer.lex(tok.content);
                // ignore terminator
                partner = readStatement(sublexer, false);
                partner->type = tok.t;
                
                groupEnder = new state();
                groupEnder->type = END_GROUP;
                
                partner->linkEnd(groupEnder);
                
                if (tok.t == REP ||
                    tok.t == REP_PLUS) {
                    groupEnder->push_back(partner);
                }
                if (concat) {
                    laststate->push_back(partner);
                } else {
                    root->push_back(partner);
                    concat = true;
                }
                laststate = groupEnder;
                break;
            case SYM:
                partner = new state(tok);
                if (concat) {
                    laststate->push_back(partner);
                } else {
                    root->push_back(partner);
                    concat = true;
                }
                laststate = partner;
                break;
                
            case UNION:
                concat = false;
                break;
            case CONCAT:
                concat = true;
                break;
            case WHITESPACE:
                break;
            
            // terminators
            case HOLDER:
                if (termEnd) {
                    root->cascadeDelete();
                    root = NULL;
                }
                break;
            case TERM:
                ender = new state();
                ender->type = TERM;
                root->linkEnd(ender);
                break;
                
            // ignore
            case COMMENT:
                break;
                
            // error control
			default:
                throw std::domain_error("invalid token passed into parser");
        }
        if (NULL == partner) {
            if (WHITESPACE != tok.t) {
                throw std::domain_error("NULL token");
            }
        } else {
            partner->negate = negate;
        }
    }
    
    return root;
}

void parse (lexer& lex,
    StateMap& states,
    std::vector<std::string>& order) {
    state* stateBuffer;
    
    for (eToken tok = lex.getToken();
        HOLDER != tok.t;
        tok = lex.getToken()) {
        eToken tok2 = lex.getToken();
        while (WHITESPACE == tok2.t) {
            tok2 = lex.getToken();
        }
        if (SYM == tok.t && ASSIGN == tok2.t) {
            stateBuffer = readStatement(lex);
            if (NULL == stateBuffer) {
                throw std::domain_error("statement returned NULL tree");
            }
            StateMap::iterator it = states.find(tok.content);
            if (states.end() == it) {
                states.insert(std::pair<std::string, state*>(tok.content, stateBuffer));
                order.push_back(tok.content);
            } else {
                it->second->adoptChildren(stateBuffer);
                delete stateBuffer;
            }
        } else {
            // log error
            // skip to next statement
            while (TERM != tok.t &&
                HOLDER != tok.t) {
                tok = lex.getToken();
            }
        }
    }
}

}