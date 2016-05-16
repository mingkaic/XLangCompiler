CMPT 379 course prototype compiler

lexer 1:
	uses c++11 regex library for token pattern matching. consider reimplementation to c implemented determinisitic finite automata for academic purposes. alternatively consider lex generator if necessary

	Overall the lexer is comprised of 2 parts:
		Symbol table: which associates TOKENs and their regex Pattern (from here the patterns can be converted to DFAs for use by other parts)
		Lex-Scanner: reads lexeme

	Symbol table reads token + pattern and prioritizes from top of file to bottom. Lower the line of token + pattern = Higher priority.
	In poorly formed patterns such as "[^\"]" and .* which accepts any keyword, the higher priority token takes precedence (consider better alternatives in the future) 