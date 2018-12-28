#include "error.h"
#include "getSym.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

void reportError(int ERROR_NUM) {
	if (ERROR_NUM == E_TOO_LONG)
		printf("Word is too long at (%d)\n", lineNum);
	else if (ERROR_NUM == E_IN_NOTE)
		printf("Reach the end of the file in note at (%d)\n", lineNum);
	else if (ERROR_NUM == E_NO_EQUAL)
		printf("We need a '=' at (%d)\n", lineNum);
	else if (ERROR_NUM == E_ILL_CHAR || ERROR_NUM == E_IN_CHAR || ERROR_NUM == E_IN_STR)
		printf("Illegal character at (%d)\n", lineNum);
	else if (ERROR_NUM == E_NO_SIN_QUOT)
		printf("We need a ''' at (%d)\n", lineNum);
	else if (ERROR_NUM == E_NO_DOUB_QUOT)
		printf("We need a '\"' at (%d)\n", lineNum);
	else if (ERROR_NUM == E_MISS_TOKEN)
		printf("We loose a token at (%d)\n", lineNum);
	else if (ERROR_NUM == E_REDEF)
		printf("Redeclaration at (%d)\n", lineNum);
	else if (ERROR_NUM == E_UNDEF)
		printf("Find undefined identifier at (%d)\n", lineNum);
	else if(ERROR_NUM == E_INEF_PARA)
		printf("Insufficient value in parameter list at (%d)\n", lineNum);
	else if(ERROR_NUM == E_SUR_PARA)
		printf("Too much values in parameter list at (%d)\n", lineNum);
	else if(ERROR_NUM == E_UN_MATCH)
		printf("Mismatching type at (%d)\n", lineNum);
	else if(ERROR_NUM == E_VAL_VAR)
		printf("Failed to modify a constant at (%d)\n", lineNum);
	else if (ERROR_NUM == E_RET)
		printf("Mismatching type in return at (%d)\n", lineNum);
	else if(ERROR_NUM == E_PARA_LSIT)
		printf("Mismatching type in para list at (%d)\n", lineNum);
	else if(ERROR_NUM==E_NO_RET)
		printf("No return until last of a function at (%d)\n", lineNum);
	else if(ERROR_NUM== E_OUT_BOUND)
		printf("Index out of bound at (%d)\n", lineNum);
	else if(ERROR_NUM==E_MAIN)
		printf("Wrong main at (%d)\n", lineNum);
	else
		printf("Unknown error at (%d)\n", lineNum);
	printf("Exception Found! Press any key to continue\n");
	cin.get();
}

int isUntilToken(char c, char * untilToken, int size) {
	for (int i = 0; i < size; i++) {
		if (c == untilToken[i])
			return 1;
	}
	return 0;
}

void skip(char * untilToken, int size) {
	while (!isUntilToken(curChar, untilToken, size)) {
		curChar = sourceFile.get();
	}
}

