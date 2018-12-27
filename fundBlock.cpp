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
    while (curCode != NULL) {
        //检查每一条语句是否是：跳转、条件、函数调用等等，开始、结束或连接基本块
        curCode = curCode->next;
    }
}