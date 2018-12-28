//
// Created by 段逸骁 on 2018-12-27.
//

#include "fundBlock.h"

void fundBlock::addPrev(fundBlock prev) {
    this->prevBlock.push_back(prev);
}

void fundBlock::addSucc(fundBlock succ) {
    this->succBlock.push_back(succ);
}

void fundBlock::setLastTetra(struct tetraCode *code) {
    this->lastTetra = code;
}

int dividFundBlock(){
    tetraCode * curCode = &MidCode.code_head;
    //设置第一个基本块
    fundBlock beginBlock(curCode);
    beginBlock.addPrev(NULL);
    beginBlock.setLastTetra(curCode);
    fundBlock curBlock = beginBlock;
    curCode = curCode->next;
    //初步划分基本块
    while (curCode != NULL) {
        //检查每一条语句是否是：跳转、条件、函数调用等等，开始、结束或连接基本块
        if(curCode->op==LAB || curCode->op==FUNC_DEF){
            //开启一个新的基本块
            fundBlock newBlock(curCode);
            curBlock.addSucc(newBlock);
            newBlock.addPrev(curBlock);
            curBlock = newBlock;
            curBlock.setLastTetra(curCode);
        }
        else if(curCode->op==JUMP ||
                curCode->op==BEQ  ||
                curCode->op==BNE  ||
                curCode->op==BGE  ||
                curCode->op==BGT  ||
                curCode->op==CALL){
            //结束一个基本块
            curBlock.setLastTetra(curCode);
            curCode = curCode->next;
            if(curCode==NULL){
                continue;
            }
            else{
                fundBlock newBlock(curCode);
                curBlock.addSucc(newBlock);
                newBlock.addPrev(curBlock);
                curBlock = newBlock;
                curBlock.setLastTetra(curCode);
            }
        }
        else{
            curBlock.setLastTetra(curCode);
        }
        curCode = curCode->next;
    }
    curBlock.isLast = true;
    //连接好所有基本块的后继、前驱
    curBlock = beginBlock;
    while(true){
        if(curBlock.lastTetra->op==JUMP||
           curBlock.lastTetra->op==BEQ ||
           curBlock.lastTetra->op==BNE ||
           curBlock.lastTetra->op==BGE ||
           curBlock.lastTetra->op==BGT){
            fundBlock tmp = beginBlock;
            while (true){
                if(tmp.lastTetra->op==LAB && strcmp(curBlock.lastTetra->rd, tmp.lastTetra->rd)){
                    curBlock.addSucc(tmp);
                    tmp.addPrev(curBlock);
                }
                if(tmp.isLast) break;
                tmp = tmp.succBlock[0];
            }
        }
        else if(curBlock.lastTetra->op==CALL){
            fundBlock tmp = beginBlock;
            while (true){
                if(tmp.lastTetra->op==FUNC_DEF && strcmp(curBlock.lastTetra->rd, tmp.lastTetra->rd)){
                    curBlock.addSucc(tmp);
                    tmp.addPrev(curBlock);
                }
                if(tmp.isLast) break;
                tmp = tmp.succBlock[0];
            }
        }
        if(curBlock.isLast) break;
        curBlock = curBlock.succBlock[0];
    }
}