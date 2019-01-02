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
    string varName;

    Register (string Name){
        this->regName = Name;
        this->hasAlloc = false;
    }
    void apply(string var, int * curRelativeAddress, struct tetraCode * curCode, ofstream &mips);
    void release(int * curRelativeAddress, struct tetraCode * curCode, ofstream &mips);
};
class RegisterAlloc{
private:
    int tempCount;
    int tempNum;
public:
    vector<shared_ptr<Register>> paraReg;
    vector<shared_ptr<Register>> globReg;
    vector<shared_ptr<Register>> tempReg;
    RegisterAlloc(int tmpNum){
        this->tempNum = tempNum;
        this->tempCount = 0;
        for(int i=0; i<4; i++){
            paraReg.push_back(make_shared<Register>(Register("$a"+i)));
        }
        for(int i=0; i<8; i++){
            globReg.push_back(make_shared<Register>(Register("$s"+i)));
        }
        for(int i=0; i<tmpNum; i++){
            tempReg.push_back(make_shared<Register>(Register("$t"+i)));
        }
    }
    string alloc(string var, int * curRelativeAddress, struct tetraCode * current, ofstream &mips);
    void referCount();

};
extern map<string, int > NAMap;
#endif //COMPILER_REGISTER_H
