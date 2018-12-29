//
// Created by 段逸骁 on 2018-12-29.
// 常量合并
//
#include "midCode.h"
#include <string>
#include <vector>

#define isAddSub 1
#define isMulDiv -1
#define unknown 0
#define known 1
using namespace std;
map < string, int> constTabel;  //常量表
struct tmpValue{                //临时变量数值区
    int state;              //当前临时变量的作用：+- */ 未知
    struct defin{        //确定值区域
        int state;
        int value;
    }defin;
    struct indefin{     //不确定值区域
        vector<string> names;    //不确定的变量名
        vector<char> opers;      //变量名所对应的符号
    }indefin;
};
map <string, struct tmpValue> tmpTabel;
bool isTemp(string str){
    return str.find("&")!=string::npos;
}
void constFold(){
    struct tetraCode * newHead = (struct tetraCode *)malloc(sizeof(struct tetraCode));
    newHead->next = NULL;
    struct tetraCode * curCode = MidCode.code_head.next;
    struct tetraCode * curNew = newHead;
    while(curCode!=NULL){
        if(curCode->op==CONST_C || curCode->op==CONST_I){
            string name = curCode->rd;
            if(isTemp(name)){
                //如果是临时变量，则加入到临时变量表中
                struct tmpValue tmp;
                tmp.state = unknown;
                tmp.defin.state = known;
                tmp.defin.value = curCode->rs_num;
                tmpTabel[name] = tmp;
            }
            else{
                //如果是常量，则加入到常量表中
                constTabel[name] = curCode->rs_num;
            }
        }
        else if(curCode->op==ADD){
            if(isTemp(curCode->rd)){
                if(strcmp(curCode->rt, ":&zero")){
                    //直接赋值形式
                    struct tmpValue tmp;
                    tmp.state = unknown;
                    if(constTabel.count(curCode->rs)==0){
                        tmp.defin.state = unknown;
                        tmp.indefin.names.push_back(curCode->rs);
                        tmp.indefin.opers.push_back('+');
                    }
                    else{
                        tmp.defin.state = known;
                        tmp.defin.value = constTabel[curCode->rs];
                    }
                    tmpTabel[curCode->rd] = tmp;
                }
                else{
                    //四则运算模式
                    struct tmpValue * tmp = &tmpTabel[curCode->rd];
                    tmp->state = isAddSub;
                    if(isTemp(curCode->rt)&&tmpTabel[curCode->rt].indefin.names.size()==0){
                        if(tmp->defin.state==unknown){
                            tmp->defin.state = known;
                            tmp->defin.value = 0;
                        }
                        tmp->defin.value+=tmpTabel[curCode->rt].defin.value;
                    }
                    else if(constTabel.count(curCode->rt)!=0){
                        if(tmp->defin.state==unknown){
                            tmp->defin.state = known;
                            tmp->defin.value = 0;
                        }
                        tmp->defin.value+=constTabel[curCode->rt];
                    }
                    else{
                        tmp->indefin.names.push_back(curCode->rt);
                        tmp->indefin.opers.push_back('+');
                    }
                }
            }
            else{
                //赋值模式
            }
        }
        else if(curCode->op==SUB){
            if(isTemp(curCode->rd)){
                //四则运算模式
            }
            else{
                //赋值模式
            }
        }
        curCode = curCode->next;
    }
}