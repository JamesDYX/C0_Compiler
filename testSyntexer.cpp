#include "getSym.h"
#include <iostream>
#include "syntaxAnalys.h"
#include "midCode.h"
#include "objectCode.h"
#include "error.h"
using namespace std;
int main() {
	string fileName = "";
	cin >> fileName;
	sourceFile.open(fileName);

	curChar = sourceFile.get();
	int state = getSym();
	if (state == -E_END_FILE) {
		printf("Reach the end of the file at (%d)\n", lineNum);
	}
	recgProgram(0);
	ofstream tetraCodeFile;
	tetraCodeFile.open("midCode.txt");
	MidCode.outPut(tetraCodeFile);
	objectCode MIPS;
	MIPS.printMips();
	system("pause");
	return 0;
}