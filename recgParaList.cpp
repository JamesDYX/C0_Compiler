#include <iostream>
#include "syntaxAnalys.h"
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgParaList(int level, struct tableNode * prenode) {
	int state = 0;
	int type = curSym.getType();
	if (type != INT_SYM && type != CHAR_SYM) {
		reportError(E_MISS_TOKEN);
		char until[2] = { ';','\n' };
		skip(until, 2);
		state = E_MISS_TOKEN;
	}
	struct tableNode node;
	getSym();
	if (curSym.getType() != ID_SYM) {
		reportError(E_MISS_TOKEN);
		char until[2] = { ';','\n' };
		skip(until, 2);
		state = E_MISS_TOKEN;
	}
	node.id.lev = level;
	strcpy_s(node.id.name, curSym.getValueStr());
	node.kind = TB_VAR;
	node.type = type;
	if (state == 0) {
		if (Table.insert(node) != 0)
			reportError(E_REDEF);
		else {
			prenode->param[prenode->paramNum] = (struct tableNode*)malloc(sizeof(struct tableNode));;
			*(prenode->param[prenode->paramNum]) = node;
			prenode->paramNum = prenode->paramNum + 1;
		}
	}
	streampos sp = sourceFile.tellg();
	while (true)
	{
		state = 0;
		getSym();
		if (curSym.getType() != COMMA_SYM)
			break;
		getSym();
		type = curSym.getType();
		if (type != INT_SYM && type != CHAR_SYM) break;
		getSym();
		if (curSym.getType() != ID_SYM) {
			reportError(E_MISS_TOKEN);
			char until[2] = { ';','\n' };
			skip(until, 2);
			state = E_MISS_TOKEN;
		}
		node.id.lev = level;
		strcpy_s(node.id.name, curSym.getValueStr());
		node.kind = TB_VAR;
		node.type = type;
		if (state == 0) {
			if (Table.insert(node) != 0)
				reportError(E_REDEF);
			else {
				prenode->param[prenode->paramNum] = (struct tableNode*)malloc(sizeof(struct tableNode));;
				*(prenode->param[prenode->paramNum]) = node;
				prenode->paramNum = prenode->paramNum + 1;
			}
		}
		sp = sourceFile.tellg();
	}
	seekSp(sp);
	curChar = sourceFile.get();
	cout << "At line " << lineNum << " This is a parameter list!" << endl;
	getSym();
	return 0;
}