#pragma once
#include <string>
class symbol
{
public:
	symbol();
	void set(int state, char * value);
	int getType();
	int getValueNum();
	char getValueCh();
	char * getValueStr();
	int len;
	
private:
	char valueStr[1001];
	int type;
	int valueNum;
	char valueCh;
};