//
// Created by 段逸骁 on 2018-12-27.
//

#ifndef COMPILER_FUNDBLOCK_H
#define COMPILER_FUNDBLOCK_H

#include <vector>
#include "midCode.h"
using namespace std;

class fundBlock {
public:
    vector<fundBlock> prevBlock;//前驱基本块
    vector<fundBlock> succBlock;//后继基本块
    struct tetraCode* firstTetra;//该基本块初始的四元式
    struct tetraCode* lastTetra;//该基本块末尾的四元式
    bool isLast = false;

    fundBlock(struct tetraCode * firstTetra){
        this->firstTetra = firstTetra;
    }

    void addPrev(fundBlock prev);
    void addSucc(fundBlock succ);
    void setLastTetra(struct tetraCode * code);
};

int dividFundBlock();

#endif //COMPILER_FUNDBLOCK_H
