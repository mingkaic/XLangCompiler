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

struct state {
    bool negate = false;
    E_TOKEN type = GROUP;
    std::string content;
    std::vector<state*> nexts;
    
    state() {}
    
    state(eToken tok) : content(tok.content), type(tok.t) {}

    void cascadeDelete ();
    
    void adoptChildren(state* parent);

    void push_back(state* child);

    void linkChildren(state* child);
};

#endif /* ebnftree_hpp */
