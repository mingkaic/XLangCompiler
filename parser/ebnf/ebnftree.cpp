//
//  ebnftree.cpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-26.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "ebnftree.hpp"

namespace ebnf {

void state::visitChildren(std::function<void(searchstate*)> visit) {
    state* next = NULL;
    std::map<state*, void*> set;
    for (std::vector<state*>::iterator it = nexts.begin(); it != nexts.end(); it++) {
        next = *it;
        if (set.end() == set.find(*it)) {
            visit(*it);
        }
        set.insert(std::pair<state*, void*>(*it, NULL));
        if (next->type == OPT || next->type == REP) {
            size_t endDet = 1;
            for (next = next->nexts[0];
                next->nexts.size() && endDet > 0;
                next = next->nexts[0]) {
                switch (next->type) {
                    case GROUP:
                    case OPT:
                    case REP:
                    case REP_PLUS:
                        endDet++;
                        break;
                    case END_GROUP:
                        endDet--;
                    case TERM:
                        endDet = 0;
                        break;
                }
            }
            for (std::vector<state*>::iterator iit = next->nexts.begin();
                iit != next->nexts.end(); iit++) {
                if (set.end() == set.find(*iit)) {
                    visit(*iit);
                }
            }
        }
    }
}

bool state::canExit (void) {
    bool exit = false;
    
    for (std::vector<state*>::iterator it = nexts.begin(); it != nexts.end(); it++) {
        exit = exit || TERM == (*it)->type;
        if (END_GROUP == (*it)->type) {
            exit = exit || (*it)->canExit();
        }
    }
    return exit;
}

void state::cascadeDelete (void) {
    for (std::vector<state*>::iterator it = nexts.begin();
        it != nexts.end(); it++) {
        (*it)->cascadeDelete();
    }
    delete this;
}

void state::adoptChildren (state* parent) {
    for (std::vector<state*>::iterator it = parent->nexts.begin(); it != parent->nexts.end(); it++) {
        nexts.push_back(*it);
    }
}

void state::push_back (state* child) {
    if (NULL == child) {

    } else {
        nexts.push_back(child);
    }
}

void state::linkEnd (state* child) {
    if (NULL == child) {
        
    } else if (TERM == type) { // base case 1: end
        nexts.insert(nexts.end(), child->nexts.begin(), child->nexts.end());
    } else if (END_GROUP == type) { // base case 2: group fork
        if (nexts.size() < 2) {
            nexts.push_back(child);
        } else {
            for (std::vector<state*>::iterator it = nexts.begin()+1; it != nexts.end(); it++) {
                (*it)->linkEnd(child);
            }
        }
    } else if (0 == nexts.size()) { // base case 3: normal end
        nexts.push_back(child);
    } else {
        for (std::vector<state*>::iterator it = nexts.begin(); it != nexts.end(); it++) {
            (*it)->linkEnd(child);
        }
    }
}

}