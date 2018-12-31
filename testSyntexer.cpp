#include "getSym.h"
#include <iostream>
#include "syntaxAnalys.h"
#include "midCode.h"
#include "objectCode.h"
#include "optimize.h"
#include "error.h"
using namespace std;
int main() {
	char fileName[100];
	cin.getline(fileName, 100);
	sourceFile.open(fileName);

	curChar = sourceFile.get();
	int state = getSym();
	if (state == -E_END_FILE) {
		printf("Reach the end of the file at (%d)\n", lineNum);
	}
	recgProgram(0);
	//开始优化
	removeLabel();
	constFold();
	shared_ptr<fundBlock> firstBlock = dividFundBlock();//划分基本块

	ofstream tetraCodeFile;
	tetraCodeFile.open("midCode.txt");
	MidCode.outPut(tetraCodeFile);
	objectCode MIPS;
	MIPS.printMips();

	cout << "Press any key to continue..." << endl;
	cin.get();
	return 0;
}