#include <iostream>
#include "syntaxAnalys.h"
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgFuncDef(int level) {
	int curState = 0;
	streampos sp = sourceFile.tellg();
	int n = curSym.len;
	int type = curSym.getType();
	try {
		struct tableNode node;
		int type_tmp = curSym.getType();
		if (type_tmp == INT_SYM || type_tmp==CHAR_SYM || type_tmp==VOID_SYM) {
			getSym();
			if (curSym.getType() != ID_SYM) {
				throw(E_NOT_THIS);
			}
			node.id.lev = level;
			strcpy_s(node.id.name, curSym.getValueStr());
			node.kind = type_tmp==VOID_SYM?TB_NFUNC:TB_IFUNC;
			node.type = type_tmp;
			node.paramNum = 0;
			getSym();
			if (curSym.getType() == LPAREN_SYM) {
				getSym();
				recgParaList(level+1, &node);
				if (curSym.getType() != RPAREN_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
				}
				if (Table.insert(node) != 0) {
					reportError(E_REDEF);
					return 0;
				}
				MidCode.add(FUNC_DEF, node.id.name, node.type);
				strcpy(MidCode.func_name, node.id.name);
				for (int i = 0; i < node.paramNum; i++) {
					if (node.param[i]->type == INT_SYM) MidCode.add(PARA_I, node.param[i]->id.name);
					else  MidCode.add(PARA_C, node.param[i]->id.name);
				}
				getSym();
				if (curSym.getType() != LCURLY_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
				}
				getSym();
				recgCompound(level + 1);
				if (curSym.getType() != RCURLY_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
				}
				if (!hasRetrun) {
					if (type_tmp == VOID_SYM) MidCode.add(RET_V);
					else {
						reportError(E_NO_RET);
						MidCode.add(type_tmp == INT_SYM ? RET_I : RET_C, ZERO);
					}
				}
				hasRetrun = false;
				Table.delet(level + 1);
				strcpy(MidCode.func_name, "");
				cout << "At line " << lineNum << " This is a function!" << endl;
				getSym();
				return 0;
			}
			else if (curSym.getType() == LCURLY_SYM) {
				if (Table.insert(node) != 0) {
					reportError(E_REDEF);
					return 0;
				}
				MidCode.add(FUNC_DEF, node.id.name, node.type);
				strcpy(MidCode.func_name, node.id.name);
				getSym();
				recgCompound(level + 1);
				if (curSym.getType() != RCURLY_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
				}
				if (!hasRetrun) {
					if (type_tmp == VOID_SYM) MidCode.add(RET_V);
					else {
						reportError(E_NO_RET);
						MidCode.add(type_tmp == INT_SYM ? RET_I : RET_C, ZERO);
					}
				}
				hasRetrun = false;
				Table.delet(level + 1);
				strcpy(MidCode.func_name, "");
				cout << "At line " << lineNum << " This is a function!" << endl;
				getSym();
				return 0;
			}
			else {
				if (type_tmp == VOID_SYM) {
					reportError(E_MISS_TOKEN);
					char until[2] = { ';','\n' };
					skip(until, 2);
				}
				else throw(E_NOT_THIS);
			}
		}
		else throw(E_NOT_THIS);
	}
	catch (int) {
		seekSp(sp);
		backSym(n, type);
		curChar = sourceFile.get();
		getSym();
		return -E_NOT_THIS;
	}
}