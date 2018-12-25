#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;

int recgFactor(int level, struct tableNode * tmpnode) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		int isPos = true;
		if (curSym.getType() == PLUS_SYM || curSym.getType() == MINIUS_SYM || curSym.getType() == UNSNUM_SYM) {		//整数
			tmpnode->type = INT_SYM;
			if (curSym.getType() == PLUS_SYM || curSym.getType() == MINIUS_SYM) {
				if (curSym.getType() == MINIUS_SYM) isPos = false;
				getSym();
				if(curSym.getType()!=UNSNUM_SYM) MISS_TOKEN
			}
			tmpnode->val = isPos ? curSym.getValueNum() : 0 - curSym.getValueNum();
			if (curState==0) MidCode.add(CONST_I, tmpnode->id.name, isPos ? curSym.getValueNum() : 0 - curSym.getValueNum());
			cout << "At line " << lineNum << " This is a factor!" << endl;
			getSym();
			return 0;
		}
		else if (curSym.getType() == CHARACH_SYM) {
			tmpnode->type = CHAR_SYM;
			tmpnode->val = curSym.getValueCh();
			MidCode.add(CONST_C, tmpnode->id.name, curSym.getValueCh());
			cout << "At line " << lineNum << " This is a factor!" << endl;
			getSym();
			return 0;
		}
		else if (curSym.getType() == LPAREN_SYM) {
			getSym();
			recgExpression(level, tmpnode);
			if (curSym.getType() != RPAREN_SYM) MISS_TOKEN;
			tmpnode->type = INT_SYM;
			tmpnode->val = EXCEPTVALUE;
			cout << "At line " << lineNum << " This is a factor!" << endl;
			getSym();
			return 0;
		}
		else if (curSym.getType() == ID_SYM) {
			struct tableNode node;
			node.id.lev = level;
			strcpy(node.id.name, curSym.getValueStr());
			int i = Table.find(node.id, &node);
			if (i != 0 || node.kind==TB_NFUNC) {
				tmpnode->type = INT_SYM;
				reportError(i!=0?E_UNDEF:E_UN_MATCH);
				char until[2] = { ';','\n' };
				skip(until, 2);
				return 0;
			}
			if (node.kind==TB_ARRAY) {
				//数组
				tmpnode->type = node.type;
				getSym();
				if(curSym.getType()!=LBRACKET_SYM) MISS_TOKEN
				getSym();
				struct tableNode * shift = (struct tableNode *)malloc(sizeof(struct tableNode));
				strcpy(shift->id.name, TMP_VAR);
				recgExpression(level, shift);
				if (shift->type == INT_SYM) {
					if (shift->val != EXCEPTVALUE && (shift->val >= node.len || shift->val < 0)) {
						reportError(E_OUT_BOUND);
					}
				}
				else {
					reportError(E_UN_MATCH);
				}
				if (curSym.getType() != RBRACKET_SYM) MISS_TOKEN;
				if (curState == 0) {
					if (node.type == INT_SYM) MidCode.add(LOAD_I, tmpnode->id.name, node.id.name, shift->id.name);
					else MidCode.add(LOAD_C, tmpnode->id.name, node.id.name, shift->id.name);
				}
				tmpnode->val = EXCEPTVALUE;
				cout << "At line " << lineNum << " This is a factor!" << endl;
				getSym();
				return 0;
			}
			else if (node.kind == TB_CONST || node.kind == TB_VAR) {
				//变量或常量
				if (node.kind == TB_CONST) tmpnode->val = node.val;
				else tmpnode->val = EXCEPTVALUE;
				tmpnode->type = node.type;
				MidCode.add(ADD, tmpnode->id.name, node.id.name, ZERO);
				cout << "At line " << lineNum << " This is a factor!" << endl;
				getSym();
				return 0;
			}
			else {
				//有返回值函数调用
				recgIFuncCall(level, tmpnode);
				tmpnode->val = EXCEPTVALUE;
				cout << "At line " << lineNum << " This is a factor!" << endl;
				return 0;
			}
		}
		else throw(E_NOT_THIS);
	}
	catch (int) {
		seekSp(sp_tmp);
		backSym(n_tmp, type_tmp);
		curChar = sourceFile.get();
		getSym();
		return -E_NOT_THIS;
	}
}