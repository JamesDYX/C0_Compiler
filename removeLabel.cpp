//
// Created by 段逸骁 on 2018-12-28.
//
#include <vector>
#include <string>
#include "midCode.h"
#include "optimize.h"
using namespace std;
void removeLabel(){
    struct tetraCode * curCode = &MidCode.code_head;
    while(curCode!=NULL){
        if(curCode->op==LAB){
            struct tetraCode * sameLabel = curCode->next;
            vector<string> labels;
            while (sameLabel!=NULL && sameLabel->op==LAB){
                labels.push_back(sameLabel->rd);
                sameLabel = sameLabel->next;
            }
            if(labels.size()!=0){
                struct tetraCode * tmp = &MidCode.code_head;
                while(tmp!=NULL){
                    if(tmp->op==JUMP||
                       tmp->op==BEQ ||
                       tmp->op==BNE ||
                       tmp->op==BGT ||
                       tmp->op==BGE){
                        bool isRedundant = false;
                        for(string s:labels){
                            if(s.compare(tmp->rd)==0){
                                isRedundant = true;
                                break;
                            }
                        }
                        if(isRedundant)
                            strcpy(tmp->rd, curCode->rd);
                    }
                    tmp = tmp->next;
                }
                curCode->next = sameLabel;
            }
        }
        curCode = curCode->next;
    }
}
