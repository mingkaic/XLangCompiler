//
//  ebnfout.cpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-27.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#define SEARCH_HEADER "searchstate.hpp"
#define OUTPUT_HEADER "out.hpp"
#define OUTPUT_SOURCE "out.cpp"

#include "ebnfout.hpp"
#include SEARCH_HEADER

#include <iostream>
namespace ebnf {

void string2Tree (state* node, std::vector<std::string>& out, size_t height = 0) {
    if (out.size() == height) {
        out.push_back("height ");
        out[height].push_back('0'+height);
        out[height].append(": ");
    }
    out[height].push_back('<');
    out[height].append(node->content);
    out[height].push_back('>');
    out[height].push_back(' ');
    
    if (TERM == node->type) {
        return;
    }
    size_t i = 0;
    size_t j = 0;
    if (END_GROUP == node->type && node->canExit()) {
        return;
    }
    node->visitChildren([&out, height, i, &j](searchstate* s) {
        if (j >= i) {
            state* st = dynamic_cast<state*>(s);
            string2Tree(st, out, height+1);
        }
        j++;
    });
}

void printTree (state* node) {
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

static void fileMapper(std::string file,
    StateMap& map,
    std::vector<std::string>& tokenOrder) {
    std::ifstream fstr(file);
    std::string str((std::istreambuf_iterator<char>(fstr)),
        std::istreambuf_iterator<char>());
    lexer lex(str);
    parse(lex, map, tokenOrder);
}

void compile (std::string tokenfile, std::string grammarfile, std::string infile) {
    std::ifstream fstr(infile);
    std::string str((std::istreambuf_iterator<char>(fstr)), std::istreambuf_iterator<char>());
    StateMap tokens;
    StateMap statements;
    matcher match;
    std::vector<std::string> tokPriority;
    
    fileMapper(tokenfile, tokens, tokPriority);
    
    for (std::string tok : tokPriority) {
        statements.insert(std::pair<std::string, state*>(tok, tokens[tok]));
        match.put(tok, tokens[tok]);
    }
    
    compiler::lexer lex(match, str);
    
    for (compiler::token t = lex.getToken();
        t.t != -1;
        t = lex.getToken()) {
        std::cout << match.vec[t.t] << " <" << t.content << ">\n";
    }
    
    fileMapper(grammarfile, statements, tokPriority);
}

void serialize (std::string tokenfile, std::string grammarfile) {
    std::ofstream outhead(OUTPUT_HEADER);
    std::ofstream outsrc(OUTPUT_SOURCE);
    outsrc << "#include \"" << OUTPUT_HEADER << "\"\n";
    outhead << "#include \"" << SEARCH_HEADER << "\"\n";
    
    StateMap tokens;
    StateMap statements;
    //fileMapper(tokenfile, tokens);
    //fileMapper(grammarfile, statements);
    
    outhead << "enum TOKEN {";
    for (std::map<std::string, state*>::iterator it = tokens.begin();
        it != tokens.end();
        it++) {
        outhead << it->first << ", ";
        std::cout << it->first << "\n";
        printTree(it->second);
    }
    outhead << "};\n";

    for (std::map<std::string, state*>::iterator it = statements.begin();
        it != statements.end();
        it++) {
        std::cout << it->first << "\n";
        printTree(it->second);
    }
}

}

namespace compiler {

void lexer::lex (std::string in) {
    token buffer;
    while (0 < in.size()) {
        size_t next = 0;
        std::vector<signed> toks = match.nextMatch(in, next);
        if (0 == toks.size()) {
            std::string errmessage = "cannot match token ";
            errmessage.push_back(in[0]);
            throw std::domain_error(errmessage);
        }
        buffer.t = toks[0];
        buffer.content = in.substr(0, next-1);
        result.push(buffer);
        in = in.substr(next-1);
    }
}

}