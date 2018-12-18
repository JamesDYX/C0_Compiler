#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgValDef(int level) {
	int curState = 0;
	streampos sp = sourceFile.tellg();
	int n = curSym.len;
	int type = curSym.getType();
	try {
		if (curSym.getType() == INT_SYM) {
			struct tableNode node;
			getSym();
			if (curSym.getType() != ID_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			strcpy_s(node.id.name, curSym.getValueStr());
			node.id.lev = level;
			node.kind = TB_CONST;
			node.type = INT_SYM;
			getSym();
			if (curSym.getType() != ASSIGN_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			getSym();
			bool isPos = true;
			if (curSym.getType() == PLUS_SYM || curSym.getType() == MINIUS_SYM) {
				if (curSym.getType() == MINIUS_SYM) isPos = false;
				getSym();
			}
			if (curSym.getType() != UNSNUM_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			node.val = isPos ? curSym.getValueNum() : (0 - curSym.getValueNum());
			if (curState != E_MISS_TOKEN) {
				if (Table.insert(node) != 0)
					reportError(E_REDEF);
				else
					MidCode.add(CONST_I, node.id.name, node.val);
			}
			streampos sp = sourceFile.tellg();
			while (true) {
				struct tableNode node;
				curState = 0;
				getSym();
				if (curSym.getType() != COMMA_SYM)
					break;
				getSym();
				if (curSym.getType() != ID_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
					curState = E_MISS_TOKEN;
				}
				strcpy_s(node.id.name, curSym.getValueStr());
				node.id.lev = level;
				node.kind = TB_CONST;
				node.type = INT_SYM;
				getSym();
				if (curSym.getType() != ASSIGN_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
					curState = E_MISS_TOKEN;
				}
				getSym();
				bool isPos = true;
				if (curSym.getType() == PLUS_SYM || curSym.getType() == MINIUS_SYM) {
					if (curSym.getType() == MINIUS_SYM) isPos = false;
					getSym();
				}
				if (curSym.getType() != UNSNUM_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
					curState = E_MISS_TOKEN;
				}
				node.val = isPos ? curSym.getValueNum() : (0 - curSym.getValueNum());
				if (curState != E_MISS_TOKEN) {
					if (Table.insert(node) != 0)
						reportError(E_REDEF);
					else
						MidCode.add(CONST_I, node.id.name, node.val);
				}
				sp = sourceFile.tellg();
			}
			seekSp(sp);
			curChar = sourceFile.get();
			cout << "At line " << lineNum << " This is a constant definition statement!" << endl;
			getSym();
			return 0;
		}
		if (curSym.getType() == CHAR_SYM) {
			struct tableNode node;
			getSym();
			if (curSym.getType() != ID_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			strcpy_s(node.id.name, curSym.getValueStr());
			node.id.lev = level;
			node.kind = TB_CONST;
			node.type = CHAR_SYM;
			getSym();
			if (curSym.getType() != ASSIGN_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			getSym();
			if (curSym.getType() != CHARACH_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			node.val = curSym.getValueCh();
			if (curState != E_MISS_TOKEN) {
				if (Table.insert(node) != 0)
					reportError(E_REDEF);
				else
					MidCode.add(CONST_C, node.id.name, node.val);
			}
			streampos sp = sourceFile.tellg();
			while (true) {
				struct tableNode node;
				curState = 0;
				getSym();
				if (curSym.getType() != COMMA_SYM)
					break;
				getSym();
				if (curSym.getType() != ID_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
					curState = E_MISS_TOKEN;
				}
				strcpy_s(node.id.name, curSym.getValueStr());
				node.id.lev = level;
				node.kind = TB_CONST;
				node.type = CHAR_SYM;
				getSym();
				if (curSym.getType() != ASSIGN_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
					curState = E_MISS_TOKEN;
				}
				getSym();
				if (curSym.getType() != CHARACH_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
					curState = E_MISS_TOKEN;
				}
				node.val = curSym.getValueCh();
				if (curState != E_MISS_TOKEN) {
					if (Table.insert(node) != 0)
						reportError(E_REDEF);
					else
						MidCode.add(CONST_C, node.id.name, node.val);
				}
				sp = sourceFile.tellg();
			}
			seekSp(sp);
			curChar = sourceFile.get();
			cout << "At line " << lineNum << " This is a constant definition statement!" << endl;
			getSym();
			return 0;
		}
		else {
			throw(E_NOT_THIS);
		}
	}
	catch (int) {
		seekSp(sp);
		backSym(n, type);
		curChar = sourceFile.get();
		getSym();
		return -E_NOT_THIS;
	}
}