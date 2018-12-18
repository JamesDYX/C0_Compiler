#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgTerm(int level, struct tableNode * tmpnode) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (recgFactor(level, tmpnode) == -E_NOT_THIS) throw(E_NOT_THIS);
		streampos sp = sourceFile.tellg();
		int n = curSym.len;
		int type = curSym.getType();
		bool found = false;
		while (true) {
			bool isStar = true;
			if (curSym.getType() != STAR_SYM && curSym.getType() != SLASH_SYM)
				break;
			tmpnode->val = EXCEPTVALUE;
			if (curSym.getType() != STAR_SYM) isStar = false;
			found = true;
			getSym();
			struct tableNode * tmpnode2 = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(tmpnode2->id.name, TMP_VAR);
			recgFactor(level, tmpnode2);
			
			if(isStar) MidCode.add(MULT, tmpnode->id.name, tmpnode->id.name, tmpnode2->id.name);
			else MidCode.add(DIV, tmpnode->id.name, tmpnode->id.name, tmpnode2->id.name);

			sp = sourceFile.tellg();
			n = curSym.len;
			type = curSym.getType();
		}
		if (found) tmpnode->type = INT_SYM;
		seekSp(sp);
		backSym(n, type);
		curChar = sourceFile.get();
		cout << "At line " << lineNum << " This is a term!" << endl;
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