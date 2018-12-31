//
// Created by 段逸骁 on 2018-12-29.
// 常量合并
//
#include "midCode.h"
#include "syntaxAnalys.h"
#include <stdlib.h>
#include <string>
#include <vector>

#define unknown 0
#define known 1
using namespace std;
map < string, int> constTabel;  //常量表
class indefi{
public:
    vector<string> names;    //不确定的变量名
    vector<int> opers;      //变量名所对应的符号
//    indefi(){
//        names.reserve(100);
//        opers.reserve(100);
//    }
};
struct tmpValue{                //临时变量数值区
    int state=unknown;              //当前临时变量的作用：+- */ 未知
    struct defi{        //确定值区域
        int state=unknown;      //是否已经有值
        int value=0;
    }defin;
    indefi * indefin = new indefi();
};
map <string, struct tmpValue *> tmpTabel;
bool isTemp(const string &str){
    return str.find('&')!=string::npos;
}
void arithmetic(int operKind, struct tetraCode * curCode){
    struct tmpValue * tmp = tmpTabel[curCode->rd];
    tmp->state = operKind;

    if(operKind==ADD){
        if(strcmp(curCode->rs, curCode->rd)!=0){
            if(strcmp(curCode->rs, ":&zero")==0 && curCode->op==SUB && strcmp(curCode->rt, curCode->rd)==0){
                tmp->defin.value = -(tmp->defin.value);
                for(int i=0; i<tmp->indefin->opers.size(); i++){
                    tmp->indefin->opers[i] = tmp->indefin->opers[i]==ADD?SUB:ADD;
                }
            }
            else if(isTemp(curCode->rs)&&tmpTabel.count(curCode->rs)!=0&& tmpTabel[curCode->rs]->indefin->names.empty()){
                if(tmp->defin.state==unknown){
                    tmp->defin.state = known;
                    tmp->defin.value = 0;
                }
                tmp->defin.value=tmpTabel[curCode->rs]->defin.value;
                tmp->indefin->opers.clear();
                tmp->indefin->names.clear();
            }
            else if(constTabel.count(curCode->rs)!=0){
                if(tmp->defin.state==unknown){
                    tmp->defin.state = known;
                    tmp->defin.value = 0;
                }
                tmp->defin.value=constTabel[curCode->rs];
                tmp->indefin->opers.clear();
                tmp->indefin->names.clear();
            }
            else{
                tmp->indefin->opers.clear();
                tmp->indefin->names.clear();
                if(tmpTabel[curCode->rs]->state == ADD){
                    if(tmpTabel[curCode->rs]->defin.state == known){
                        tmp->defin.value = tmpTabel[curCode->rs]->defin.value;
                        tmpTabel[curCode->rs]->defin.state = unknown;
                    }
                    if(!tmpTabel[curCode->rs]->indefin->opers.empty()){
                        tmp->indefin->names.emplace_back(curCode->rs);
                        tmp->indefin->opers.push_back(ADD);
                    }
                }
                else{
                    tmp->indefin->names.emplace_back(curCode->rs);
                    tmp->indefin->opers.push_back(ADD);
                }
            }
        }
        if(tmpTabel.count(curCode->rt)!=0&&isTemp(curCode->rt)&& tmpTabel[curCode->rt]->indefin->names.empty()){
            if(tmp->defin.state==unknown){
                tmp->defin.state = known;
                tmp->defin.value = 0;
            }
            switch (curCode->op){
                case ADD:tmp->defin.value+=tmpTabel[curCode->rt]->defin.value;break;
                case SUB:tmp->defin.value-=tmpTabel[curCode->rt]->defin.value;break;
            }
        }
        else if(constTabel.count(curCode->rt)!=0){
            if(tmp->defin.state==unknown){
                tmp->defin.state = known;
                tmp->defin.value = 0;
            }
            switch (curCode->op){
                case ADD:tmp->defin.value+=constTabel[curCode->rt];break;
                case SUB:tmp->defin.value-=constTabel[curCode->rt];break;
            }
        }
        else{
            if(tmpTabel[curCode->rt]->state == ADD){
                if(tmpTabel[curCode->rt]->defin.state == known){
                    tmp->defin.value += tmpTabel[curCode->rt]->defin.value;
                    tmpTabel[curCode->rt]->defin.state = unknown;
                }
                if(!tmpTabel[curCode->rt]->indefin->opers.empty()){
                    tmp->indefin->names.emplace_back(curCode->rt);
                    tmp->indefin->opers.push_back(curCode->op);
                }
            }
            else {
                tmp->indefin->names.emplace_back(curCode->rt);
                tmp->indefin->opers.push_back(curCode->op);
            }
        }
    }
    else{
        if((curCode->op==MULT && find(tmp->indefin->opers.begin(), tmp->indefin->opers.end(), DIV) != tmp->indefin->opers.begin()) ||
           (curCode->op==DIV && !tmp->indefin->opers.empty())){
            tmp->indefin->names.emplace_back(curCode->rt);
            tmp->indefin->opers.push_back(curCode->op);
        }
        else if(tmpTabel.count(curCode->rt)!=0&&isTemp(curCode->rt)&& tmpTabel[curCode->rt]->indefin->names.empty()){
            if(tmp->defin.state==unknown){
                tmp->defin.state = known;
                tmp->defin.value = 1;
            }
            switch (curCode->op){
                case MULT:tmp->defin.value*=tmpTabel[curCode->rt]->defin.value;break;
                case DIV:tmp->defin.value/=tmpTabel[curCode->rt]->defin.value;break;
            }
        }
        else if(constTabel.count(curCode->rt)!=0){
            if(tmp->defin.state==unknown){
                tmp->defin.state = known;
                tmp->defin.value = 1;
            }
            switch (curCode->op){
                case MULT:tmp->defin.value*=constTabel[curCode->rt];break;
                case DIV:tmp->defin.value/=constTabel[curCode->rt];break;
            }
        }
        else{
            tmp->indefin->names.emplace_back(curCode->rt);
            tmp->indefin->opers.push_back(curCode->op);
        }
    }
}
void decode(char * tmpName, tetraCode * * curNew, char * replaceName = (char *) nullptr){
    struct tmpValue * tmp = tmpTabel[tmpName];
    int size = static_cast<int>(tmp->indefin->names.size());
    int i=0;
    if(size==0){
        //直接加载立即数
        auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
        tmpCode->op = LI;
        strcpy(tmpCode->rd, replaceName== nullptr ?tmpName:replaceName);
        tmpCode->rs_num = tmp->defin.value;
        tmpCode->next = nullptr;
        (*curNew)->next = tmpCode;
        *curNew = tmpCode;
    }
    else if(tmp->defin.state==known){
        if(tmp->indefin->names[0]==string("UNCERTAIN")){
            i++;
            if(replaceName== nullptr){
                char tmpVar[100];
                strcpy(tmpVar, TMP_VAR);
                auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                tmpCode->op = LI;
                strcpy(tmpCode->rd, tmpVar);
                tmpCode->rs_num = tmp->defin.value;
                tmpCode->next = nullptr;
                (*curNew)->next = tmpCode;
                *curNew = tmpCode;

                tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                tmpCode->op = (tmp->state==ADD)?(tmp->indefin->opers[0]==ADD)?ADD:SUB:MULT;
                strcpy(tmpCode->rd, tmpName);
                strcpy(tmpCode->rs, tmpVar);
                strcpy(tmpCode->rt, tmpName);
                tmpCode->next = nullptr;
                (*curNew)->next = tmpCode;
                *curNew = tmpCode;
            }
            else{
                auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                tmpCode->op = LI;
                strcpy(tmpCode->rd, replaceName);
                tmpCode->rs_num = tmp->defin.value;
                tmpCode->next = nullptr;
                (*curNew)->next = tmpCode;
                *curNew = tmpCode;

                tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                tmpCode->op = (tmp->state==ADD)?(tmp->indefin->opers[0]==ADD)?ADD:SUB:MULT;
                strcpy(tmpCode->rd, replaceName);
                strcpy(tmpCode->rs, replaceName);
                strcpy(tmpCode->rt, tmpName);
                tmpCode->next = nullptr;
                (*curNew)->next = tmpCode;
                *curNew = tmpCode;
            }
        }
        else{
            auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
            tmpCode->op = LI;
            strcpy(tmpCode->rd, replaceName== nullptr?tmpName:replaceName);
            tmpCode->rs_num = tmp->defin.value;
            tmpCode->next = nullptr;
            (*curNew)->next = tmpCode;
            *curNew = tmpCode;
        }
    }
    else{
        if(tmp->indefin->names[0]==string("UNCERTAIN")){
            if(size==1){
                i++;
                if(replaceName!= nullptr){
                    auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                    tmpCode->op = ADD;
                    strcpy(tmpCode->rd, replaceName);
                    strcpy(tmpCode->rs, tmpName);
                    strcpy(tmpCode->rt, ":&zero");
                    tmpCode->next = nullptr;
                    (*curNew)->next = tmpCode;
                    *curNew = tmpCode;
                }
            }
            else{
                i+=2;
                char num2[100];
                strcpy(num2, (tmp->indefin->names[1]).c_str());
                decode(num2, curNew);
                auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                tmpCode->op = (tmp->state==ADD)?(tmp->indefin->opers[1]==ADD)?ADD:SUB:MULT;
                strcpy(tmpCode->rd, replaceName== nullptr?tmpName:replaceName);
                strcpy(tmpCode->rs, tmpName);
                strcpy(tmpCode->rt, num2);
                tmpCode->next = nullptr;
                (*curNew)->next = tmpCode;
                *curNew = tmpCode;
            }
        }
        else{
            if(size==1){
                i++;
                char num1[100];
                strcpy(num1, (tmp->indefin->names[0]).c_str());
                decode(num1, curNew, replaceName== nullptr?tmpName:replaceName);
            }
            else{
                i+=2;
                char num1[100];
                strcpy(num1, (tmp->indefin->names[0]).c_str());
                decode(num1, curNew);
                char num2[100];
                strcpy(num2, (tmp->indefin->names[1]).c_str());
                decode(num2, curNew);
                auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
                tmpCode->op = (tmp->state==ADD)?(tmp->indefin->opers[1]==ADD)?ADD:SUB:MULT;
                strcpy(tmpCode->rd, replaceName== nullptr?tmpName:replaceName);
                strcpy(tmpCode->rs, num1);
                strcpy(tmpCode->rt, num2);
                tmpCode->next = nullptr;
                (*curNew)->next = tmpCode;
                *curNew = tmpCode;
            }
        }
    }
    while(i<size){
        char num[10];
        strcpy(num, (tmp->indefin->names[i]).c_str());
        decode(num, curNew);
        auto * tmpCode = (struct tetraCode *)malloc(sizeof(struct tetraCode));
        tmpCode->op = (tmp->state==ADD)?(tmp->indefin->opers[i]==ADD)?ADD:SUB:MULT;
        strcpy(tmpCode->rd, replaceName== nullptr?tmpName:replaceName);
        strcpy(tmpCode->rs, replaceName== nullptr?tmpName:replaceName);
        strcpy(tmpCode->rt, num);
        tmpCode->next = nullptr;
        (*curNew)->next = tmpCode;
        *curNew = tmpCode;
        i++;
    }
    free(tmp);
}
void constFold(){
    auto * newHead = (struct tetraCode *)malloc(sizeof(struct tetraCode));
    newHead->next = nullptr;
    struct tetraCode * curCode = MidCode.code_head.next;
    struct tetraCode * curNew = newHead;
    while(curCode!= nullptr){
        if(curCode->op==CONST_C || curCode->op==CONST_I){
            string name = curCode->rd;
            if(isTemp(name)){
                //如果是临时变量，则加入到临时变量表中
                auto * tmp = new struct tmpValue();
                tmp->state = unknown;
                tmp->defin.state = known;
                tmp->defin.value = curCode->rs_num;
                tmpTabel[name] = tmp;
            }
            else{
                //如果是常量，则加入到常量表中
                constTabel[name] = curCode->rs_num;
            }
        }
        else if(curCode->op==ADD){
            if(isTemp(curCode->rd)){
                if(strcmp(curCode->rt, ":&zero")==0){
                    //直接赋值形式
                    auto * tmp = new struct tmpValue();
                    tmp->state = unknown;
                    if(isTemp(curCode->rs)){
                        if(tmpTabel[curCode->rs]->indefin->opers.empty()){
                            tmp->defin.state = known;
                            tmp->defin.value = tmpTabel[curCode->rs]->defin.value;
                        }
                        else {
                            tmp->defin.state = unknown;
                            tmp->indefin->names.push_back(curCode->rs);
                            tmp->indefin->opers.push_back(ADD);
                        }
                    }
                    else if(constTabel.count(curCode->rs)==0){
                        tmp->defin.state = unknown;
                        tmp->indefin->names.push_back("UNCERTAIN");
                        tmp->indefin->opers.push_back(ADD);
                        curNew->next = curCode;
                        curNew = curCode;
                    }
                    else{
                        tmp->defin.state = known;
                        tmp->defin.value = constTabel[curCode->rs];
                    }
                    tmpTabel[curCode->rd] = tmp;
                }
                else{
                    //四则运算模式
                    arithmetic(ADD, curCode);
                }
            }
            else{
                //赋值模式
                if(strcmp(curCode->rt, ":&zero")==0)
                    decode(curCode->rs, &curNew, curCode->rd);
                else{
                    if(isTemp(curCode->rs))
                        decode(curCode->rs, &curNew);
                    if(isTemp(curCode->rt))
                        decode(curCode->rt, &curNew);
                    curNew->next = curCode;
                    curNew = curCode;
                }
            }
        }
        else if(curCode->op==SUB){
            if(isTemp(curCode->rd)){
                //四则运算模式
                arithmetic(ADD, curCode);
            }
            else{
                //赋值模式
                if(isTemp(curCode->rs))
                    decode(curCode->rs, &curNew);
                if(isTemp(curCode->rt))
                    decode(curCode->rt, &curNew);
                curNew->next = curCode;
                curNew = curCode;
            }
        }
        else if(curCode->op==MULT || curCode->op==DIV){
            arithmetic(MULT, curCode);
        }
        else if(curCode->op==GET_RET_C || curCode->op==GET_RET_I){
            auto * tmp = new struct tmpValue();
            tmp->state = unknown;
            tmp->defin.state = unknown;
            tmp->indefin->names.emplace_back("UNCERTAIN");
            tmp->indefin->opers.push_back(ADD);
            tmpTabel[curCode->rd] = tmp;
            curNew->next = curCode;
            curNew = curCode;
        }
        else if(curCode->op==LOAD_C || curCode->op==LOAD_I){
            decode(curCode->rt, &curNew);

            auto * tmp = new struct tmpValue();
            tmp->state = unknown;
            tmp->defin.state = unknown;
            tmp->indefin->names.emplace_back("UNCERTAIN");
            tmp->indefin->opers.push_back(ADD);
            tmpTabel[curCode->rd] = tmp;
            curNew->next = curCode;
            curNew = curCode;
        }
        else if(curCode->op==SAVE_C || curCode->op==SAVE_I){
            decode(curCode->rd, &curNew);
            decode(curCode->rt, &curNew);
            curNew->next = curCode;
            curNew = curCode;

        }
        else if(curCode->op==PUSH_C || curCode->op==PUSH_I){
            struct tetraCode * pushBegin = curCode;
            while(curCode->next->op==PUSH_I || curCode->next->op==PUSH_C){
                decode(curCode->rd, &curNew);
                curCode = curCode->next;
            }
            decode(curCode->rd, &curNew);
            curNew->next = pushBegin;
            curNew = curCode;
        }
        else if(curCode->op==PRINT_C || curCode->op==PRINT_I ||
                curCode->op==RET_C || curCode->op==RET_I){
            decode(curCode->rd, &curNew);
            curNew->next = curCode;
            curNew = curCode;
        }
        else if(curCode->op==BEQ || curCode->op==BNE || curCode->op==BGE || curCode->op==BGT){
            decode(curCode->rs, &curNew);
            decode(curCode->rt, &curNew);
            curNew->next = curCode;
            curNew = curCode;
        }
        else{
            curNew->next = curCode;
            curNew = curCode;
        }
        curCode = curCode->next;
    }
    curNew->next = nullptr;
    MidCode.code_head.next = newHead->next;
}