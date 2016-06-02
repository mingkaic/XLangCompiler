//
//  searchstate.cpp
//  symbol
//
//  Created by Mingkai Chen on 2016-05-27.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "searchstate.hpp"
#include <iostream>
#include <regex>

void matcher::put (std::string name, searchstate* s) {
    smap.insert(std::pair<std::string, searchstate*>(name, s));
    vmap.insert(std::pair<std::string, size_t>(name, counter++));
    vec.push_back(name); // test
}

// match the string (from left to right) to the state symbols
// return all possible matches of the maximum length
std::vector<signed> matcher::nextMatch (std::string str, size_t& index) {
    signed maxIndex = -1;
    std::vector<signed> token;
    signed buffer = -1;
    std::string sub = str.substr(index);
    
    for (std::map<std::string, size_t>::iterator it = vmap.begin();
        it != vmap.end();
        it++) {
        std::string copy = sub;
        buffer = treeMatch(copy, smap[it->first]);
        if (buffer > maxIndex) { // longer length
            maxIndex = buffer;
            token.clear();
            token.push_back((signed) it->second);
        } else if (maxIndex > -1 &&
            buffer == maxIndex) { // same length
            token.push_back((signed) it->second);
        }
    }
    if (maxIndex > 0) {
        index += maxIndex+1;
    }
    
    return token;
}

static void uniquePush(std::queue<searchstate*> &q , searchstate* state) {
    if (q.empty()) q.push(state);
    else {
        bool found = false;
        searchstate* marker = q.front();
        searchstate* buffer = marker;
        do {
            q.pop();
            if (state == buffer) {
                found = true;
            }
            q.push(buffer);
            buffer = q.front();
        } while (buffer != marker);
        if (found) {
            q.push(state);
        }
    }
}

signed matcher::treeMatch(std::string s, searchstate* root, size_t start) {
    if (NULL == root) {
        return -1;
    }
    std::queue<searchstate*> travelQue;
    std::map<searchstate*, size_t> travelMap;
    travelQue.push(root);
    travelMap.insert(std::pair<searchstate*, size_t>(root, start));
    
    while (false == travelQue.empty() && NULL != root) {
        root = travelQue.front();
        travelQue.pop();
        signed length = -1;
        size_t cur = travelMap[root];
        
        std::string rootStr = root->getString();
        if (0 < rootStr.size()) {
            if (root->isID()) { // symbols
                std::map<std::string, searchstate*>::iterator it = smap.find(rootStr);
                if (smap.end() != it) {
                    length = treeMatch(s.substr(cur), it->second, 0);
                }
            } else { // regex string
                try {
                    std::smatch m;
                rootStr = "(" + rootStr + ")";
                    std::regex reg(rootStr);
                    if (std::regex_search (s.substr(cur), m, reg) &&
                        0 == m.position(1)) {
                        length = (signed) m[1].length();
                    }
                } catch (std::exception e) { }
            }

            // no match found, check exit condition
            if (length < 0) {
                if (root->canExit()) { // exit
                    continue;
                } else { // not valid
                    return -1;
                }
            } else {
                cur += length;
            }
        }
        // transition
        root->visitChildren([&travelQue, &travelMap, cur](searchstate* state) {
            uniquePush(travelQue, state);
            
            // update index
            std::map<searchstate*, size_t>::iterator it = travelMap.find(state);
            if (travelMap.end() != it) {
                travelMap.erase(state);
            }
            travelMap.insert(std::pair<searchstate*, size_t>(state, cur));
        });
    }
    
    size_t max = 0;
    for (std::map<searchstate*, size_t>::iterator it = travelMap.begin();
        it != travelMap.end(); it++) {
        if (it->second > max) {
            max = it->second;
        }
    }
    
    // return max only if max is an improvement on start
    return max == start ? -1 : (signed) max;
}