//
// Created by 段逸骁 on 2018-12-27.
//

#ifndef COMPILER_FUNDBLOCK_H
#define COMPILER_FUNDBLOCK_H

#include <vector>
#include <memory>
#include "midCode.h"
using namespace std;

class fundBlock {
public:
    vector<shared_ptr<fundBlock>> prevBlock;//前驱基本块
    vector<shared_ptr<fundBlock>> succBlock;//后继基本块
    struct tetraCode* firstTetra;//该基本块初始的四元式
    struct tetraCode* lastTetra;//该基本块末尾的四元式
    bool isValid = false;

    fundBlock(struct tetraCode * firstTetra){
        this->firstTetra = firstTetra;
    }

    void addPrev(shared_ptr<fundBlock> prev);
    void addSucc(shared_ptr<fundBlock> succ);
    void setLastTetra(struct tetraCode * code);
};

shared_ptr<fundBlock> dividFundBlock();
void pruneBlock(shared_ptr<fundBlock> root);

#endif //COMPILER_FUNDBLOCK_H
