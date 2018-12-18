#include "midCode.h"
#include <cstdio>
#pragma warning(disable : 4996)
#include <fstream>
using namespace std;

char s[] = "";
midCode MidCode(&Table, s);

void midCode::outPut(ofstream &tetraCodeFile) {
	tetraCode * curCode = &(this->code_head);
	while (curCode != 0) {
		outPut(*curCode, tetraCodeFile);
		curCode = curCode->next;
	}
}

void midCode::outPut(tetraCode code, ofstream & tetraCodeFile) {
	switch (code.op) {
	case(CONST_I): 
	{
		tetraCodeFile << "const int " << code.rd << " = " << code.rs_num << endl;
		break;
	}
	case(CONST_C):
	{
		tetraCodeFile << "const char " << code.rd << " = " << '0'+code.rs_num-48 << endl;
		break;
	}
	case (FUNC_DEF):
	{
		tetraCodeFile << ((code.rs_num == 20) ? "int " : (code.rs_num == 22) ? "void " : "char ") << code.rd << "()" << endl;
		break;
	}
	case (PARA_I):
	{
		tetraCodeFile << "para int " << code.rd << endl;
		break;
	}
	case (PARA_C):
	{
		tetraCodeFile << "para char " << code.rd << endl;
		break;
	}
	case (VAR_I):
	{
		tetraCodeFile << "var int " << code.rd << endl;
		break;
	}
	case (VAR_C):
	{
		tetraCodeFile << "var char " << code.rd << endl;
		break;
	}
	case (ARRAY_I):
	{
		tetraCodeFile << "int array " << code.rd << "[" << code.rs_num << "]" << endl;
		break;
	}
	case (ARRAY_C):
	{
		tetraCodeFile << "char array " << code.rd << "[" << code.rs_num << "]" << endl;
		break;
	}
	case (ADD):
	{
		tetraCodeFile << code.rd << " = " << code.rs << " + " << code.rt << endl;
		break;
	}
	case (SUB):
	{
		tetraCodeFile << code.rd << " = " << code.rs << " - " << code.rt << endl;
		break;
	}
	case (MULT):
	{
		tetraCodeFile << code.rd << " = " << code.rs << " * " << code.rt << endl;
		break;
	}
	case (DIV):
	{
		tetraCodeFile << code.rd << " = " << code.rs << " / " << code.rt << endl;
		break;
	}
	case (LOAD_I):
	case (LOAD_C):
	{
		tetraCodeFile << code.rd << " = " << code.rs << "[" << code.rt << "]" << endl;
		break;
	}
	case (SAVE_I):
	case (SAVE_C):
	{
		tetraCodeFile << code.rs << "[" << code.rt << "]" << " = " << code.rd << endl;
		break;
	}
	case (CALL):
	{
		tetraCodeFile << "call " << code.rd << endl;
		break;
	}
	case (GET_RET_C):
	case (GET_RET_I):
	{
		tetraCodeFile << code.rd << " = RET" << endl;
		break;
	}
	case (PUSH_C):
	case (PUSH_I):
	{
		tetraCodeFile << "PUSH " << code.rd << endl;
		break;
	}
	case (BEQ):
	{
		tetraCodeFile << "BEQ " << code.rs << " " << code.rt << " " << code.rd << endl;
		break;
	}
	case (BNE):
	{
		tetraCodeFile << "BNE " << code.rs << " " << code.rt << " " << code.rd << endl;
		break;
	}
	case (BGT):
	{
		tetraCodeFile << "BGT " << code.rs << " " << code.rt << " " << code.rd << endl;
		break;
	}
	case (BGE):
	{
		tetraCodeFile << "BGE " << code.rs << " " << code.rt << " " << code.rd << endl;
		break;
	}
	case (JUMP):
	{
		tetraCodeFile << "GOTO " << code.rd << endl;
		break;
	}
	case (LAB):
	{
		tetraCodeFile << code.rd << ":" << endl;
		break;
	}
	case (READ_I):
	{
		tetraCodeFile << code.rd << " = _GetI()" << endl;
		break;
	}
	case (READ_C):
	{
		tetraCodeFile << code.rd << " = _GetC()" << endl;
		break;
	}
	case (PRINT_I):
	case (PRINT_C):
	case (PRINT_S):
	{
		tetraCodeFile << "PRINT " << code.rd << endl;
		break;
	}
	case (RET_C):
	case (RET_I):
	{
		tetraCodeFile << "ret " << code.rd << endl;
		break;
	}
	case (RET_V):
	{
		tetraCodeFile << "ret"<< endl;
		break;
	}
	default:
	{
		break;
	}
	}
}

struct tetraCode* midCode::add(int op, char* rd, int rs_num)
{
	struct tetraCode * code = (struct tetraCode*)malloc(sizeof(struct tetraCode));
	code->op = op;

	this->add_head(code->rd, rd);

	code->rs_num = rs_num;
	code->next = NULL;

	this->code_now->next = code;
	this->code_now = code;
	return code;

}


int midCode::add_head(char* dst_name, char* src_name)
{
	if (this->extend_name == 0)
	{
		if (src_name != NULL)
		{
			strcpy(dst_name, src_name);
		}
		else {
			dst_name = NULL;
		}
		return 0;
	}
	struct tableNode node;

	if (src_name == NULL)
	{
		strcpy(dst_name, "");
		return 0;
	}
	if (0 == strcmp(src_name, ZERO)) //若恒为0
	{
		sprintf(dst_name, ":%s", src_name);
	}
	else
		if (src_name[0] == '&') //&是临时变量, ^是标签(没有放入符号表，也不需要)
		{
			sprintf(dst_name, "%s:%s", this->func_name, src_name);
		}
		else if (src_name[0] == '$') {
			sprintf(dst_name, "%s", src_name);
		}
		else
		{
			strcpy(node.id.name, src_name);
			node.id.lev = 1;
			this->sym_table->find(node.id, &node);
			if(node.id.lev==1)//是局部变量
				sprintf(dst_name, "%s:%s", this->func_name, src_name);
			else  //是全局变量
				sprintf(dst_name, ":%s", src_name);
		}
	return 0;
}
struct tetraCode* midCode::add(int op, char* rd, char* rs, char* rt)
{


	struct tetraCode * code = (struct tetraCode*)malloc(sizeof(struct tetraCode));
	code->op = op;

	if(rs!=NULL) this->add_head(code->rs, rs);
	if(rt != NULL)this->add_head(code->rt, rt);
	if (rd != NULL)this->add_head(code->rd, rd);

	this->code_now->next = code;
	this->code_now = code;
	code->next = NULL;

	return code;
}

struct tetraCode* midCode::add(struct tetraCode* code)
{

	struct tetraCode * new_code = (struct tetraCode*)malloc(sizeof(struct tetraCode));
	*new_code = *code;
	this->code_now->next = new_code;
	new_code->next = NULL;

	this->code_now = new_code;
	return new_code;
}

//为了支持非线性生成中间代码，添加以下函数(用于do-for语句和函数调用语句)
//将节点插入到指定的head为开头的链中
struct tetraCode*  midCode::addn(struct tetraCode& head, int op, char* rd, int rs_num)
{
	struct tetraCode* next = &head;
	while (next->next != NULL)
		next = next->next;
	struct tetraCode* new_node = (struct tetraCode*)malloc(sizeof(struct tetraCode));
	new_node->next = NULL;
	new_node->op = op;
	this->add_head(new_node->rd, rd);
	new_node->rs_num = rs_num;
	next->next = new_node;
	return new_node;
}
struct tetraCode* midCode::addn(struct tetraCode& head, int op, char* rd, char* rs, char* rt)
{

	struct tetraCode* next = &head;
	while (next->next != NULL)
		next = next->next;
	struct tetraCode* new_node = (struct tetraCode*)malloc(sizeof(struct tetraCode));
	new_node->next = NULL;
	new_node->op = op;
	this->add_head(new_node->rd, rd);
	this->add_head(new_node->rs, rs);
	this->add_head(new_node->rt, rt);
	next->next = new_node;
	return new_node;
}

//将head开头的链插入到主链中
struct tetraCode* midCode::insert(struct tetraCode head, struct tetraCode *node_pre)
{
	struct tetraCode* node_next = node_pre->next, *next = &head;
	if (head.next == NULL)
		return NULL;
	while (next->next != NULL)
		next = next->next;
	node_pre->next = head.next;
	next->next = node_next;
	if (node_next == NULL)
		this->code_now = next; //插到尾部的话，要改变code_now指向尾部
	return head.next;

}

