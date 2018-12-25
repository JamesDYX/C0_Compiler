#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgMain(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() != VOID_SYM) throw E_NOT_THIS;
		getSym();
		if (curSym.getType() != MAIN_SYM) throw E_NOT_THIS;

		struct tableNode node;
		node.id.lev = level;
		strcpy(node.id.name, curSym.getValueStr());
		node.kind = TB_NFUNC;
		node.type = VOID_SYM;
		node.paramNum = 0;
		Table.insert(node);
		MidCode.add(FUNC_DEF, node.id.name, node.type);
		strcpy(MidCode.func_name, node.id.name);

		getSym();
		if (curSym.getType() != LPAREN_SYM) MISS_TOKEN;
		getSym();
		if (curSym.getType() != RPAREN_SYM) MISS_TOKEN;
		getSym();
		if (curSym.getType() != LCURLY_SYM) MISS_TOKEN;
		getSym();
		recgCompound(level+1);
		if (curSym.getType() != RCURLY_SYM) MISS_TOKEN;
		if (!hasRetrun) MidCode.add(RET_V);
		hasRetrun = false;
		Table.delet(level + 1);
		strcpy(MidCode.func_name, "");
		getSym();
		cout << "At line " << lineNum << " This is a Main function!" << endl;
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