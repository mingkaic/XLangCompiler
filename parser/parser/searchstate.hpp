//
//  searchstate.hpp
//  symbol
//
//  Created by Mingkai Chen on 2016-05-27.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#ifndef statesearch_hpp
#define statesearch_hpp

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <queue>

class searchstate {
public:
    virtual ~searchstate (void) {}
    
    virtual void visitChildren (std::function<void(searchstate*)> visit) = 0;
    
    virtual size_t getNChildren(void) = 0;
    
    virtual std::string getString (void) = 0;
    
    virtual bool isID (void) = 0;
    
    virtual bool canExit (void) = 0;
};

class matcher {
private:
    size_t counter = 0;
    std::map<std::string, searchstate*> smap;
    std::map<std::string, size_t> vmap;
    
    signed treeMatch (std::string s, searchstate* root, size_t start = 0);
public:
    std::vector<std::string> vec; // test
    void put (std::string name, searchstate* s);
    
    std::vector<signed> nextMatch(std::string str, size_t& nextIndex);
};

#endif /* statesearch_hpp */
