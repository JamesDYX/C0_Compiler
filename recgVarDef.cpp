#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgVarDef(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		int type = curSym.getType();
		if (type != INT_SYM && type != CHAR_SYM) throw(E_NOT_THIS);

		struct tableNode node;
		node.type = type == INT_SYM ? INT_SYM : CHAR_SYM;
		getSym();
		if (curSym.getType() != ID_SYM) throw(E_NOT_THIS);
		node.id.lev = level;
		strcpy(node.id.name, curSym.getValueStr());
		getSym();
		if (curSym.getType() == LBRACKET_SYM) {
			//数组
			node.kind = TB_ARRAY;
			getSym();
			if (curSym.getType() != UNSNUM_SYM) MISS_TOKEN
			node.len = curSym.getValueNum();
			getSym();
			if (curSym.getType() != RBRACKET_SYM) MISS_TOKEN
			if (curState == 0) {
				if (Table.insert(node) != 0)
					reportError(E_REDEF);
				else
					MidCode.add(type == INT_SYM ? ARRAY_I : ARRAY_C, node.id.name, node.len);
			}
			getSym();
		}
		else if (curSym.getType() == COMMA_SYM || curSym.getType() == SEMICOL_SYM) {
			//变量
			node.kind = TB_VAR;
			node.val = 0;																						
			if (Table.insert(node) != 0)
				reportError(E_REDEF);
			else
				MidCode.add(type == INT_SYM ? VAR_I : VAR_C, node.id.name, 0);
		}
		else throw(E_NOT_THIS);
		streampos sp = sourceFile.tellg();
		while (true)
		{
			struct tableNode node;
			node.type = type == INT_SYM ? INT_SYM : CHAR_SYM;
			curState = 0;
			if (curSym.getType() != COMMA_SYM)
				break;
			getSym();
			if (curSym.getType() != ID_SYM) MISS_TOKEN
			node.id.lev = level;
			strcpy(node.id.name, curSym.getValueStr());
			getSym();
			if (curSym.getType() == LBRACKET_SYM) {
				//TODO数组
				node.kind = TB_ARRAY;
				getSym();
				if (curSym.getType() != UNSNUM_SYM) MISS_TOKEN
				node.len = curSym.getValueNum();
				getSym();
				if (curSym.getType() != RBRACKET_SYM) MISS_TOKEN
				if (curState == 0) {
					if (Table.insert(node) != 0)
						reportError(E_REDEF);
					else
						MidCode.add(type == INT_SYM ? ARRAY_I : ARRAY_C, node.id.name, node.len);
				}
				getSym();
			}
			else {
				node.kind = TB_VAR;
				node.val = 0;																							///////////////
				//TODO标识符
				if (Table.insert(node) != 0)
					reportError(E_REDEF);
				else
					MidCode.add(type == INT_SYM ? VAR_I : VAR_C, node.id.name, 0);
			}
			sp = sourceFile.tellg();
		}
		seekSp(sp);
		curChar = sourceFile.get();
		cout << "At line " << lineNum << " This is a variable definition statement!" << endl;
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