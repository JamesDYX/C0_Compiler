#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgStatement(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (recgIf(level) != -E_NOT_THIS) return 0;
		if (recgRepeat(level) != -E_NOT_THIS) return 0;
		if (curSym.getType() == LCURLY_SYM) {
			getSym();
			while (true)
			{
				if (recgStatement(level) == -E_NOT_THIS) break;
			}
			if (curSym.getType() != RCURLY_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		if (recgAssign(level) != -E_NOT_THIS) {
			if (curSym.getType() != SEMICOL_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		if (recgRead(level) != -E_NOT_THIS) {
			if (curSym.getType() != SEMICOL_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		if (recgWrite(level) != -E_NOT_THIS) {
			if (curSym.getType() != SEMICOL_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		if (recgReturn(level) != -E_NOT_THIS) {
			if (curSym.getType() != SEMICOL_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		if (recgNFuncCall(level) != -E_NOT_THIS) {
			if (curSym.getType() != SEMICOL_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		if (curSym.getType() == SEMICOL_SYM) {
			getSym();
			return 0;
		}
		struct tableNode * result = (struct tableNode *)malloc(sizeof(struct tableNode));
		strcpy(result->id.name, TMP_VAR);
		if (recgIFuncCall(level, result) != -E_NOT_THIS) {
			if (curSym.getType() != SEMICOL_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
			return 0;
		}
		throw E_NOT_THIS;
	}
catch (int) {
	seekSp(sp_tmp);
	backSym(n_tmp, type_tmp);
	curChar = sourceFile.get();
	getSym();
	return -E_NOT_THIS;
}
}