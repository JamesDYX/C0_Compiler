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


struct tableNodeId { //符号表中的标识符id，通过名称和级别唯一确定一符号
	char name[STR_MAX_LEN];
	int lev;		//所在级别
};

struct tableNode {

	struct tableNodeId id;
	int kind;		//有无返回值函数、常量、变量、数组变量
	int type;		//int char void
	union {
		int val;	//常量的值
		struct {	//用于记录形参的形参结构体
			int paramNum;	//形参数量
			struct tableNode *param[PAR_MAX_LEN];	//形参表
		};
		struct {	//用于记录数组的数组结构体
			int len;	//数组长度

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
	int delet(int lev);	//删除某一级所有参数

};


extern symbolTable Table;

#endif