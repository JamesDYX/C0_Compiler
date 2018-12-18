#include "symbol.h"
#include "getSym.h"
#include <stdlib.h>

int main(){
	sourceFile.open("16061183_test.txt");
	
	curChar = sourceFile.get();
	int i = 1;
	while (true) {
		int state = getSym();
		if (state == -E_END_FILE) {
			printf("Reach the end of the file at (%d)\n", lineNum);
			break;
		}
		else if (state == 0) {
			printf("%d\t%s\t", i, symName[curSym.getType()]);
			if (curSym.getType() == CHARACH_SYM) 
				printf("%c\n", curSym.getValueCh());
			else if (curSym.getType() == NUM_SYM || curSym.getType() == UNSNUM_SYM) 
				printf("%d\n", curSym.getValueNum());
			else printf("%s\n", curSym.getValueStr());
			i++;
		}
		else
		{
			if (state == -E_TOO_LONG) {
				printf("Word is too long at (%d)\n", lineNum);
			}
			else if (state == -E_IN_NOTE)
				printf("Reach the end of the file in note at (%d)\n", lineNum);
			else if (state == -E_NO_EQUAL)
				printf("We need a '=' at (%d)\n", lineNum);
			else if(state==-E_ILL_CHAR || state==-E_IN_CHAR || state==-E_IN_STR)
				printf("Illegal character at (%d)\n", lineNum);
			else if (state==-E_NO_SIN_QUOT)
				printf("We need a ''' at (%d)\n", lineNum);
			else if (state == -E_NO_DOUB_QUOT)
				printf("We need a '\"' at (%d)\n", lineNum);
			else
				printf("Unknown error at (%d)\n", lineNum);

		}
	}
	system("pause");
	return 0;
}