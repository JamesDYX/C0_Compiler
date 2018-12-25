#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgRead(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != SCANF_SYM) throw E_NOT_THIS;
		getSym();
		if (curSym.getType() != LPAREN_SYM) {
			MISS_TOKEN;
			return 0;
		}
		getSym();
		if (curSym.getType() != ID_SYM) {
			MISS_TOKEN;
			return 0;
		}
		struct tableNode node;
		node.id.lev = level;
		strcpy(node.id.name, curSym.getValueStr());
		int i = Table.find(node.id, &node);
		if (i == -1) reportError(E_UNDEF);
		else {
			if (node.kind == TB_VAR) MidCode.add(node.type == INT_SYM ? READ_I : READ_C, node.id.name);
			else if (node.kind == TB_CONST) reportError(E_VAL_VAR);
			else reportError(E_UN_MATCH);
		}
		getSym();
		while (true) {
			if (curSym.getType() != COMMA_SYM) break;
			getSym();
			if (curSym.getType() != ID_SYM) {
				MISS_TOKEN;
				return 0;
			}
			struct tableNode node1;
			node1.id.lev = level;
			strcpy(node1.id.name, curSym.getValueStr());
			int ii = Table.find(node1.id, &node1);
			if (ii == -1) reportError(E_UNDEF);
			else {
				if (node1.kind == TB_VAR) MidCode.add(node1.type == INT_SYM ? READ_I : READ_C, node1.id.name);
				else if (node1.kind == TB_CONST) reportError(E_VAL_VAR);
				else reportError(E_UN_MATCH);
			}
			getSym();
		}
		if (curSym.getType() != RPAREN_SYM) {
			MISS_TOKEN;
			return 0;
		}
		getSym();
		cout << "At line " << lineNum << " This is a read statement!" << endl;
		return 0;
	}
	catch (int) {
		seekSp(sp_tmp);
		backSym(n_tmp, type_tmp);
		curChar = sourceFile.get();
		getSym();
		return -E_NOT_THIS;
	}
}