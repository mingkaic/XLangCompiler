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
    EBNFCompile("decafToken.txt", "decafGrammar.txt");
    
    return 0;
}
