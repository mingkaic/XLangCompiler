CMPT 379 course prototype compiler

lexical analyzer (lexer) 1:
	uses c++11 regex library for token pattern matching. consider reimplementation to c implemented determinisitic finite automata for academic purposes. alternatively consider lex generator if necessary

	Overall the lexer is comprised of 2 parts:
		Symbol table: which associates TOKEN enums and their regex Pattern
		Lex-Scanner: reads lexeme, get a list of TOKEN enums

	Symbol table reads and includes a token header file.

	Token-pattern associations are prioritized based on order from top of file to bottom. First line has the lowest priority. Last line has highest priority.

	In poorly formed patterns such as "[^\"]" and .* which both accepts the string "keyword", the higher priority token takes precedence (consider better alternatives in the future)

	Token header declares a single enum TOKEN.
	For each line of enumerable under TOKEN, the comments that follows is the associated pattern in ECMAscript regex (with a single capture).

syntax analyzer (parser) 1:
	(in progress)