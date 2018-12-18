#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
bool hasRetrun = false;
int recgReturn(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != RETURN_SYM) throw E_NOT_THIS;
		getSym();

		struct tableNode node;
		node.id.lev = 0;
		strcpy(node.id.name, MidCode.func_name);
		int i = Table.find(node.id, &node);
		
		if (node.type == VOID_SYM) {
			if(curSym.getType()!=SEMICOL_SYM) {
					reportError(E_RET);			
					char until[2] = { ';','\n' };		
					skip(until, 2);
					return 0;
			}
			else {
				MidCode.add(RET_V);
				cout << "At line " << lineNum << " This is a return statement!" << endl;
				hasRetrun = true;
				return 0;
			}
		}
		else {
			if (curSym.getType() != LPAREN_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			struct tableNode * result = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(result->id.name, TMP_VAR);
			recgExpression(level, result);
			if (curSym.getType() != RPAREN_SYM) {
				MISS_TOKEN;
				return 0;
			}
			if (node.type != result->type) {
				reportError(E_RET);
				curState = E_RET;
			}
			if (curState == 0) {
				MidCode.add(node.type == INT_SYM ? RET_I : RET_C, result->id.name);
			}
			getSym();
			cout << "At line " << lineNum << " This is a return statement!" << endl;
			hasRetrun = true;
			return 0;
		}
	}
	catch (int) {
		seekSp(sp_tmp);
		backSym(n_tmp, type_tmp);
		curChar = sourceFile.get();
		getSym();
		return -E_NOT_THIS;
	}
}