#include "symbol.h"
#include "getSym.h"
#include <string.h>
#include <stdlib.h>



symbol::symbol() {
	for (int i = 0; i < 1001; i++) {
		this->valueStr[i] = '\0';
	}
}

void symbol:: set(int state, char * value) {
	if (state == CHARACH2) {
		this->type = CHARACH_SYM;
		this->valueCh = value[1];
		this->len = 1;
	}
	else if (state == STR2) {
		this->type = STR_SYM;
		this->len = strlen(value);
		value[strlen(value) - 1] = '\0';
		for (int i = 1, j = 0; i <= strlen(value); i++, j++) {
			if (value[i] == '\\') this->valueStr[j++] = '\\';
			this->valueStr[j] = value[i];
		}
	}
	else if (state == NUM) {
		this->type = UNSNUM_SYM;
		this->len = strlen(value);
		this->valueNum = 0;
		for (int i = 0; i < strlen(value); i++) {
			this->valueNum = this->valueNum * 10 + value[i] - '0';
		}
	}
	else if (state==SIGNINT){
		this->type = NUM_SYM;
		this->len = strlen(value);
		this->valueNum = 0;
		for (int i = 1; i < strlen(value); i++) {
			this->valueNum = this->valueNum * 10 + value[i] - '0';
		}
	}
	else {
		this->len = strlen(value);
		for (int i = 0; i < len; i++) {
			if (value[i] >= 'A' && value[i] <= 'Z') value[i] = value[i] - 'A' + 'a';
		}
		int hasFound = 0;
		int i;
		for (i = 0; i < 31; i++) {
			if (strcmp(value, token[i]) == 0) {
				hasFound = 1;
				break;
			}
		}
		for (int i = 0; i <=strlen(value); i++) {
			this->valueStr[i] = value[i];
		}
		if (hasFound) {
			this->type = i;
		}
		else this->type = ID_SYM;
	}
}
int symbol::getType() {
	return this->type;
}
int symbol::getValueNum() {
	return this->valueNum;
}
char symbol::getValueCh() {
	return this->valueCh;
}
char * symbol::getValueStr() {
	return this->valueStr;
}

symbol curSym;
