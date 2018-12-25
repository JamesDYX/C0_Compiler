#include "syntaxAnalys.h"
#include <iostream>
#include "getSym.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
using namespace std;
int recgCompound(int level) {
	recgValIntro(level);
	recgVarIntro(level);
	while (true)
	{
		if (recgStatement(level) == -E_NOT_THIS) return 0;
	}
}