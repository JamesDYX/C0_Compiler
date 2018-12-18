#include <iostream>
#include "syntaxAnalys.h"
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgVarIntro(int level) {
	int curState = 0;
	streampos sp = sourceFile.tellg();
	int n = curSym.len;
	int type = curSym.getType();
	try {
		if (recgVarDef(level) == -E_NOT_THIS) throw(E_NOT_THIS);
		if (curSym.getType() != SEMICOL_SYM) {
			reportError(E_MISS_TOKEN);
			char until[2] = { ';','\n' };
			skip(until, 2);
		}
		sp = sourceFile.tellg();
		getSym();
		while (recgVarDef(level) != -E_NOT_THIS)
		{
			if (curSym.getType() != SEMICOL_SYM) {
				reportError(E_MISS_TOKEN);
				char until[2] = { ';','\n' };
				skip(until, 2);
			}
			sp = sourceFile.tellg();
			getSym();
		}
		seekSp(sp);
		curChar = sourceFile.get();
		cout << "At line " << lineNum << " This is a variable declaration statement!" << endl;
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