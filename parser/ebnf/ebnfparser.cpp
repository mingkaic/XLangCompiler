//
//  ebnfparser.cpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-25.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "ebnfparser.hpp"

static state* readStatement(lexer& lex, bool termEnd = true) {
    state* root = new state();
    state* laststate = root;
    state* partner = NULL;
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
                    laststate->linkChildren(partner);
                } else {
                    root->push_back(partner);
                    laststate = partner;
                    concat = true;
                }
                break;
			case GROUP:
			case OPT:
            case REP:
            case REP_PLUS:
                sublexer.lex(tok.content);
                // ignore terminator
                partner = readStatement(sublexer, false);
                partner->type = tok.t;
                if (concat) {
                    laststate->linkChildren(partner);
                } else {
                    root->push_back(partner);
                    laststate = partner;
                    concat = true;
                }
                break;
            case SYM:
                partner = new state(tok);
                if (concat) {
                    laststate->linkChildren(partner);
                } else {
                    root->push_back(partner);
                    laststate = partner;
                    concat = true;
                }
                break;
                
            case UNION:
                concat = false;
                break;
            case CONCAT:
                concat = true;
                break;
            
            // terminators
            case HOLDER:
                if (termEnd) {
                    root->cascadeDelete();
                    root = NULL;
                }
                break;
            case TERM:
                break;
                
            // ignore
            case COMMENT:
                break;
                
            // error control
			default:
                throw std::domain_error("invalid token passed into parser");
        }
        if (NULL == partner) {
            throw std::domain_error("NULL token");
        }
        partner->negate = negate;
    }
    
    return root;
}

void parse(lexer& lex, std::map<std::string, state*>& states) {
    state* stateBuffer;
    
    for (eToken tok = lex.getToken();
        HOLDER != tok.t;
        tok = lex.getToken()) {
        eToken tok2 = lex.getToken();
        if (SYM == tok.t && ASSIGN == tok2.t) {
            stateBuffer = readStatement(lex);
            if (NULL == stateBuffer) {
                // log error
            }
            std::map<std::string, state*>::iterator it = states.find(tok.content);
            if (states.end() == it) {
                states.insert(std::pair<std::string, state*>(tok.content, stateBuffer));
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