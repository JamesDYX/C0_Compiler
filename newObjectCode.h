//
// Created by 段逸骁 on 2019-01-02.
//

#ifndef COMPILER_NEWOBJECTCODE_H
#define COMPILER_NEWOBJECTCODE_H

#include <vector>
#include <string>
#include "Register.h"
using namespace std;

class newObjectCode {
public:
    vector<string> mips;
    int relative_address;	//相对于当前函数指针的地址
    int push_address;		//压入函数参数的地址
    bool begindef = false;
    int end_golbal_address = 0;
    int para_count = 0;
    int push_count = 0;
    RegisterAlloc Registers = RegisterAlloc(10);
    newObjectCode(){
        mips.emplace_back(".data");
        mips.emplace_back("fp: .space 0");
        mips.emplace_back(".text");
        mips.emplace_back("la $gp fp");
        mips.emplace_back("srl $gp $gp 2");
        mips.emplace_back("sll $gp $gp 2");
        mips.emplace_back("addiu $gp $gp 4");
        mips.emplace_back("move $fp $gp");
        genObjectCode();
    }
    void genObjectCode();
    void addCode(struct tetraCode * midCode);
    void printMips();
};


#endif //COMPILER_NEWOBJECTCODE_H
