#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
using namespace std;

int recgValIntro(int level) {
	int curState = 0;
	streampos sp = sourceFile.tellg();
	int n = curSym.len;
	int type = curSym.getType();
	try {
		if (curSym.getType() != CONST_SYM) {
			throw(E_NOT_THIS);
		}
		getSym();
		recgValDef(level);
		if (curSym.getType() != SEMICOL_SYM) {
			reportError(E_MISS_TOKEN);
			char until[2] = { ';','\n' };
			skip(until, 2);
			curState = E_MISS_TOKEN;
		}
		sp = sourceFile.tellg();
		while (true) {
			getSym();
			if (curSym.getType() != CONST_SYM)
				break;
			getSym();
			recgValDef(level);
			if (curSym.getType() != SEMICOL_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
				curState = E_MISS_TOKEN;
			}
			sp = sourceFile.tellg();
		}
		seekSp(sp);
		curChar = sourceFile.get();
		cout << "At line " << lineNum << " This is a const declaration statement!" << endl;
		getSym();
		return 0;
	}
	catch (int) {
		seekSp(sp);
		backSym(n, type);
		curChar = sourceFile.get();
		getSym();
		return -E_NOT_THIS;
	}
}