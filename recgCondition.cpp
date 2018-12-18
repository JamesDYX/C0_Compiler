#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgCondition(int level, char *  Label, bool satisfy) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		struct tableNode * left = (struct tableNode *)malloc(sizeof(struct tableNode));
		strcpy(left->id.name, TMP_VAR);
		if(recgExpression(level, left)==-E_NOT_THIS) throw E_NOT_THIS;
		switch (curSym.getType())
		{
		case EQUAL_SYM : {
			getSym();
			struct tableNode * right = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(right->id.name, TMP_VAR);
			recgExpression(level, right);
			MidCode.add(satisfy?BEQ :BNE, Label, left->id.name, right->id.name);
			break;
		}
		case NOTEQ_SYM: {
			getSym();
			struct tableNode * right = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(right->id.name, TMP_VAR);
			recgExpression(level, right);
			MidCode.add(satisfy?BNE:BEQ, Label, left->id.name, right->id.name);
			break;
		}
		case GREAT_SYM: {
			getSym();
			struct tableNode * right = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(right->id.name, TMP_VAR);
			recgExpression(level, right);
			satisfy? MidCode.add(BGT, Label, left->id.name, right->id.name) : MidCode.add(BGE, Label, right->id.name, left->id.name);
			break;
		}
		case GREATEQ_SYM: {
			getSym();
			struct tableNode * right = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(right->id.name, TMP_VAR);
			recgExpression(level, right);
			satisfy ? MidCode.add(BGE, Label, left->id.name, right->id.name) : MidCode.add(BGT, Label, right->id.name, left->id.name);
			break;
		}
		case LESS_SYM: {
			getSym();
			struct tableNode * right = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(right->id.name, TMP_VAR);
			recgExpression(level, right);
			satisfy ? MidCode.add(BGT, Label, right->id.name, left->id.name) : MidCode.add(BGE, Label, left->id.name, right->id.name);
			break;
		}
		case LESSEQ_SYM: {
			getSym();
			struct tableNode * right = (struct tableNode *)malloc(sizeof(struct tableNode));
			strcpy(right->id.name, TMP_VAR);
			recgExpression(level, right);
			satisfy ? MidCode.add(BGE, Label, right->id.name, left->id.name) : MidCode.add(BGT, Label, left->id.name, right->id.name);
			break;
		}
		default: {
			MidCode.add(satisfy ? BNE : BEQ, Label, left->id.name, ZERO);
			break;
		}
		}
		cout << "At line " << lineNum << " This is a condition statement!" << endl;
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
