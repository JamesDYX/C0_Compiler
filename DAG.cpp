//
// Created by 段逸骁 on 2018-12-31.
//
#include "optimize.h"
#include "fundBlock.h"
#include <string>
using namespace std;
bool IsTemp(const string &str){
    return str.find('&')!=string::npos;
}
char * check(struct tetraCode * end, char * var, int curBlockNum, int lastBlockNum){
    struct tetraCode * curCode = allBlock[curBlockNum]->firstTetra;
    struct tetraCode * change = nullptr;
    while(curCode!=end){
        if(curCode->op==ADD && strcmp(curCode->rd, var)==0 && strcmp(":&zero", curCode->rt)==0) change=curCode;
        else if(strcmp(curCode->rd, var)==0) change = nullptr;
        else if(change != nullptr && strcmp(curCode->rd, change->rs)==0) change = nullptr;
        curCode = curCode->next;
    }
    if(change== nullptr) return nullptr;
    if(IsTemp(change->rs)){
        char * tmp = check(change, change->rs, curBlockNum, lastBlockNum);
        if(change==allBlock[curBlockNum]->firstTetra){
            allBlock[curBlockNum]->firstTetra = change->next;
        }
        struct tetraCode * prevCode = lastBlockNum==-1?allBlock[0]->firstTetra:allBlock[lastBlockNum]->lastTetra;
        while(prevCode->next!=change) prevCode = prevCode->next;
        prevCode->next = change->next;
        if(tmp== nullptr){
            return change->rs;
        }
        else{
            return tmp;
        }
    }
    else{
        if(change==allBlock[curBlockNum]->firstTetra){
            allBlock[curBlockNum]->firstTetra = change->next;
        }
        struct tetraCode * prevCode = lastBlockNum==-1?allBlock[0]->firstTetra:allBlock[lastBlockNum]->lastTetra;
        while(prevCode->next!=change) prevCode = prevCode->next;
        prevCode->next = change->next;
        return change->rs;
    }
}
void removeSame(struct tetraCode * end, int curBlockNum, int lastBlockNum){
    struct tetraCode * curCode = allBlock[curBlockNum]->firstTetra;
    while (true){
        if(curCode->op==ADD || curCode->op==SUB || curCode->op==MULT || curCode->op==DIV || curCode->op==BEQ || curCode->op==BNE || curCode->op==BGE || curCode->op==BGT){
            if(IsTemp(curCode->rs)) {
                //查看rs
                char * rs = check(curCode, curCode->rs, curBlockNum, lastBlockNum);
                if(rs!= nullptr) strcpy(curCode->rs, rs);
            }
            if(IsTemp(curCode->rt)){
                //查看rt
                char * rt = check(curCode, curCode->rt, curBlockNum, lastBlockNum);
                if(rt!= nullptr) strcpy(curCode->rt, rt);
            }
        }
//        else if(curCode->op==SUB && !IsTemp(curCode->rd)){
//            if(IsTemp(curCode->rs)) {
//                //查看rs
//                char * rs = check(curCode, curCode->rs, curBlockNum, lastBlockNum);
//                if(rs!= nullptr) strcpy(curCode->rs, rs);
//            }
//            if(IsTemp(curCode->rt)){
//                //查看rt
//                char * rt = check(curCode, curCode->rt, curBlockNum, lastBlockNum);
//                if(rt!= nullptr) strcpy(curCode->rt, rt);
//            }
//        }
        else if(curCode->op==LOAD_I || curCode->op==LOAD_C){
            //rt
            char * rt = check(curCode, curCode->rt, curBlockNum, lastBlockNum);
            if(rt!= nullptr) strcpy(curCode->rt, rt);
        }
        else if(curCode->op==SAVE_I || curCode->op==SAVE_C){
            //rd rt
            char * rd = check(curCode, curCode->rd, curBlockNum, lastBlockNum);
            if(rd!= nullptr) strcpy(curCode->rd, rd);
            char * rt = check(curCode, curCode->rt, curBlockNum, lastBlockNum);
            if(rt!= nullptr) strcpy(curCode->rt, rt);
        }
        else if(curCode->op==PRINT_C || curCode->op==PRINT_I ||
                curCode->op==RET_C || curCode->op==RET_I ||
                curCode->op==PUSH_C || curCode->op==PUSH_I){
            //rd
            char * rd = check(curCode, curCode->rd, curBlockNum, lastBlockNum);
            if(rd!= nullptr) strcpy(curCode->rd, rd);
        }
//        else if(curCode->op==BEQ || curCode->op==BNE || curCode->op==BGE || curCode->op==BGT) {
//            //rs rt
//            char * rs = check(curCode, curCode->rs, curBlockNum, lastBlockNum);
//            if(rs!= nullptr) strcpy(curCode->rs, rs);
//            char * rt = check(curCode, curCode->rt, curBlockNum, lastBlockNum);
//            if(rt!= nullptr) strcpy(curCode->rt, rt);
//        }
        if(curCode==end) break;
        curCode = curCode->next;
    }
}
void DAG(){
    int j= -1;
    for(int i=0; i<allBlock.size(); i++){
//        removeSame(allBlock[i]->lastTetra, i, i-1);
        if(!allBlock[i]->isValid){
            while(!allBlock[i]->isValid) i++;
            allBlock[j]->lastTetra->next = allBlock[i]->firstTetra;
        }
        removeSame(allBlock[i]->lastTetra, i, j);
        j = i;
    }
}

