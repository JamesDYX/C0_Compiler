#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgIFuncCall(int level, struct tableNode * tmpnode) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != ID_SYM) throw E_NOT_THIS;
		struct tableNode node;
		strcpy(node.id.name, curSym.getValueStr());
		node.id.lev = 0;
		if (Table.find(node.id, &node) != 0 || node.kind!=TB_IFUNC) throw E_NOT_THIS;
		struct tetraCode head;
		head.next = NULL;

		if(node.paramNum != 0) {
			getSym();
			if (curSym.getType() != LPAREN_SYM) {
				reportError(E_INEF_PARA);
				curState = E_INEF_PARA;
			}
			getSym();
			int len;
			recgValueParaList(level, head, &len, node);
			if (len < node.paramNum) {
				reportError(E_INEF_PARA);
				curState = E_INEF_PARA;
			}
			else if (len > node.paramNum) {
				reportError(E_SUR_PARA);
				curState = E_SUR_PARA;
			}
			if (curSym.getType() != RPAREN_SYM) MISS_TOKEN
		}

		if (curState == 0) {
			MidCode.addn(head, CALL, node.id.name, node.paramNum);
			MidCode.insert(head, MidCode.code_now);
			MidCode.add(node.type==INT_SYM?GET_RET_I:GET_RET_C, tmpnode->id.name);
		}
		tmpnode->type = node.type;
		cout << "At line " << lineNum << " This is a function call with return value!" << endl;
		getSym();
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

