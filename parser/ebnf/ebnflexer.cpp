//
//  ebnflexer.cpp
//  ebnf
//
//  Created by Mingkai Chen on 2016-05-25.
//  Copyright © 2016 Mingkai Chen. All rights reserved.
//

#include "ebnflexer.hpp"
#include <regex>

// ISO 14977 standard
#define E_ASSIGN    '='
#define E_TERM1     ';'
#define E_TERM2     '.'

// set grouping
#define E_OPT_BEGIN '['
#define E_OPT_END   ']'
#define E_REP_BEGIN '{'
#define E_REP_END   '}'
#define E_CAP_BEGIN '('
#define E_CAP_END   ')'
#define E_REP_PLUS  "}+" // non-ISO 14977 standard

// set operation
#define E_CONCAT    ','
#define E_UNION     '|'
#define E_NOT       '-'

// special grouping
#define E_STR1      '"'
#define E_STR2      '\''

#define E_GROUP_COM '*'
#define E_COMMENT   std::regex("\\*.*\\*|\\?.*\\?")
#define E_TOK_SYM   std::regex("(\\s*)")

static std::string trim (const std::string& str, const std::string& whitespace = " \t\r\n\v\f") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

static inline bool isAlnum(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            (c == '_');
}

std::string lexer::walkNIgnore(std::string in, char ignore, char end, size_t& counter) {
    std::string buffer;
    if (counter >= in.size()) {
        return buffer;
    }
    counter++;
    char c = in[counter];
	while (end != c && counter < in.size()) {
		c = in[counter++];
        if ((c == ignore && counter < in.size() && in[counter] == ignore) ||
            (c == ignore && counter < in.size() && in[counter] == end)) {
            buffer.push_back(c);
			buffer.push_back(in[counter++]);
        } else if (end != c){
			buffer.push_back(c);
		}
	}
	return buffer;
}

std::string lexer::walkNCount(std::string in, char begin, char end, size_t& counter, bool contentOblivious) {
	std::string buffer;
    if (counter >= in.size()) {
        return buffer;
    }
	size_t groupLevel = 1;
    counter++;
	while (groupLevel > 0 && counter < in.size()) {
		char c = in[counter++];
        if (false == contentOblivious) {
            switch (c) {
                case E_STR1:
                case E_STR2:
                    buffer.push_back(c);
                    // add '\\' back if found
                    buffer += walkNIgnore(in, '\\', c, --counter);
                    
                    //buffer.push_back(c);
                    break;
            }
        }
		if (c == end) {
            groupLevel--;
		} else if (c == begin) {
			groupLevel++;
		}
		if (groupLevel > 0) {
			buffer.push_back(c);
		}
	}
	return buffer;
}

std::string lexer::readWord(std::string in, size_t& counter) {
    std::string buffer;
    bool endFound = false;
    size_t end = in.size();
	while (false == endFound && counter < end) {
		char c = in[counter++];
        if (isAlnum(c)) {
			buffer.push_back(c);
		} else {
            endFound = true;
            counter--;
        }
	}
	return buffer;
}

lexer::lexer(std::string in) {
    lex(in);
}

void lexer::lex (std::string in) {
 	std::regex symbol("(\\s*([\\(\\)\\[\\]\\+\\{\\}-\\.;=\\|,])\\s*)");
    in = trim(std::regex_replace(in, symbol, "$2"));
    size_t i = 0;
    eToken buffer;
    
	if (in.length() == 0) {
		return;
	} else {
        size_t end = in.size();
        while (i < end && in[i] != EOF) {
            switch (in[i]) {
                case E_ASSIGN:
                    buffer.t = ASSIGN;
                    i++;
                    break;
                    
                case E_TERM1:
                case E_TERM2:
                    buffer.t = TERM;
                    i++;
                    break;
                    
                case E_STR1:
                    buffer.t = DOUBLE_QUOTE;
                    buffer.content = walkNIgnore(in, '\\', in[i], i);
                    break;
                case E_STR2:
                    buffer.t = QUOTE;
                    buffer.content = walkNIgnore(in, '\\', in[i], i);
                    break;
                    
                case E_CAP_BEGIN:
                    buffer.t = GROUP;
                    if (E_GROUP_COM == in[i+1]) {
                        buffer.t = COMMENT;
                        // content oblivious
                        // doesn't care about strings
                        buffer.content = walkNCount(in, E_CAP_BEGIN, E_CAP_END, i, true);
                    } else {
                        buffer.content = walkNCount(in, E_CAP_BEGIN, E_CAP_END, i);
                    }
                    break;
                    
                case E_OPT_BEGIN:
                    buffer.t = OPT;
                    buffer.content = walkNCount(in, E_OPT_BEGIN, E_OPT_END, i);
                    break;
                    
                case E_REP_BEGIN:
                    buffer.t = REP;
                    buffer.content = walkNCount(in, E_REP_BEGIN, E_REP_END, i);
                    if (in[i] == E_REP_PLUS[1]) {
                        buffer.t = REP_PLUS;
                        i++;
                    }
                    break;
                case '\n':
                case '\r':
                    i++;
                    break;
                
                case ' ':
                case '\t':
                case '\v':
                case '\f':
                case E_CONCAT:
                    buffer.t = CONCAT;
                    i++;
                    break;
                    
                case E_UNION:
                    buffer.t = UNION;
                    i++;
                    break;
                    
                case E_NOT:
                    buffer.t = NOT;
                    i++;
                    break;
                    
                default:
                    if (isAlnum(in[i])) {
                        buffer.t = SYM;
                        buffer.content = readWord(in, i);
                    } else {
                        // err
                        i++;
                    }
            }
            if (buffer.t != HOLDER) {
                result.push(buffer);
                buffer.content.clear();
                buffer.t = HOLDER;
            }
        }
    }
}