#include "symbolTable.h"
symbolTable Table;


int symbolTable::find(struct tableNodeId id, struct tableNode *node)
{
	table_map::iterator ree = this->table.lower_bound(id);
	if (ree == this->table.end()) {
		return -1;
	}
	struct tableNode re = ree->second;

	if (strcmp(re.id.name, id.name) == 0) {
		(*node) = re;
		return 0;
	}
	else
		return -1;
}

int symbolTable::insert(struct tableNode node, int force)	//����Ѿ������򷵻�-1�����������ű�����0
{
	tableNode re;
	int in = this->find(node.id, &re);
	if (in != 0 || re.id.lev != node.id.lev) { //�����ڶ�Ӧֵ���߶�Ӧֵ�㼶��ͬ
		this->table[node.id] = node;
		return 0;
	}
	else {
		if (force != 0) {
			this->table[node.id] = node;
			return 0;
		}
		else
			return -1;
	}
}

int symbolTable::delet(int lev)
{
	table_map::iterator i;
	for (i = this->table.begin();i != this->table.end();)
	{
		if (i->first.lev == lev)
			this->table.erase(i++);
		else
			i++;
	}
	return 0;
}
