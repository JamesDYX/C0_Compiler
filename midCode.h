#ifndef MIDCODE_H
#define MIDCODE_H
#include<cstdlib>
#include<cstring>
#include "symbolTable.h"
#pragma warning(disable : 4996)
#define STR_MAX_LEN 1000
#define ZERO (char *)"&zero"

#define CONST_I 1	//���ͳ�������
#define CONST_C 2	//�ַ��ͳ�������
#define FUNC_DEF 3		//��������
#define PARA_I 4		//���Ͳ���
#define PARA_C 5		//�ַ��Ͳ���

#define VAR_I 6		//���ͱ�������
#define VAR_C 7		//�ַ���������
#define ARRAY_I 8	//�������鶨��
#define ARRAY_C	9	//�ַ����鶨��

#define ADD 10
#define SUB 11
#define MULT 12
#define DIV 13
#define LOAD_I 14
#define LOAD_C 15
#define SAVE_I 16
#define SAVE_C 17

#define CALL 18		//���ú���
#define GET_RET_I 19		//���غ���
#define GET_RET_C 34
#define PUSH_I 20		//���ú���ǰ������ѹջ
#define PUSH_C 35		//���ú���ǰ������ѹջ

#define BEQ 21		//������ת
#define BNE 22
#define BGT 23
#define BGE 24

#define LAB 25		//���ɱ�ǩ

#define READ_I 26	//������
#define READ_C 27	//���ַ�

#define PRINT_S 28
#define PRINT_I 29
#define PRINT_C 30

#define RET_V 31	//���ؿ�
#define RET_I 32	//��������
#define RET_C 33	//�����ַ�����

#define JUMP 36
#define NOP 37

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

	struct tetraCode* next;	//�����������ʽ��֯��Ԫʽ
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
	
	struct tetraCode* add(int op, char* rd, int rs_num);	//������ rd ����
	struct tetraCode* add(int op, char* rd = (char*)NULL, char* rs = (char*)NULL, char* rt = (char*)NULL);	//������ rd rs rt
	struct tetraCode* add(struct tetraCode* code);	//ֱ�������Ԫʽ
	
	int add_head(char* dst_name, char* src_name);

	/*Ϊ��֧�ַ����������м���룬������º���(����switch���ͺ����������)
	 *���ڵ���뵽ָ����headΪ��ͷ������
	*/
	struct tetraCode* addn(struct tetraCode& head, int op, char* rd, int rs_num);
	struct tetraCode* addn(struct tetraCode& head, int op, char* rd = (char*)NULL, char* rs = (char*)NULL, char* rt = (char*)NULL);
	//��head��ͷ�������뵽������
	struct tetraCode* insert(struct tetraCode head, struct tetraCode*node_pre);

	void outPut(ofstream &tetraCodeFile);
	void outPut(tetraCode code, ofstream & tetraCodeFile);

};

extern midCode MidCode;

#endif