#include <string>
#include "midCode.h"
#include <map>
#include <fstream>
using namespace std;
#define addrMap map < string, int>	
#define STRING (string("string")+to_string(STRING_COUNT++))
extern int STRING_COUNT;

enum mipsCode {
	nop,
	li,
	add,
	addi,
	sub,
	sll,
	mult,
	divid,
	mflo,
	sw,
	lw,
	la,
	beq,
	bne,
	bge,
	bgt,
	j,
	jal,
	jr,
	lab,
	syscall,
};

struct objectNode {
	int mipsCode;
	char rd_reg[1000];
	char rs_reg[1000];
	char rt_reg[1000];
	int rd_num;
	int rs_num;
	int rt_num;
	objectNode * next;
	objectNode * prev;
};
class objectCode
{
public:
	objectNode * head;
	objectNode * now;
	ofstream outfp;
	objectCode();
	void addNode(tetraCode midCode);
	void printMips();
private:
	int relative_address;	//相对于当前函数指针的地址
	int push_address;		//压入函数参数的地址
	bool begindef = false;
	int end_golbal_address = 0;
	objectNode * fp;
	addrMap Map;
};

