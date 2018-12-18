#include <cstring>
#include <map>
#include "error.h"
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#define STR_MAX_LEN 1000
#define PAR_MAX_LEN 20

#define TB_IFUNC 1
#define TB_NFUNC 2
#define TB_CONST 3
#define TB_VAR 4
#define TB_ARRAY 5

#define TB_ERR_REDEF 1
#define TB_ERR_UNDEF 2
#define TB_ERR_UNMAT 3
#define TB_ERR_MISSRET 4
#define table_map map < struct tableNodeId, struct tableNode, struct tableCmp >		// tabelNodeId ======tabelCmp======> tabelNode

using namespace std;


struct tableNodeId { //���ű��еı�ʶ��id��ͨ�����ƺͼ���Ψһȷ��һ����
	char name[STR_MAX_LEN];
	int lev;		//���ڼ���
};

struct tableNode {

	struct tableNodeId id;
	int kind;		//���޷���ֵ�������������������������
	int type;		//int char void
	union {
		int val;	//������ֵ
		struct {	//���ڼ�¼�βε��βνṹ��
			int paramNum;	//�β�����
			struct tableNode *param[PAR_MAX_LEN];	//�βα�
		};
		struct {	//���ڼ�¼���������ṹ��
			int len;	//���鳤��

		};
	};

};

struct tableCmp {
	bool operator() (const struct tableNodeId & k1,
		const struct tableNodeId & k2) const {
		int re = strcmp(k1.name, k2.name);
		if (re < 0)
			return true;
		else {
			if (re > 0)
				return false;
			else {
				if (k1.lev > k2.lev)
					return true;
				else
					return false;
			}
		}
	}
};
class symbolTable{
public:
	table_map table;
	int find(struct tableNodeId id, struct tableNode *node);
	int insert(struct tableNode node, int force = 0);
	int delet(int lev);	//ɾ��ĳһ�����в���

};


extern symbolTable Table;

#endif