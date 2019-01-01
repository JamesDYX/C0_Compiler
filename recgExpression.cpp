#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgExpression(int level, struct tableNode * tmpnode) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		bool isPos = true;
		bool found = false;
		if (curSym.getType() == PLUS_SYM || curSym.getType() == MINIUS_SYM) {
			found = true;
			if(curSym.getType() == MINIUS_SYM) isPos = false;
			getSym();
		}
        struct tableNode * tmpnode1 = (struct tableNode *)malloc(sizeof(struct tableNode));
        strcpy(tmpnode1->id.name , TMP_VAR);
		if (recgTerm(level, tmpnode1) == -E_NOT_THIS) throw(E_NOT_THIS);
		tmpnode->kind = tmpnode1->kind;
		tmpnode->type = tmpnode1->type;
		if (!isPos) {
			tmpnode->val = EXCEPTVALUE;
			MidCode.add(SUB, tmpnode->id.name, ZERO, tmpnode1->id.name);
		}
		else{
		    tmpnode->val = tmpnode1->val;
            MidCode.add(ADD, tmpnode->id.name, tmpnode1->id.name, ZERO);
		}

		streampos sp = sourceFile.tellg();
		int n = curSym.len;
		int type = curSym.getType();

		while (true) {
			isPos = true;
			if (curSym.getType() != PLUS_SYM && curSym.getType() != MINIUS_SYM)
				break;
			tmpnode->val = EXCEPTVALUE;
			if (curSym.getType() == MINIUS_SYM) isPos = false;
			found = true;
			getSym();
			struct tableNode * tmpnode2 = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(tmpnode2->id.name , TMP_VAR);
			recgTerm(level, tmpnode2);

			if(isPos) MidCode.add(ADD, tmpnode->id.name, tmpnode->id.name, tmpnode2->id.name);
			else MidCode.add(SUB, tmpnode->id.name, tmpnode->id.name, tmpnode2->id.name);

			sp = sourceFile.tellg();
			n = curSym.len;
			type = curSym.getType();
		}
		if (found) tmpnode->type = INT_SYM;

		seekSp(sp);
		backSym(n, type);
		curChar = sourceFile.get();
		cout << "At line " << lineNum << " This is a expression!" << endl;
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