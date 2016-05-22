#include "EBNFParser.hpp"

static std::map<std::string, signed> name2Symbol;
static std::map<signed, std::string> symb2Name; // for testing

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

static std::string walkNIgnore(std::string in, char ignore, char end, size_t& counter) {
    std::string buffer;
    char c = in[counter];
	while (c != end && counter < in.size()) {
		c = in[counter++];
        if (c == ignore && counter < in.size() && in[counter] == end) {
			buffer.push_back(in[counter++]);
        } else if (c != end) {
			buffer.push_back(c);
		}
	}
	return buffer;
}

state* stateTree (std::string statement, signed cur_level = 0) {
	state* topState = NULL;
	statement = trim(statement);
	std::smatch matcher;

	if (statement.length() == 0) {
		return NULL;
	} else {
		// set grouping
		std::regex captureWord("(\\w+)\\s*(.*)");
		std::string group;
		size_t index = 1;
		REP rep = ONCE;
        topState = new state(-2-cur_level);
        bool NEGATE = statement[0] == E_NOT;
        if (NEGATE) { // prefix
            statement = statement.erase(0, 1);
        }
		switch (statement[0]) {
            case E_STR1:
            case E_STR2:
				group = walkNIgnore(statement, '\\', statement[0], index);
				statement = statement.substr(index, statement.size()-index);
                topState->sid = -1;
                topState->content = group;
                break;
			case E_CAP_BEGIN:
				group = walkUntilEnd(statement, E_CAP_BEGIN, E_CAP_END, index);
				statement = statement.substr(index, statement.size()-index);
				topState->push_back(stateTree(group, cur_level+1));
				break;
			case E_OPT_BEGIN:
				rep = OPTIONAL;
				group = walkUntilEnd(statement, E_OPT_BEGIN, E_OPT_END, index);
				statement = statement.substr(index, statement.size()-index);
				topState->push_back(stateTree(group, cur_level+1));
				break;
			case E_REP_BEGIN:
				rep = MORETHANONCE;
				group = walkUntilEnd(statement, E_REP_BEGIN, E_REP_END, index);
				topState->push_back(stateTree(group, cur_level+1));
				statement = statement.substr(index, statement.size()-index);
				break;
			default:
				// capture symbol until space
				if (std::regex_match(statement, matcher, captureWord)) {
					group = matcher[1];
					statement = matcher[2];
					if (name2Symbol.end() == name2Symbol.find(group)) {
						topState->push_back(stateTree(group.erase(0, 1)));
					} else {
						topState->sid = name2Symbol[group];
					}
				} else {
                    statement = trim(statement.erase(0, 1));
                }
		}
        if (NEGATE) {
			std::map<size_t, void*> dumpIds; 
			state* dump = topState;
            topState = new state(-2-cur_level);
			for (std::vector<state*>::iterator it = dump->nexts.begin(); it != dump->nexts.end(); it++) {
				dumpIds.insert(std::pair<size_t, void*>((*it)->sid, NULL));
			}
			dump->cascadeDelete();
			for (std::map<std::string, signed>::iterator it = name2Symbol.begin(); it != name2Symbol.end(); it++) {
				if (dumpIds.end() == dumpIds.find(it->second)) { // not dump
					state* buffer = new state();
					buffer->sid = it->second;
					topState->push_back(buffer);
				}
			}
        } else {
            if ((topState->reps == MORETHANONCE && rep == OPTIONAL) ||
                (rep == MORETHANONCE && topState->reps == OPTIONAL)) {
                topState->reps = MORETHANZERO;
            }
            if (topState->reps < rep) {
                topState->reps = rep;
            }
        }
		// set operation
		if (statement.length() == 0) {
			// DO NOTHING
		} else {
            state* partner;
			switch (statement[0]) {
				case E_UNION:
					partner = stateTree(statement.erase(0, 1));
					if (partner->sid > -2) {
						state* front = topState;
						topState = new state(-2-cur_level);

						topState->push_back(front);
						topState->push_back(partner);
					} else { // either union subgroup or union group of current level 
                        partner->push_back(topState);
                        topState = partner;
					}
					break;
				case E_CONCAT:
				default:
					topState->linkChildren(stateTree(statement));
			}
		}
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
std::vector<state*> topParse (std::map<std::string, std::string>& smap) {
	std::vector<state*> initialStates;
	state* stateBuffer;
	std::string statement;
	for (std::map<std::string, std::string>::iterator it = smap.begin(); it != smap.end(); it++) {
		statement = spaceFilter(it->second);
		stateBuffer = stateTree(statement);
		std::map<std::string, signed>::iterator sIt = name2Symbol.find(it->first);
		if (sIt == name2Symbol.end()) {
			// error handle
		} else {
			stateBuffer->sid = sIt->second;
			initialStates.push_back(stateBuffer);
		}
	}
    return initialStates;
}

int test () {
	populateTokenMap();
	std::map<std::string, std::string> m;
	
	readGrammarFile("decafGrammar.txt", m);
	std::vector<state*> states = topParse(m); // ends with a lot of empty states
    for (std::vector<state*>::iterator it = states.begin(); it != states.end(); it++) {
        printTree(*it);
    }

	return 0;
}