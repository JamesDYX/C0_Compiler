#include <stdio.h>
#include <fstream>
#include "symbol.h"
#include "error.h"
using namespace std;

#define START 0
#define END 1
#define ERROR 2
#define SLASH 3		//����/
#define SINGLCH0 4	//����+-��
#define SIGNINT 5	//����+1346
#define SINGLCH1 6	//����,()[];
#define SINGLCH2 7	//����<>=
#define DOUBLCH0 8	//����!
#define DOUBLCH1 9	//!=
#define CHARACH0 10	//����'
#define CHARACH1 11	//����'a
#define CHARACH2 12	//����'a'
#define STR0 13		//����"
#define STR1 14		//����"abcd
#define STR2 15		//����"abc"
#define WORD 16		//����toupper_1
#define NUM 17		//����123456
#define SINGNOTE 18
#define MULTNOTE0 19
#define MULTNOTE1 20

extern ifstream sourceFile;
extern char curChar;
extern int lineNum;
extern symbol curSym;

enum wordType {
	PLUS_SYM,
	MINIUS_SYM,
	STAR_SYM,
	SLASH_SYM,
	ASSIGN_SYM,
	LPAREN_SYM,
	RPAREN_SYM,
	LBRACKET_SYM,
	RBRACKET_SYM,
	LCURLY_SYM,
	RCURLY_SYM,
	COMMA_SYM,
	SEMICOL_SYM,
	LESS_SYM,
	GREAT_SYM,

	EQUAL_SYM,
	NOTEQ_SYM,
	LESSEQ_SYM,
	GREATEQ_SYM,

	//������
	CONST_SYM,
	INT_SYM,
	CHAR_SYM,
	VOID_SYM,
	MAIN_SYM,
	SCANF_SYM,
	PRINTF_SYM,
	RETURN_SYM,
	IF_SYM,
	ELSE_SYM,
	FOR_SYM,
	WHILE_SYM,

	//��һ��һ��
	CHARACH_SYM,
	NUM_SYM,	//�з�������
	UNSNUM_SYM,	//�޷�������
	ID_SYM,
	STR_SYM,
	
};

extern const char * symName[];

extern const char * token[];

void moveState(int * from, int to);
int getSym();
int addChar(char token[11]);
int isLetter(char c);
void backSym(int n, int Type);