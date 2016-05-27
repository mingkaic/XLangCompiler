//
//  ebnftree.cpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-26.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "ebnftree.hpp"

void state::cascadeDelete () {
    for (std::vector<state*>::iterator it = nexts.begin();
        it != nexts.end(); it++) {
        (*it)->cascadeDelete();
    }
    delete this;
}

void state::adoptChildren(state* parent) {
    for (std::vector<state*>::iterator it = parent->nexts.begin(); it != parent->nexts.end(); it++) {
        nexts.push_back(*it);
    }
}

void state::push_back(state* child) {
    if (NULL == child) {

    } else {
        nexts.push_back(child);
    }
}

void state::linkChildren(state* child) {
    if (NULL == child) {

    } else if (nexts.size() == 0) { // base case
        nexts.push_back(child);
    } else {
        for (std::vector<state*>::iterator it = nexts.begin(); it != nexts.end(); it++) {
            (*it)->linkChildren(child);
        }
    }
}

