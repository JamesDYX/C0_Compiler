#include "syntaxAnalys.h"
#include "error.h"
#include "getSym.h"
#include <iostream>
#include "midCode.h"
using namespace std;

int TMP_VAR_COUNT = 1;
int LABLE_COUNT = 1;
int recgProgram(int level) {
	recgValIntro(level);
	recgVarIntro(level);
	char toMain[100];
	strcpy(toMain, LABLE);
	MidCode.add(JUMP, toMain);
	while (recgFuncDef(level) != -E_NOT_THIS);
	if (recgMain(level) == -E_NOT_THIS) {
		reportError(E_MAIN);
		return -E_NOT_THIS;
	}
	else {
		MidCode.add(LAB, toMain);
		cout << "At line " << lineNum << " This is a program!" << endl;
		return 0;
	}
	return 0;
}
void seekSp(streampos sp) {
	streampos tmp_sp = sourceFile.tellg();
	sourceFile.seekg(-(off_t)sizeof(char) + sp, ios::beg);
	if (curChar == '\n')
		lineNum++;
	while (sourceFile.tellg() != tmp_sp)
	{
		if (sourceFile.get() == '\n')
			lineNum--;
	}
	sourceFile.seekg(-(off_t)sizeof(char) + sp, ios::beg);
}