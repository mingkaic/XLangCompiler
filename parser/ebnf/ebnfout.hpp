//
//  ebnfout.hpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-27.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#ifndef ebnfout_hpp
#define ebnfout_hpp

#include <string>
#include <fstream>
#include <streambuf>
#include <map>
#include "ebnfparser.hpp"

void EBNFCompile(std::string tokenfile, std::string grammarfile);

#endif /* ebnfout_hpp */
