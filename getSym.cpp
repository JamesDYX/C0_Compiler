#include "getSym.h"
#include "symbol.h"
#include "error.h"
#include <ctype.h>
#include <string.h>

ifstream sourceFile;
char curChar;
int lineNum = 0;

const char * symName[] = {
	"PLUS_SYM",		"MINUS_SYM",	"STAR_SYM",		"SLASH_SYM",	"ASSIGN_SYM",
	"LPAREN_SYM",	"RPAREN_SYM",	"LBRACKET_SYM",	"RBRACKET_SYM",	"LCURLY_SYM",
	"RCURLY_SYM",	"COMMA_SYM",
	"SEMICOL_SYM",	"LESS_SYM",		"GREAT_SYM",	"EQUAL_SYM",	"NOTEQ_SYM",
	"LESSEQ_SUM",	"GREATEQ_SYM",	"CONST_SYM",	"INT_SYM",		"CHAR_SYM",
	"VOID_SYM",		"MAIN_SYM",		"SCANF_SYM",	"PRINTF_SYM",	"RETURN_SYM",
	"IF_SYM",		"ELSE_SYM",		"FOR_SYM",		"ELSE_SYM",
	"CHARACH_SYM",	"NUM_SYM",		"UNSNUM_SYM",	"ID_SYM",		"STR_SYM",
};

const char * token[] = {
	"+",	"-",	"*",	 "/",	  "=",
	"(",	")",	"[",	 "]",	  "{",
	"}",	",",
	";",	"<",	">",	 "==",	  "!=",
	"<=",	">=",	"const", "int",	  "char",
	"void",	"main",	"scanf", "printf","return",
	"if",	"else",	"for",	 "while",
};

int getSym() {
	int STATE = START;
	int ERROR_NUM = 0;
	char token[1001] = {};
	while (STATE != END && STATE != ERROR) {
		if (STATE == START) {
			if (curChar == EOF)
				return -E_END_FILE;
			if (curChar == ' ' || curChar == '\t')
				moveState(&STATE, START);
			else if (curChar == '\n') {
				moveState(&STATE, START);
				lineNum++;
			}
			else if (curChar == '/')
				moveState(&STATE, SLASH);
			else if (curChar == '+' || curChar == '-') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, SINGLCH1);
			}
			else if (curChar == '*' || curChar == ',' || curChar == ';' || curChar == '(' || curChar == ')' || curChar == '[' || curChar == ']' || curChar == '{' || curChar == '}') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, SINGLCH1);
			}
			else if (curChar == '<' || curChar == '>' || curChar == '=') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, SINGLCH2);
			}
			else if (curChar == '!') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, DOUBLCH0);
			}
			else if (curChar == '\'') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, CHARACH0);
			}
			else if (curChar == '\"') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, STR0);
			}
			else if (isLetter(curChar)) {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, WORD);
			}
			else if (curChar>='0' && curChar <= '9') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, NUM);
			}
			else {
				reportError(E_ILL_CHAR);
				moveState(&STATE, START);
				//curChar = sourceFile.get();
				//colNum++;
				//ERROR_NUM = E_IN_LEXER;
				//moveState(&STATE, ERROR);
			}
		}
		else if (STATE == SLASH) {
			// 若想使文法支持注释，取消以下注释即可
			//if (curChar == '/')
			//	moveState(&STATE, SINGNOTE);
			//else if (curChar == '*')
			//	moveState(&STATE, MULTNOTE0);
			//else 
			{
				token[0] = '/';
				curSym.set(STATE, token);
				moveState(&STATE, END);
			}
		}
		else if (STATE == SINGNOTE) {
			if (curChar == '\n') {
				moveState(&STATE, START);
				lineNum++;
			}
			else if (STATE == EOF)
				return -E_END_FILE;
			else
				moveState(&STATE, SINGNOTE);
		}
		else if (STATE == MULTNOTE0) {
			if (curChar == EOF)
				return -E_IN_NOTE;
			else if (curChar == '*')
				moveState(&STATE, MULTNOTE1);
			else {
				if (curChar == '\n') {
					lineNum++;
				}
				moveState(&STATE, MULTNOTE0);
			}
		}
		else if (STATE == MULTNOTE1) {
			if (curChar == EOF)
				return -E_IN_NOTE;
			else if (curChar == '*')
				moveState(&STATE, MULTNOTE1);
			else if (curChar == '/')
				moveState(&STATE, START);
			else {
				if (curChar == '\n') {
					lineNum++;
				}
				moveState(&STATE, MULTNOTE0);
			}
		}
		else if (STATE == SINGLCH0) {
			if (isdigit(curChar)) {
				if (addChar(token) != 0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, SIGNINT);
			}
			else {
				curSym.set(STATE, token);
				moveState(&STATE, END);
			}
		}
		else if (STATE == SINGLCH1) {
			curSym.set(STATE, token);
			moveState(&STATE, END);
		}
		else if (STATE == SINGLCH2) {
			if (curChar == '=') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, DOUBLCH1);
			}
			else
			{
				curSym.set(STATE, token);
				moveState(&STATE, END);
			}
		}
		else if (STATE == DOUBLCH0) {
			if (curChar == '=') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, DOUBLCH1);
			}
			else {
				ERROR_NUM = E_NO_EQUAL;
				moveState(&STATE, ERROR);
			}
		}
		else if (STATE == DOUBLCH1) {
			curSym.set(STATE, token);
			moveState(&STATE, END);
		}
		else if (STATE == CHARACH0) {
			if (curChar == '+' || curChar == '-' || curChar == '*' || curChar == '/' || isLetter(curChar) || (curChar>='0' && curChar<='9')) {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, CHARACH1);
			}
			else {
				ERROR_NUM = E_IN_CHAR;
				moveState(&STATE, ERROR);
			}
		}
		else if (STATE == CHARACH1) {
			if (curChar == '\'') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, CHARACH2);
			}
			else {
				ERROR_NUM = E_NO_SIN_QUOT;
				moveState(&STATE, ERROR);
			}
		}
		else if (STATE == CHARACH2) {
			curSym.set(STATE, token);
			moveState(&STATE, END);
		}
		else if (STATE == STR0) {
			if(curChar>=' ' && curChar<='~' && curChar!='\"'){
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, STR0);
			}
			else if (curChar == '\"') {
				if (addChar(token) != 0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, STR2);
			}
			else {
				ERROR_NUM = E_IN_STR;
				moveState(&STATE, ERROR);
			}
		}
		else if (STATE == STR2) {
			curSym.set(STATE, token);
			moveState(&STATE, END);
		}
		else if (STATE == WORD) {
			if (isdigit(curChar) || isLetter(curChar)) {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, WORD);
			}
			else {
				curSym.set(STATE, token);
				moveState(&STATE, END);
			}
		}
		else if (STATE == NUM) {
			if (curChar>='0' && curChar<='9') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, NUM);
			}
			else {
				curSym.set(STATE, token);
				moveState(&STATE, END);
			}
		}
		else if (STATE == SIGNINT) {
			if (curChar >= '0' && curChar <= '9') {
				if(addChar(token)!=0) {ERROR_NUM = E_TOO_LONG; STATE = ERROR;}
				moveState(&STATE, SIGNINT);
			}
			else {
				curSym.set(STATE, token);
				moveState(&STATE, END);
			}
		}
		else {
			ERROR_NUM = E_IN_LEXER;
			moveState(&STATE, ERROR);
		}
		if (STATE == ERROR) {
			//TODO
			reportError(ERROR_NUM);
			if (ERROR_NUM == E_TOO_LONG || ERROR_NUM==E_NO_SIN_QUOT || ERROR_NUM==E_NO_DOUB_QUOT) {
				while (curChar != '\n' && curChar != EOF) {
					curChar = sourceFile.get();
				}
				moveState(&STATE, START);
				token[0] = '\0';
			}
			else if (ERROR_NUM == E_NO_EQUAL) {
				moveState(&STATE, START);
				token[0] = '\0';
			}
			else if (ERROR_NUM == E_IN_CHAR) {
				STATE = CHARACH0;
			}
			else if (ERROR_NUM == E_IN_STR) {
				STATE = STR0;
			}
			else 
			{
				while (curChar != '\n' && curChar != EOF) {
					curChar = sourceFile.get();
				}
				moveState(&STATE, START);
				token[0] = '\0';
			}
		}
	}
	return 0;
}

void moveState(int * from, int to) {
	*from = to;
	if (to != ERROR && to != END) {
		curChar=sourceFile.get();
	}
}
int addChar(char token[11]) {
	int len = strlen(token);
	if (len == 1000)
		return -E_TOO_LONG;
	token[len++] = curChar;
	token[len] = '\0';
	return 0;
}
int isLetter(char c) {
	return (c >= 'a' & c <= 'z') | (c >= 'A' & c <= 'Z') | c == '_';
}

void backSym(int n, int Type) {
	for (int i = 0; i < n; i++) {
		sourceFile.seekg(-(off_t)sizeof(char), ios::cur);
		char c = sourceFile.get();
		if ((c == ' ' && Type != STR_SYM) || c=='\t') {
			//sourceFile.seekg(-(off_t)sizeof(char), ios::cur);
			i--;
		}
		else if (c == '\n') {
			sourceFile.seekg(-(off_t)sizeof(char), ios::cur);
			i--;
		}
		sourceFile.seekg(-(off_t)sizeof(char), ios::cur);
	}
}