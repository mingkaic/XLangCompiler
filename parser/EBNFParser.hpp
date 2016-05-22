#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <regex>
#include <queue>
#include "../lexer/symboltable.hpp"

// EBNF ISO/IEC 14977 Standard Symbols
// syntax operators/delimiters
#define E_ASSIGN 	'='
#define E_TERM1 	';'
#define E_TERM2 	'.'
// set grouping
#define E_OPT_BEGIN	'['
#define E_OPT_END	']'
#define E_REP_BEGIN	'{'
#define E_REP_END	'}'
#define E_CAP_BEGIN	'('
#define E_CAP_END	')'
// set operation
#define E_CONCAT	','
#define E_UNION		'|'
#define E_NOT		'-'
// special grouping
#define E_STR1		'"'
#define E_STR2		'\''

// fixed syntax
enum REP {
	ONCE,
	OPTIONAL,
	MORETHANONCE,
	MORETHANZERO,
};

class state {
	public:
		REP reps = ONCE;
		std::vector<state*> nexts;
		std::string content;
		signed sid = -1; // -2 >= ::= groups, -1 ::= content, 0 <= ::= content-free
		
		state() {}

		state(signed id) : sid(id) {}
    
        void cascadeDelete () {
            for (std::vector<state*>::iterator it = nexts.begin();
                it != nexts.end(); it++) {
                (*it)->cascadeDelete();
            }
            delete this;
        }

		void push_back(state* child) {
			if (NULL == child) {

			} else {
				nexts.push_back(child);
			}
		}

		void linkChildren(state* child) {
			if (NULL == child) {

			} else if (nexts.size() == 0) { // base case
				nexts.push_back(child);
			} else {
				for (std::vector<state*>::iterator it = nexts.begin(); it != nexts.end(); it++) {
					(*it)->linkChildren(child);
				}
			}
		}
};

int test ();