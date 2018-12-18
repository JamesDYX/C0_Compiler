#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgRepeat(int level) {
	int curState = 0;
	streampos sp_tmp = sourceFile.tellg();
	int n_tmp = curSym.len;
	int type_tmp = curSym.getType();
	try {
		if (curSym.getType() == WHILE_SYM) {
			char whileLable_1[100], whileLable_2[100];
			strcpy(whileLable_1, LABLE);
			strcpy(whileLable_2, LABLE);
			MidCode.add(LAB, whileLable_1);
			getSym();
			if (curSym.getType() != LPAREN_SYM) MISS_TOKEN;
			getSym();
			recgCondition(level, whileLable_2, false);
			if (curSym.getType() != RPAREN_SYM) MISS_TOKEN;
			getSym();
			recgStatement(level);
			if (curState == 0) {
				MidCode.add(JUMP, whileLable_1);
				cout << "At line " << lineNum << " This is a while-repeat statement!" << endl;
			}
			MidCode.add(LAB, whileLable_2);
			return 0;
		}
		else if (curSym.getType() == FOR_SYM) {
			getSym();
			if (curSym.getType() != LPAREN_SYM) MISS_TOKEN;
			getSym();
			if (curSym.getType() != ID_SYM) MISS_TOKEN;
			struct tableNode node;
			node.id.lev = level;
			strcpy(node.id.name, curSym.getValueStr());
			int i = Table.find(node.id, &node);
			if (i != 0 || node.kind!=TB_VAR) {
				reportError(i!=0?E_UNDEF:E_UN_MATCH);
				char until[2] = { ';','\n' };
				skip(until, 2);
			}
			else {
				getSym();
				if (curSym.getType() != ASSIGN_SYM) MISS_TOKEN;
				getSym();
				int type = node.type;
				recgExpression(level, &node);
				if (type != node.type) {
					node.type = type;
					reportError(E_UN_MATCH);
				}
			}
			if (curSym.getType() != SEMICOL_SYM) MISS_TOKEN;
			struct tetraCode head2;
			struct tetraCode * head1;
			struct tetraCode nop;
			nop.next = NULL;
			nop.op = NOP;
			head1 = MidCode.code_now;
			MidCode.add(&nop);
			head2.next = NULL;
			getSym();
			char forLable[100];
			strcpy(forLable, LABLE);
			recgCondition(level, forLable, true);
			MidCode.code_now = head1;
			head1 = head1->next;
			MidCode.code_now->next = NULL;
			if (curSym.getType() != SEMICOL_SYM) MISS_TOKEN;
			getSym();
			if (curSym.getType() != ID_SYM) MISS_TOKEN;
			struct tableNode node1;
			node1.id.lev = level;
			strcpy(node1.id.name, curSym.getValueStr());
			int ii = Table.find(node1.id, &node1);
			if(ii==0 && node1.kind == TB_VAR && node1.type == INT_SYM) {
				getSym();
				if (curSym.getType() != ASSIGN_SYM) MISS_TOKEN;
				getSym();
				struct tableNode node2;
				node2.id.lev = level;
				strcpy(node2.id.name, curSym.getValueStr());
				int i = Table.find(node2.id, &node2);
				if (i != 0) {
					reportError(E_UNDEF);
					char until[3] = { ';','\n',')' };
					skip(until, 3);
					return 0;
				}
				else if (node2.kind == TB_NFUNC || node2.kind == TB_ARRAY) {
					reportError(E_UN_MATCH);
					char until[3] = { ';','\n',')' };
					skip(until, 3);
					return 0;
				}
				else if (node2.kind == TB_IFUNC && node2.paramNum != 0) {
					reportError(E_INEF_PARA);
					char until[3] = { ';','\n',')' };
					skip(until, 3);
					return 0;
				}
				else {
					struct tableNode * func = (struct tableNode *)malloc(sizeof(struct tableNode));
					strcpy(func->id.name, TMP_VAR);
					if (node2.kind == TB_IFUNC) {
						MidCode.addn(head2, CALL, node2.id.name, node2.paramNum);
						MidCode.addn(head2, node2.type==INT_SYM?RET_I:RET_C, func->id.name);
					}
					getSym();
					bool isPos = true;
					if (curSym.getType() == PLUS_SYM) isPos = true;
					else if (curSym.getType() == MINIUS_SYM) isPos = false;
					else MISS_TOKEN;
					getSym();
					if (curSym.getType() != UNSNUM_SYM) MISS_TOKEN;
					struct tableNode * stride = (struct tableNode *)malloc(sizeof(struct tableNode));
					strcpy(stride->id.name, TMP_VAR);
					MidCode.addn(head2, CONST_I, stride->id.name, curSym.getValueNum());
					if (node2.kind == TB_IFUNC) MidCode.addn(head2, isPos ? ADD : SUB, node1.id.name, func->id.name, stride->id.name);
					else MidCode.addn(head2, isPos ? ADD : SUB, node1.id.name, node2.id.name, stride->id.name);
				}
				getSym();
				if (curSym.getType() != RPAREN_SYM)MISS_TOKEN;
				getSym();
				MidCode.add(LAB, forLable);
				recgStatement(level);
				if (curState == 0) {
					MidCode.insert(head2, MidCode.code_now);
					MidCode.insert(*head1, MidCode.code_now);
					cout << "At line " << lineNum << " This is a for-repeat statement!" << endl;
				}
				return 0;
			}
			else {
				reportError(i != 0 ? E_UNDEF : E_UN_MATCH);
				char until[3] = { ';','\n',')' };
				skip(until, 3);
				return 0;
			}
		}
		else throw E_NOT_THIS;
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