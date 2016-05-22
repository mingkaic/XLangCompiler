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

static std::map<std::string, size_t> name2Symbol;
static std::map<size_t, std::string> symb2Name; // for testing

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

		void demote() {

		}
};

void printTree(state* node) {
	std::cout << "----------------------------\n";
	std::queue<state*> q;
	std::map<state*, void*> visited;
	q.push(node);
	q.push(NULL);
	bool lastNULL = false;
	if (node == NULL) return;
	while (!q.empty()) {
		node = q.front();
		q.pop();
		if (node == NULL) {
			if (!lastNULL) {
				std::cout << "\n";
			}
			lastNULL = true;
		} else {
			lastNULL = false;
			std::cout << symb2Name[node->sid] << " ";
			visited.insert(std::pair<state*, void*>(node, NULL));
			for (std::vector<state*>::iterator it = node->nexts.begin(); it != node->nexts.end(); it++) {
				if (visited.end() == visited.find(*it)) {
					q.push(*it);
				}
			}
			q.push(NULL);
		}
	}
	std::cout << "----------------------------\n";
}

static std::string spaceFilter (std::string statement) {
  	std::regex comments("([?*][^*?]*[*?])");
 	std::regex symbol("\\W([|,])\\W");

	std::string filtered = std::regex_replace(statement, comments, "");
	filtered = std::regex_replace(filtered, symbol, "$1");

	return filtered;
}

static std::string trim (const std::string& str, const std::string& whitespace = " \t\r\n\v\f") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

static std::string walkUntilEnd(std::string in, char begin, char end, size_t& counter) {
	std::string buffer;
	size_t groupLevel = 1;
	while (groupLevel > 0) {
		char c = in[counter++];
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

state* stateTree (std::string statement, signed cur_level = 0) {
	state* topState = NULL;
	//std::regex strFormat("\"((?:[^\\\n\"]|\\[abtnvfr\\'\"])+)\"|'((?:[^\\\n']|\\[abtnvfr\\\''])+)'");
	statement = trim(statement);
	std::smatch matcher;

	/*if (std::regex_match(statement, matcher, strFormat)) {
		topState = new state;
		topState->sid = -1;
		if (matcher[1].length()) {
			topState->content = matcher[1];
		} else if (matcher[2].length()) {
			topState->content = matcher[2];
		}
	} else*/ /*if (name2Symbol.end() != name2Symbol.find(statement)) {
		topState = new state;

		topState->sid = name2Symbol[statement];
	} else */if (statement.length() == 0) {
		return NULL;
	} else {
		// set grouping
		std::regex captureWord("(\\w+)\\s*(.*)");
		std::string group;
		size_t index = 1;
		REP rep = ONCE;
		switch (statement[0]) {
			topState = new state(-2-cur_level);
			case E_CAP_BEGIN:
				group = walkUntilEnd(statement, E_CAP_BEGIN, E_CAP_END, index);
				statement = statement.substr(index, statement.size()-index);
				topState->push_back(stateTree(group, cur_level++));
				break;
			case E_OPT_BEGIN:
				rep = OPTIONAL;
				group = walkUntilEnd(statement, E_OPT_BEGIN, E_OPT_END, index);
				statement = statement.substr(index, statement.size()-index);
				topState->push_back(stateTree(group, cur_level++));
				break;
			case E_REP_BEGIN:
				rep = MORETHANONCE;
				group = walkUntilEnd(statement, E_REP_BEGIN, E_REP_END, index);
				topState->push_back(stateTree(group, cur_level++));
				statement = statement.substr(index, statement.size()-index);
				break;
			default:
				// capture symbol until space
				if (std::regex_match(statement, matcher, captureWord)) {
					if (name2Symbol.end() == name2Symbol.find(statement)) {
						topState->push_back(stateTree(group.erase(0, 1)));
					} else {
						topState->sid = name2Symbol[statement];
					}
					group = matcher[1];
					statement = matcher[2];
				}
		}
		if ((topState->reps == MORETHANONCE && rep == OPTIONAL) ||
			(rep == MORETHANONCE && topState->reps == OPTIONAL)) {
			topState->reps = MORETHANZERO;
		}
		if (topState->reps < rep) {
			topState->reps = rep;
		}
		// set operation
		if (statement.length() == 0) {
			// end
		} else {
            state* partner;
			switch (statement[0]) {
				case E_UNION:
					partner = stateTree(statement.erase(0, 1));
					if (partner->sid > -2) {
						state* front = topState;
						front->demote();
						topState = new state(-2-cur_level);

						topState->push_back(front);
						topState->push_back(partner);
					} else { // either union subgroup or union group of current level 
                        partner->push_back(topState);
                        topState = partner;
					}
					break;
				case E_NOT:
					break;
				case E_CONCAT:
				default:
					topState->linkChildren(stateTree(statement));
			}
		}
/*
		std::regex capture("\\(([^\\)]+)\\)(.*)");
		std::regex optional("\\[([^\\]]+)\\](.*)");
		std::regex rep("\\{([^\\}]+)\\}(.*)");
		std::regex concat("(\\[^ \f\n\r\t\v,])[ \f\n\r\t\v,](.*)");
		std::regex unions("(\\[^\\|])\\|(.+)");
		std::regex unionsEnd("(\\w+)(.+)");
		std::regex exception("-\\s*(\\w+)(.+)");

		if (std::regex_match(statement, matcher, capture)) {
			topState = stateTree(matcher[1]);
		} else if (std::regex_match(statement, matcher, optional)) {
			topState = stateTree(matcher[1]);
			topState->reps = (topState->reps == MORETHANONCE) ? MORETHANZERO : OPTIONAL;
			topState->linkChildren(stateTree(matcher[2]));
		} else if (std::regex_match(statement, matcher, rep)) {
			topState = stateTree(matcher[1]);
			if (topState->reps != MORETHANZERO) {
				topState->reps = MORETHANONCE;
			}
			topState->linkChildren(stateTree(matcher[2]));
		} else if (std::regex_match(statement, matcher, concat)) {
			topState = stateTree(matcher[1]);
			topState->push_back(stateTree(matcher[2]));
		} else if (std::regex_match(statement, matcher, unions)) {
			topState = new state();
			do {
				topState->nexts.push_back(stateTree(matcher[1]));
				statement = matcher[2];
			} while (std::regex_match(statement, matcher, unions));
			if (std::regex_match(statement, matcher, unionsEnd)) {
			std::cout << "IN UNION " << statement << std::endl;
				topState->nexts.push_back(stateTree(matcher[1]));
				topState->linkChildren(stateTree(matcher[2]));
			}
		} else if (std::regex_match(statement, matcher, exception)) {
			topState = new state();
			std::map<size_t, void*> dumpIds; 
			state* dump = stateTree(matcher[1]);
			for (std::vector<state*>::iterator it = dump->nexts.begin(); it != dump->nexts.end(); it++) {
				dumpIds.insert(std::pair<size_t, void*>((*it)->sid, NULL));
			}
			delete dump;
			for (std::map<std::string, size_t>::iterator it = name2Symbol.begin(); it != name2Symbol.end(); it++) {
				if (dumpIds.end() == dumpIds.find(it->second)) { // not dump
					state* buffer = new state();
					buffer->sid = it->second;
					topState->push_back(buffer);
				}
			}
			topState->linkChildren(stateTree(matcher[2]));
		} else {
			topState = stateTree(statement.erase(0, 1));
		}*/
	}
	return topState;
}

// fname[in]
// smap[out]
void readGrammarFile (std::string fname, std::map<std::string, std::string>& smap) {
	std::map<std::string, std::vector<std::string> > vmap;
	std::string sname;
	std::string statement;
	std::string* entry = &sname;
	FILE *fp = fopen(fname.c_str(), "r");
	if (fp) {
		// start constructing the statement
		std::map<std::string, std::vector<std::string> >::iterator it;
		while (!feof(fp)) {
			char c = fgetc(fp);
			switch (c) {
				case E_TERM1:
				case E_TERM2:
					statement = trim(statement);
					it = vmap.find(sname);
					if (it == vmap.end()) {
						std::vector<std::string> in;
						in.push_back(statement);
						vmap.insert(std::pair<std::string, std::vector<std::string> >(sname, in));
					} else {
						(it->second).push_back(statement);
					}
					sname.clear();
					statement.clear();
					entry = &sname;
					break;
				case E_ASSIGN:
					sname = trim(sname); 
					entry = &statement;
					break;
				default:
					entry->push_back(c);
			}
		}
		fclose(fp);
	}

	size_t tokenCount = 0;
	mapAccess([&tokenCount](TOKEN tok, std::string patstr) {
		name2Symbol.insert(std::pair<std::string, size_t>(getTokName(tok), tok));
		symb2Name.insert(std::pair<size_t, std::string>(tok, getTokName(tok))); // for testing
		tokenCount++;
	});

	std::vector<state*> initialStates;
	for (std::map<std::string, std::vector<std::string> >::iterator it = vmap.begin(); it != vmap.end(); it++) {
		name2Symbol.insert(std::pair<std::string, size_t>(it->first, tokenCount));
		symb2Name.insert(std::pair<size_t, std::string>(tokenCount, it->first)); // for testing
		tokenCount++;

		std::string gather;
		for (std::vector<std::string>::iterator sit = it->second.begin(); sit != it->second.end(); sit++) {
			gather += "(" + (*sit) + ")|";
		}
		gather.pop_back();
		smap.insert(std::pair<std::string, std::string>(it->first, gather));
	}
}

// smap[in]
void topParse (std::map<std::string, std::string>& smap) {
	std::vector<state*>* initialStates = new std::vector<state*>;
	state* stateBuffer;
	std::string statement;
	for (std::map<std::string, std::string>::iterator it = smap.begin(); it != smap.end(); it++) {
		statement = spaceFilter(it->second);
		std::cout << ">>>>>>>>>>>" << it->first << "\n";
		std::cout << statement << "<<<<\n";
		stateBuffer = stateTree(statement);
		std::map<std::string, size_t>::iterator sIt = name2Symbol.find(it->first);
		if (sIt == name2Symbol.end()) {
			// error handle
		} else {
			stateBuffer->sid = sIt->second;
			initialStates->push_back(stateBuffer);
		}
	}
	for (std::vector<state*>::iterator it = initialStates->begin(); it != initialStates->end(); it++) {
		printTree(*it);
	}
}

int main () {
	populateTokenMap();
	std::map<std::string, std::string> m;
	
	readGrammarFile("decafGrammar.txt", m);
	topParse(m);

	return 0;
}