#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgValueParaList(int level, struct tetraCode & head, int *len, struct tableNode func) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		*len = 0;
		struct tableNode * tmpnode = (struct tableNode *)malloc(sizeof(struct tableNode));
		strcpy(tmpnode->id.name, TMP_VAR);
		if (recgExpression(level, tmpnode) != 0) throw E_NOT_THIS;
		if (*len < func.paramNum) {
			if (func.param[*len]->type != tmpnode->type) reportError(E_PARA_LSIT);
			else MidCode.addn(head, tmpnode->type == INT_SYM ? PUSH_I : PUSH_C, tmpnode->id.name);
		}
		*len = 1;
		while (curSym.getType()==COMMA_SYM) {
			getSym();
			struct tableNode * tmpnode1 = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(tmpnode1->id.name, TMP_VAR);
			recgExpression(level, tmpnode1);
			if (*len < func.paramNum) {
				if (func.param[*len]->type != tmpnode1->type) reportError(E_PARA_LSIT);
				else MidCode.addn(head, tmpnode1->type == INT_SYM ? PUSH_I : PUSH_C, tmpnode1->id.name);
			}
			*len = (*len) + 1;
		}
		cout << "At line " << lineNum << " This is a value list!" << endl;
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