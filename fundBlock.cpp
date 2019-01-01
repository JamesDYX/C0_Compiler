//
// Created by 段逸骁 on 2018-12-27.
//
#ifndef COMPILER_FUNDBLOCK_CPP
#define COMPILER_FUNDBLOCK_CPP
#include "fundBlock.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

void fundBlock::addPrev(shared_ptr<fundBlock> prev) {
    this->prevBlock.push_back(prev);
}

void fundBlock::addSucc(shared_ptr<fundBlock> succ) {
    this->succBlock.push_back(succ);
}

void fundBlock::setLastTetra(struct tetraCode *code) {
    this->lastTetra = code;
}

class func{
public:
    string name;
    shared_ptr<fundBlock> inBlock;
    vector<shared_ptr<fundBlock>> outBlock;
    func(string name){
        this->name = name;
        this->inBlock = nullptr;
    }
};
vector<shared_ptr<fundBlock>> allBlock;
shared_ptr<fundBlock> dividFundBlock(){
    map<string, shared_ptr<func>> funcs;
    tetraCode * curCode = &MidCode.code_head;
    //设置第一个基本块
    shared_ptr<fundBlock> beginBlock(new fundBlock(curCode));
    beginBlock->addPrev(NULL);
    beginBlock->setLastTetra(curCode);
    shared_ptr<fundBlock> curBlock = beginBlock;
    curCode = curCode->next;
    string curf = "";   //当前所处函数
    allBlock.push_back(beginBlock);
    //初步划分基本块
    while (curCode != NULL) {
        //检查每一条语句是否是：跳转、条件、函数调用等等，开始、结束或连接基本块
        if(curCode->op==LAB && curBlock->lastTetra->op!=LAB){
            //开启一个新的基本块
            shared_ptr<fundBlock> newBlock(new fundBlock(curCode));
            curBlock->addSucc(newBlock);
            newBlock->addPrev(curBlock);
            curBlock = newBlock;
            curBlock->setLastTetra(curCode);
            allBlock.push_back(newBlock);
        }
        else if(curCode->op==FUNC_DEF && curBlock->lastTetra->op!=FUNC_DEF){

            if(curf!="" && funcs[curf]->outBlock.empty()){
                funcs[curf]->outBlock.push_back(allBlock[allBlock.size()-2]);
            }

            shared_ptr<fundBlock> newBlock(new fundBlock(curCode));
            curBlock = newBlock;
            curBlock->setLastTetra(curCode);
            allBlock.push_back(newBlock);
            shared_ptr<func> ff(new func(curCode->rd));
            ff->inBlock = curBlock;
            curf = curCode->rd;
            funcs[curCode->rd] = ff;
        }
        else if(curCode->op==BEQ  ||
                curCode->op==BNE  ||
                curCode->op==BGE  ||
                curCode->op==BGT){
            //结束一个基本块
            curBlock->setLastTetra(curCode);
            curCode = curCode->next;
            if(curCode!=NULL){
                shared_ptr<fundBlock> newBlock(new fundBlock(curCode));
                curBlock->addSucc(newBlock);
                newBlock->addPrev(curBlock);
                curBlock = newBlock;
                curBlock->setLastTetra(curCode);
                allBlock.push_back(newBlock);
                if(curCode->op==FUNC_DEF){
                    if(curf!="" && funcs[curf]->outBlock.empty()){
                        funcs[curf]->outBlock.push_back(allBlock[allBlock.size()-2]);
                    }
                    shared_ptr<func> ff(new func(curCode->rd));
                    ff->inBlock = curBlock;
                    curf = curCode->rd;
                    funcs[curCode->rd] = ff;
                }
            }
            continue;
        }
        else if(curCode->op==JUMP){
            curBlock->setLastTetra(curCode);
            curCode = curCode->next;
            if(curCode!=NULL){
                shared_ptr<fundBlock> newBlock(new fundBlock(curCode));
                curBlock = newBlock;
                curBlock->setLastTetra(curCode);
                allBlock.push_back(newBlock);
                if(curCode->op==FUNC_DEF){
                    if(curf!="" && funcs[curf]->outBlock.empty()){
                        funcs[curf]->outBlock.push_back(allBlock[allBlock.size()-2]);
                    }
                    shared_ptr<func> ff(new func(curCode->rd));
                    ff->inBlock = curBlock;
                    curf = curCode->rd;
                    funcs[curCode->rd] = ff;
                }
            }
            continue;
        }
        else if(curCode->op==CALL){
            string name = curCode->rd;
            shared_ptr<func> mmm = funcs[name];
            curBlock->setLastTetra(curCode);
            curBlock->addSucc(funcs[name]->inBlock);
            funcs[name]->inBlock->addPrev(curBlock);
            curCode = curCode->next;
            if(curCode!=NULL){
                shared_ptr<fundBlock> newBlock(new fundBlock(curCode));
                curBlock = newBlock;
                curBlock->setLastTetra(curCode);
                allBlock.push_back(newBlock);
                for(int j=0; j<funcs[name]->outBlock.size(); j++){
                    funcs[name]->outBlock[j]->addSucc(curBlock);
                    curBlock->addPrev(funcs[name]->outBlock[j]);
                }
                if(curCode->op==FUNC_DEF){
                    if(curf!="" && funcs[curf]->outBlock.empty()){
                        funcs[curf]->outBlock.push_back(allBlock[allBlock.size()-2]);
                    }
                    shared_ptr<func> ff(new func(curCode->rd));
                    ff->inBlock = curBlock;
                    curf = curCode->rd;
                    funcs[curCode->rd] = ff;
                }
            }
            continue;
        }
        else if(curCode->op==RET_I || curCode->op==RET_C || curCode->op==RET_V){
            curBlock->setLastTetra(curCode);
            curCode = curCode->next;
            funcs[curf]->outBlock.push_back(curBlock);
            if(curCode!=NULL){
                shared_ptr<fundBlock> newBlock(new fundBlock(curCode));
                curBlock = newBlock;
                curBlock->setLastTetra(curCode);
                allBlock.push_back(newBlock);
                if(curCode->op==FUNC_DEF){
                    if(curf!="" && funcs[curf]->outBlock.empty()){
                        funcs[curf]->outBlock.push_back(allBlock[allBlock.size()-2]);
                    }
                    shared_ptr<func> ff(new func(curCode->rd));
                    ff->inBlock = curBlock;
                    curf = curCode->rd;
                    funcs[curCode->rd] = ff;
                }
            }
            continue;
        }
        else{
            curBlock->setLastTetra(curCode);
        }
        curCode = curCode->next;
    }
    //连接好所有基本块的后继、前驱
    int i=0;
    while(i<allBlock.size()){
        curBlock = allBlock[i];
        if(curBlock->lastTetra->op==JUMP||
           curBlock->lastTetra->op==BEQ ||
           curBlock->lastTetra->op==BNE ||
           curBlock->lastTetra->op==BGE ||
           curBlock->lastTetra->op==BGT){
            int j=0;
            while (j<allBlock.size()){
                shared_ptr<fundBlock> tmp = allBlock[j];
                if(tmp->firstTetra->op==LAB && strcmp(curBlock->lastTetra->rd, tmp->firstTetra->rd)==0){
                    curBlock->addSucc(tmp);
                    tmp->addPrev(curBlock);
                }
                j++;
            }
        }
        i++;
    }
    i=0;
    while(i<allBlock.size()){
        if(allBlock[i]->firstTetra->op==FUNC_DEF && strcmp(allBlock[i]->firstTetra->rd, ":main")==0) break;
        i++;
    }
    allBlock[allBlock.size()-1]->addSucc(allBlock[i]);
    allBlock[i]->addPrev(allBlock[allBlock.size()-1]);

    return beginBlock;
}
void pruneBlock(shared_ptr<fundBlock> root){
    if(root->isValid) return;
    root->isValid = true;
    for(shared_ptr<fundBlock> ptr:root->succBlock){
        pruneBlock(ptr);
    }
}

#endif