#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgAssign(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != ID_SYM) throw E_NOT_THIS;
		struct tableNode node;
		node.id.lev = level;
		strcpy(node.id.name, curSym.getValueStr());
		int i = Table.find(node.id, &node);
		if (i != 0) {
			reportError(E_UNDEF);
			char until[2] = { ';','\n' };
			skip(until, 2);
			return 0;
		}
		if (node.kind == TB_IFUNC || node.kind == TB_NFUNC) throw E_NOT_THIS;
		else if (node.kind == TB_VAR) {
			//是变量
			getSym();
			if (curSym.getType() != ASSIGN_SYM) MISS_TOKEN;
			getSym();
			struct tableNode * tmp = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(tmp->id.name, TMP_VAR);
			recgExpression(level, tmp);
			if (tmp->type != node.type) {
				reportError(E_UN_MATCH);
			}
			if(curState==0) {
				MidCode.add(ADD, node.id.name, tmp->id.name, ZERO);
			}
			cout << "At line " << lineNum << " This is a assignment!" << endl;
			return 0;
		}
		else if (node.kind == TB_ARRAY) {
			//是数组
			getSym();
			if (curSym.getType() != LBRACKET_SYM) MISS_TOKEN;
			getSym();
			struct tableNode * shift = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(shift->id.name, TMP_VAR);
			recgExpression(level, shift);
			if (shift->type == INT_SYM) {
				if (shift->val != EXCEPTVALUE && (shift->val >= node.len || shift->val < 0)) {
					reportError(E_OUT_BOUND);
				}
			}
			else {
				reportError(E_UN_MATCH);
			}
			if (curSym.getType() != RBRACKET_SYM) MISS_TOKEN;
			getSym();
			if (curSym.getType() != ASSIGN_SYM) MISS_TOKEN;
			getSym();
			struct tableNode * data = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(data->id.name, TMP_VAR);
			recgExpression(level, data);
			if (data->type != node.type) reportError(E_UN_MATCH);
			if (curState == 0) {
				MidCode.add(data->type == INT_SYM ? SAVE_I : SAVE_C, data->id.name, node.id.name, shift->id.name);
				cout << "At line " << lineNum << " This is a assignment!" << endl;
			}
			return 0;
		}
		else {
			reportError(E_VAL_VAR);
			char until[2] = { ';','\n' };
			skip(until, 2);
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