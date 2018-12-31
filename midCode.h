#ifndef MIDCODE_H
#define MIDCODE_H
#include<cstdlib>
#include<cstring>
#include "symbolTable.h"
#pragma warning(disable : 4996)
#define STR_MAX_LEN 1000
#define ZERO (char *)"&zero"

#define CONST_I 1	//整型常量定义
#define CONST_C 2	//字符型常量定义
#define FUNC_DEF 3		//函数定义
#define PARA_I 4		//整型参数
#define PARA_C 5		//字符型参数

#define VAR_I 6		//整型变量定义
#define VAR_C 7		//字符变量定义
#define ARRAY_I 8	//整型数组定义
#define ARRAY_C	9	//字符数组定义

#define ADD 10
#define SUB 11
#define MULT 12
#define DIV 13
#define LOAD_I 14
#define LOAD_C 15
#define SAVE_I 16
#define SAVE_C 17

#define CALL 18		//调用函数
#define GET_RET_I 19		//返回函数
#define GET_RET_C 34
#define PUSH_I 20		//调用函数前将参数压栈
#define PUSH_C 35		//调用函数前将参数压栈

#define BEQ 21		//各种跳转
#define BNE 22
#define BGT 23
#define BGE 24

#define LAB 25		//生成标签

#define READ_I 26	//读整数
#define READ_C 27	//读字符

#define PRINT_S 28
#define PRINT_I 29
#define PRINT_C 30

#define RET_V 31	//返回空
#define RET_I 32	//返回整型
#define RET_C 33	//返回字符类型

#define JUMP 36
#define NOP 37

#define LI 38

struct tetraCode
{
	int op;
	union {
		char rd[STR_MAX_LEN];
		int rd_num;
	};
	union {
		char rs[STR_MAX_LEN];
		int rs_num;
	};
	union {
		char rt[STR_MAX_LEN];
		int rt_num;
	};

	struct tetraCode* next;	//采用链表的形式组织四元式
};

class midCode
{
public:
	char func_name[1000];
	symbolTable * sym_table;
	struct tetraCode code_head;
	struct tetraCode* code_now;
	int extend_name;
	midCode(symbolTable* table, char* fname)
	{
		this->code_head.next = (struct tetraCode*)NULL;
		this->code_now = &(this->code_head);
		this->sym_table = table;
		strcpy(func_name, fname);
		this->extend_name = 1;
	}
	midCode()
	{
		this->code_head.next = (struct tetraCode*)NULL;
		this->code_now = &(this->code_head);
		this->extend_name = 0;
		strcpy(func_name, "");
	}
	
	struct tetraCode* add(int op, char* rd, int rs_num);	//操作符 rd 数字
	struct tetraCode* add(int op, char* rd = (char*)NULL, char* rs = (char*)NULL, char* rt = (char*)NULL);	//操作符 rd rs rt
	struct tetraCode* add(struct tetraCode* code);	//直接添加四元式
	
	int add_head(char* dst_name, char* src_name);

	/*为了支持非线性生成中间代码，添加以下函数(用于switch语句和函数调用语句)
	 *将节点插入到指定的head为开头的链中
	*/
	struct tetraCode* addn(struct tetraCode& head, int op, char* rd, int rs_num);
	struct tetraCode* addn(struct tetraCode& head, int op, char* rd = (char*)NULL, char* rs = (char*)NULL, char* rt = (char*)NULL);
	//将head开头的链插入到主链中
	struct tetraCode* insert(struct tetraCode head, struct tetraCode*node_pre);

	void outPut(ofstream &tetraCodeFile);
	void outPut(tetraCode code, ofstream & tetraCodeFile);

};

extern midCode MidCode;

#endif