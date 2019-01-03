//
// Created by 段逸骁 on 2019-01-01.
//

#ifndef COMPILER_REGISTER_H
#define COMPILER_REGISTER_H

#include <string>
#include "midCode.h"
#include <fstream>
#include <map>
#include <vector>
using namespace std;
class Register{
public:
    string regName;
    bool hasAlloc;
    bool isDirty;
    string varName;

    Register (string Name){
        this->regName = Name;
        this->hasAlloc = false;
        this->isDirty = false;
    }
    void apply(string var, int * curRelativeAddress, struct tetraCode * curCode, vector<string> &mips);
    void release(int * curRelativeAddress, struct tetraCode * curCode, vector<string> &mips);
};
class RegisterAlloc{
public:
    int tempCount;
    int tempNum;
    vector<shared_ptr<Register>> paraReg;
    vector<shared_ptr<Register>> globReg;
    vector<shared_ptr<Register>> tempReg;
    RegisterAlloc(int Num){
        this->tempNum = Num;
        this->tempCount = 0;
        for(int i=0; i<4; i++){
            paraReg.push_back(make_shared<Register>(Register("$a"+to_string(i))));
        }
        for(int i=0; i<8; i++){
            globReg.push_back(make_shared<Register>(Register("$s"+to_string(i))));
        }
        for(int i=0; i<tempNum; i++){
            tempReg.push_back(make_shared<Register>(Register("$t"+to_string(i))));
        }
        referCount();
    }
    string findVar(string var);
    string alloc(string var, int * curRelativeAddress, struct tetraCode * current, vector<string> &mips);
    void referCount();
    void dirt(string name);
    void clearTemp(int * curRelativeAddress, struct tetraCode * curCode, vector<string> &mips); //将临时寄存器中的值存入内存
    void clearTemp(vector<string> &mips);

};
extern map<string, int > NAMap;
extern map<string, string> NRMap;  //寄存器分配记录
#endif //COMPILER_REGISTER_H
