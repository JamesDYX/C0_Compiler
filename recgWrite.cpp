#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgWrite(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != PRINTF_SYM) throw E_NOT_THIS;
		getSym();
		if (curSym.getType() != LPAREN_SYM) MISS_TOKEN;
		getSym();
		if (curSym.getType() == STR_SYM) {
			MidCode.add(PRINT_S, curSym.getValueStr());
			getSym();
			if (curSym.getType() == RPAREN_SYM) {
				char tmp_s[100] = "\\n";
				MidCode.add(PRINT_S, tmp_s);
				getSym();
				cout << "At line " << lineNum << " This is a print statement!" << endl;
				return 0;
			}
			else if (curSym.getType() != COMMA_SYM) {
				MISS_TOKEN;
				return 0;
			}
			getSym();
		}
		struct tableNode * node = (struct tableNode *)malloc(sizeof(struct tableNode));
		strcpy(node->id.name, TMP_VAR);
		recgExpression(level, node);
		MidCode.add(node->type==INT_SYM?PRINT_I:PRINT_C, node->id.name);
		if (curSym.getType() != RPAREN_SYM) {
			MISS_TOKEN;
			return 0;
		}
		char tmp_s[100] = "\\n";
		MidCode.add(PRINT_S, tmp_s);
		getSym();
		cout << "At line " << lineNum << " This is a print statement!" << endl;
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