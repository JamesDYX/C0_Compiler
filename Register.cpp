//
// Created by 段逸骁 on 2019-01-01.
//

#include "Register.h"

#define LOOPWEIGHT 5
using namespace std;
map<string, int > NAMap;
bool isVar(string str){
    return str.find('&')==string::npos;
}
void Register::apply(string var, int * curRelativeAddress, struct tetraCode * current, ofstream &mips){     //对特定寄存器的申请
    if(this->hasAlloc) this->release(curRelativeAddress, current, mips);
    this->hasAlloc = true;
    this->varName = var;
    if(NAMap.find(this->varName)!=NAMap.end()){
        int shiftRd = NAMap.find(this->varName)->second;
        mips << "lw " << this->regName << " " << shiftRd << "(" << ((this->varName[0] == ':') ? "$gp" : "$fp") << ")" << endl;
    }
}
void Register::release(int *curRelativeAddress, struct tetraCode *curCode, ofstream &mips) {    //释放特定寄存器
    bool writeback = false;
    if(isVar(this->varName)) writeback = true;
    else{
        struct tetraCode * tmpCode = curCode;
        while(tmpCode!= nullptr){
            if(tmpCode->op==ADD || tmpCode->op==SUB || tmpCode->op==MULT || tmpCode->op==DIV || tmpCode->op==BEQ || tmpCode->op==BNE || tmpCode->op==BGE || tmpCode->op==BGT){
                if(this->varName==string(tmpCode->rs)){
                    writeback = true;
                    break;
                }
                if(this->varName==string(tmpCode->rt)){
                    writeback = true;
                    break;
                }
            }
            else if(tmpCode->op==LOAD_C || tmpCode->op==LOAD_I){
                if(this->varName==string(tmpCode->rt)){
                    writeback = true;
                    break;
                }
            }
            else if(tmpCode->op==SAVE_C || tmpCode->op==SAVE_I){
                if(this->varName==string(tmpCode->rd)){
                    writeback = true;
                    break;
                }
                if(this->varName==string(tmpCode->rt)){
                    writeback = true;
                    break;
                }
            }
            else if(tmpCode->op==PRINT_I || tmpCode->op==PRINT_C ||
                    tmpCode->op==RET_C || tmpCode->op==RET_I ||
                    tmpCode->op==PUSH_I || tmpCode->op==PUSH_C){
                if(this->varName==string(tmpCode->rd)){
                    writeback = true;
                    break;
                }
            }
            tmpCode = tmpCode->next;
        }
    }
    this->hasAlloc = false;
    if(!writeback) return;
    if (NAMap.find(this->varName) == NAMap.end()) {
        NAMap[string(this->varName)] = *curRelativeAddress;
        *curRelativeAddress += 4;
    }
    int shiftRd = NAMap.find(this->varName)->second;
    mips << "sw " << this->regName << " " << shiftRd << "(" << ((this->varName[0] == ':') ? "$gp" : "$fp") << ")" << endl;
}
map<string,int> counter;
void countUse(string name, struct tetraCode * curCode){
    if(!isVar(name)) return;
    int count = 1;
    struct tetraCode * tmpCode = &MidCode.code_head;
    vector<string> label;
    vector<string> branch;
    while(tmpCode!=curCode){
        if(curCode->op==LAB) label.emplace_back(curCode->rd);
        tmpCode = tmpCode->next;
    }
    while(tmpCode!= nullptr){
        if(tmpCode->op==BEQ || tmpCode->op==BNE ||
           tmpCode->op==BGT || tmpCode->op==BGE ||
           tmpCode->op==JUMP)
            branch.emplace_back(curCode->rd);
        tmpCode = tmpCode->next;
    }
    for(int i=label.size()-1; i>=0; i--){
        for(int j=0; j<branch.size(); j++){
            if(label[i]==branch[j]) count*=LOOPWEIGHT;
        }
    }
    if(counter.find(name)==counter.end())
        counter[name] = count;
    else
        counter[name]+=count;
}
int cmp(const pair<string, int> & pair1, const pair<string, int> & pair2){
    if(pair1.second>pair2.second) return 1;
    else if(pair1.second==pair2.second){
        return pair1.first>pair2.first;
    }
    else return -1;
}
void RegisterAlloc :: referCount(){
    struct tetraCode * curCode = &MidCode.code_head;
    while(curCode!= nullptr){
        if(curCode->op==CONST_I || curCode->op==CONST_C || curCode->op==PARA_I || curCode->op==PARA_C || curCode->op==VAR_I || curCode->op==VAR_C ||
           curCode->op==GET_RET_I || curCode->op==GET_RET_C || curCode->op==PUSH_C || curCode->op==PUSH_I || curCode->op==READ_C || curCode->op==READ_I ||
           curCode->op==PRINT_C || curCode->op==PRINT_I || curCode->op==RET_I || curCode->op==RET_C || curCode->op==LI){
            countUse(curCode->rd, curCode);
        }
        else if(curCode->op==ADD || curCode->op==SUB || curCode->op==MULT || curCode->op==DIV){
            countUse(curCode->rs, curCode);
            countUse(curCode->rt, curCode);
            countUse(curCode->rd, curCode);
        }
        else if(curCode->op==LOAD_I || curCode->op==LOAD_C || curCode->op==SAVE_I || curCode->op==SAVE_C){
            countUse(curCode->rt, curCode);
            countUse(curCode->rd, curCode);
        }
        else if(curCode->op==BEQ || curCode->op==BNE || curCode->op==BGE || curCode->op==BGT){
            countUse(curCode->rs, curCode);
            countUse(curCode->rt, curCode);
        }
        curCode = curCode->next;
    }
    vector <pair<string, int>> vec(counter.begin(), counter.end());
    sort(vec.begin(), vec.end(), cmp);
    for(int i=0; i<8 && i<vec.size(); i++){
        //得到需要分配全局寄存器的变量vec[i].first
        this->globReg[i]->varName = vec[i].first;
        this->globReg[i]->hasAlloc = true;
    }
}
string RegisterAlloc ::alloc(string var, int *curRelativeAddress, struct tetraCode *current, ofstream &mips) {
    for(int i=0; i<4; i++){
        if(this->paraReg[i]->hasAlloc && var==this->paraReg[i]->varName) return this->paraReg[i]->regName;
    }
    for(int i=0; i<8; i++){
        if(this->globReg[i]->hasAlloc && var==this->globReg[i]->varName) return this->globReg[i]->regName;
    }
    for(int i=0; i<this->tempNum; i++){
        if(this->tempReg[i]->hasAlloc && var==this->tempReg[i]->varName) return this->tempReg[i]->regName;
    }
    this->tempReg[this->tempCount]->apply(var, curRelativeAddress, current, mips);
    int oldCount = this->tempCount;
    this->tempCount = (this->tempCount+1)%this->tempNum;
    return this->tempReg[oldCount]->regName;
}