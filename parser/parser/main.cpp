//
//  main.cpp
//  parser
//
//  Created by Mingkai Chen on 2016-05-21.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include <iostream>
#include "ebnfout.hpp"

int main(int argc, const char * argv[]) {
    try {
    ebnf::compile("decafToken.txt", "decafGrammar.txt", "test.txt");
    } catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    
    //ebnf::serialize();
    return 0;
}
