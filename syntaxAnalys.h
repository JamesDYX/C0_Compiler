#include "error.h"
#include "symbolTable.h"
#include <fstream>
#define MISS_TOKEN  {									\
					reportError(E_MISS_TOKEN);			\
					char until[3] = { ';','\n','}' };		\
					skip(until, 3);						\
					curState = E_MISS_TOKEN;			\
					}

#ifndef SYNTAXANALYS_H
#define SYNTAXANALYS_H
#define TMP_VAR (string("&tmp")+to_string(TMP_VAR_COUNT++)).c_str()
#define LABLE (string("$lable")+to_string(LABLE_COUNT++)).c_str()
#define EXCEPTVALUE 10101010
extern int TMP_VAR_COUNT;
extern int LABLE_COUNT;
extern bool hasRetrun;

using namespace std;

void seekSp(streampos sp);

int recgProgram(int level);
int recgValIntro(int level);
int recgValDef(int level);
int recgVarIntro(int level);
int recgVarDef(int level);
int recgFuncDef(int level);
int recgCompound(int level);
int recgParaList(int level, struct tableNode * funcnode);
int recgMain(int level);
int recgExpression(int level, struct tableNode * tmpnode);
int recgTerm(int level, struct tableNode * tmpnode);
int recgFactor(int level, struct tableNode * tmpnode);
int recgStatement(int level);
int recgAssign(int level);
int recgIf(int level);
int recgCondition(int level,char *  Label, bool satisfy);	//生成一个跳转语句, satisfy为真则满足条件时跳转，否则为不满足时跳转
int recgRepeat(int level);
int recgIFuncCall(int level, struct tableNode * tmpnode);
int recgNFuncCall(int level);
int recgValueParaList(int level, struct tetraCode & head, int *len, struct tableNode func);
int recgRead(int level);
int recgWrite(int level);
int recgReturn(int level);
#endif