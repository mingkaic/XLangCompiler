//
//  ebnfout.cpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-27.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "ebnfout.hpp"
#define OUTPUT_HEADER "out.hpp"
#define OUTPUT_SOURCE "out.cpp"

#include <iostream>
void string2Tree(state* node, std::vector<std::string>& out, size_t height = 0) {
    if (out.size() == height) {
        out.push_back("height ");
        out[height].push_back('0'+height);
        out[height].append(": ");
    }
    out[height].push_back('<');
    out[height].append(node->content);
    out[height].push_back('>');
    out[height].push_back(' ');
    for (std::vector<state*>::iterator it = node->nexts.begin(); it != node->nexts.end(); it++) {
        string2Tree(*it, out, height+1);
    }
}

void printTree(state* node) {
    if (node == NULL) return;
    std::vector<std::string> tree;
    string2Tree(node, tree);
    std::cout << "----------------------------\n";
    for (std::vector<std::string>::iterator it = tree.begin();
        it != tree.end(); it++) {
        std::cout << *it << std::endl;
    }
    std::cout << "----------------------------\n";
}

void EBNFCompile(std::string tokenfile, std::string grammarfile) {
    std::ifstream ftok(tokenfile);
    std::ifstream fgram(grammarfile);
    std::ofstream outhead(OUTPUT_HEADER);
    std::ofstream outsrc(OUTPUT_SOURCE);
    std::string tstr((std::istreambuf_iterator<char>(ftok)),
                 std::istreambuf_iterator<char>());
    std::string sstr((std::istreambuf_iterator<char>(fgram)),
                 std::istreambuf_iterator<char>());

    lexer tokens(tstr);
    lexer statements(sstr);
    
    std::map<std::string, state*> tokenMap;
    std::map<std::string, state*> stateMap;
    std::map<std::string, size_t> indexLookup;
    parse(tokens, tokenMap);
    parse(statements, stateMap);
    
    outsrc << "#include \"" << OUTPUT_HEADER << "\"\n";
    outhead << "enum TOKEN {";
    size_t index = 0;
    for (std::map<std::string, state*>::iterator it = tokenMap.begin();
        it != tokenMap.end();
        it++) {
        indexLookup.insert(std::pair<std::string, size_t>(it->first, index++));
        outhead << it->first << ", ";
        std::cout << it->first << "\n";
        printTree(it->second);
    }
    outhead << "};";
    for (std::map<std::string, state*>::iterator it = stateMap.begin();
        it != stateMap.end();
        it++) {
        indexLookup.insert(std::pair<std::string, size_t>(it->first, index++));
        std::cout << it->first << "\n";
        printTree(it->second);
    }
}