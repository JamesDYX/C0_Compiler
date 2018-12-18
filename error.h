#define E_END_FILE 1
#define E_TOO_LONG 2
#define E_IN_NOTE 3
#define E_NO_EQUAL 4
#define E_IN_CHAR 5
#define E_NO_SIN_QUOT 6
#define E_IN_STR 7
#define E_NO_DOUB_QUOT 8
#define E_IN_LEXER 9
#define E_ILL_CHAR 10

#define E_MISS_TOKEN 11
#define E_NOT_THIS 12

#define E_REDEF 13
#define E_UNDEF 14
#define E_INEF_PARA 15
#define E_SUR_PARA 16
#define E_UN_MATCH 17	//���Ͳ�ƥ�䣬����int char void֮�䲻ƥ��Ĵ���
#define E_VAL_VAR 18	//��ͼ�޸ĳ�����ֵ
#define E_RET 19 //����ֵ���Ͳ�ƥ��
#define E_PARA_LSIT 20//���βα����Ͳ�ƥ��
#define E_NO_RET 21 //ȱ��return���

#define E_OUT_BOUND	22//����Խ����

void reportError(int ERROR_NUM);
void skip(char * untilToken, int size);

