#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgIf(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != IF_SYM) throw E_NOT_THIS;
		getSym();
		if (curSym.getType() != LPAREN_SYM) MISS_TOKEN;
		getSym();
		char ifLabel_1[100];
		strcpy(ifLabel_1, LABLE);
		recgCondition(level, ifLabel_1, false);
		if (curSym.getType() != RPAREN_SYM) MISS_TOKEN;
		getSym();
		recgStatement(level);
		if (curSym.getType() == ELSE_SYM) {
			char ifLabel_2[100];
			strcpy(ifLabel_2, LABLE);
			MidCode.add(JUMP, ifLabel_2);
			MidCode.add(LAB, ifLabel_1);
			getSym();
			recgStatement(level);
			MidCode.add(LAB, ifLabel_2);
		}
		else {
			MidCode.add(LAB, ifLabel_1);
		}
		cout << "At line " << lineNum << " This is a if-condition statement!" << endl;
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