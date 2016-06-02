//
//  ebnftree.hpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-26.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#pragma once

#ifndef ebnftree_hpp
#define ebnftree_hpp

#include <vector>
#include <string>
#include "ebnflexer.hpp"
#include "searchstate.hpp"

namespace ebnf {

class state : public searchstate {
private:
    std::vector<state*> nexts;
    
public:
    bool negate = false;
    E_TOKEN type = GROUP;
    std::string content;
    
    state (void) {}
    
    state (eToken tok) : content(tok.content), type(tok.t) {}
    
    void visitChildren(std::function<void(searchstate*)> visit);
    
    size_t getNChildren(void) {
        size_t n = nexts.size();
        switch (type) {
            case REP:
                n++;
            case REP_PLUS:
            case OPT:
                n++;
                break;
        }
        return n;
    }
    
    std::string getString (void) {
        return content;
    }
    
    bool isID (void) {
        return type == SYM;
    }
    
    bool canExit (void);

    void cascadeDelete (void);
    
    void adoptChildren (state* parent);

    void push_back (state* child);

    void linkEnd (state* child);
};

}

#endif /* ebnftree_hpp */
