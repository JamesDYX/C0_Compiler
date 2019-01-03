//
// Created by 段逸骁 on 2019-01-01.
//

#include "Register.h"
#include <algorithm>

#define LOOPWEIGHT 5
using namespace std;
map<string, int > NAMap;
map<string, string> NRMap;
bool isVar(string str){
    return str.find('&')==string::npos;
}
void Register::apply(string var, int * curRelativeAddress, struct tetraCode * current, vector<string> &mips){     //对特定寄存器的申请
    if(this->hasAlloc) this->release(curRelativeAddress, current, mips);
    this->hasAlloc = true;
    this->isDirty = false;
    this->varName = var;
    if(NAMap.find(this->varName)!=NAMap.end()){
        int shiftRd = NAMap.find(this->varName)->second;
        mips.push_back(string("lw " + this->regName + " " + to_string(shiftRd) + "(" + ((this->varName[0] == ':') ? "$gp" : "$fp") + ")")) ;
    }
}
void Register::release(int *curRelativeAddress, struct tetraCode *curCode, vector<string> &mips) {    //释放特定寄存器
    if(!this->hasAlloc) return;
    if(!this->isDirty){
        return;
    }
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
                if(this->varName==string(tmpCode->rd)){
                    writeback = false;
                    break;
                }
            }
            else if(tmpCode->op==LOAD_C || tmpCode->op==LOAD_I){
                if(this->varName==string(tmpCode->rt)){
                    writeback = true;
                    break;
                }
                if(this->varName==string(tmpCode->rd)){
                    writeback = false;
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
    this->isDirty = false;
    if(!writeback) return;
    if (NAMap.find(this->varName) == NAMap.end()) {
        NAMap[string(this->varName)] = *curRelativeAddress;
        *curRelativeAddress += 4;
    }
    int shiftRd = NAMap.find(this->varName)->second;
    mips.push_back("sw " + this->regName + " " + to_string(shiftRd) + "(" + ((this->varName[0] == ':') ? "$gp" : "$fp") + ")" );
    if(NRMap.find(this->varName) == NRMap.end()){
        NRMap[this->varName] = this->regName;
    }
}

struct FuncLink{
    string funcName;
    vector<struct FuncLink *> succFuncs;
    bool hasBeen;
    bool isRecur = false;
};
map<string, struct FuncLink *> FuncMap;
bool checkFunc(struct FuncLink * Func, string name){
    if(Func->funcName==name) return true;
    Func->hasBeen = true;
    for(struct FuncLink * check:Func->succFuncs){
        if(check->hasBeen) continue;
        if(checkFunc(check, name)) return true;
    }
    return false;
}
void checkRecur(){
    struct tetraCode * curCode = &MidCode.code_head;
    string curFunc = "";
    while(curCode!= nullptr){
        if(curCode->op==FUNC_DEF){
            if(strcmp(curCode->rd,":main")==0) break;
            struct FuncLink * newFunc = new FuncLink;
            newFunc->funcName = curCode->rd;
            curFunc = newFunc->funcName;
            FuncMap[newFunc->funcName] = newFunc;
        }
        else if(curCode->op==CALL){
            if(FuncMap.find(curCode->rd)==FuncMap.end()) break;
            struct FuncLink * callFunc = FuncMap[curCode->rd];
            FuncMap[curFunc]->succFuncs.emplace_back(callFunc);
        }
        curCode = curCode->next;
    }
    map<string, struct FuncLink *>::iterator iter;
    iter = FuncMap.begin();
    while(iter != FuncMap.end()) {
        map<string, struct FuncLink *>::iterator itertmp;
        itertmp = FuncMap.begin();
        while(itertmp!=FuncMap.end()){
            itertmp->second->hasBeen = false;
            itertmp++;
        }
        iter->second->hasBeen = true;
        bool isRecur = false;
        for(struct FuncLink * check:iter->second->succFuncs){
            isRecur = checkFunc(check, iter->first);
            if(isRecur) break;
        }
        iter->second->isRecur = isRecur;
        iter++;
    }
}
bool isInRecur(string var){
    int i = 0;
    for(i=0; i<var.length(); i++){
        if(var[i]==':') break;
    }
    string funcname = var.substr(0, i);
    if(funcname=="main" || funcname=="") return false;
    else return FuncMap[":"+funcname]->isRecur;
}

map<string,int> counter;
bool isPara(string name){
    struct tetraCode * curCode = &MidCode.code_head;
    while(curCode!= nullptr){
        if(curCode->op==PARA_C || curCode->op==PARA_I){
            if(name==string(curCode->rd)) return true;
        }
        curCode = curCode->next;
    }
    return false;
}
void countUse(string name, struct tetraCode * curCode){
    if(!isVar(name)) return;
    if(isInRecur(name)) return;
    if(isPara(name)) return;
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

	if(pair1.second>pair2.second) return true;
    else if(pair1.second==pair2.second){
        return pair1.first>pair2.first;
    }
    else return false;
}
void RegisterAlloc :: referCount(){
    checkRecur();
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
string RegisterAlloc ::alloc(string var, int *curRelativeAddress, struct tetraCode *current, vector<string> &mips) {
    string reg = findVar(var);
    if(reg!="") return reg;

    int tryToUse = this->tempCount;
    while(tempReg[tryToUse]->hasAlloc){
        tryToUse = (tryToUse+1)%this->tempNum;
        if(tryToUse == this->tempCount) break;
    }
    tempCount = tryToUse;
    this->tempReg[this->tempCount]->apply(var, curRelativeAddress, current, mips);
    this->tempCount = (this->tempCount+1)%this->tempNum;
    return this->tempReg[tryToUse]->regName;
    //检查寄存器分配记录，观察其是否被分配过
//    if(NRMap.find(var) == NRMap.end()){
//        //没有被分配过，则分配一个临时寄存器
//        int tryToUse = this->tempCount;
//        while(tempReg[tryToUse]->hasAlloc){
//            tryToUse = (tryToUse+1)%this->tempNum;
//            if(tryToUse == this->tempCount) break;
//        }
//        tempCount = tryToUse;
//        this->tempReg[this->tempCount]->apply(var, curRelativeAddress, current, mips);
//        this->tempCount = (this->tempCount+1)%this->tempNum;
//        return this->tempReg[tryToUse]->regName;
//    }
//    else{
//        //分配指定寄存器
//        string reg = NRMap[var];
//        if(reg[1]=='a'){
//            paraReg[reg[2]-'0']->apply(var, curRelativeAddress, current, mips);
//        }
//        else if(reg[1]=='t'){
//            tempReg[reg[2]-'0']->apply(var, curRelativeAddress, current, mips);
//            if(tempCount==reg[2]-'0') tempCount = (tempCount+1)%tempNum;
//        }
//        return reg;
//    }
}
void RegisterAlloc ::dirt(string name) {
    if(name[1]=='a'){
        paraReg[name[2]-'0']->isDirty = true;
    }
    else if(name[1]=='s'){
        globReg[name[2]-'0']->isDirty = true;
    }
    else{
        tempReg[name[2]-'0']->isDirty = true;
    }
}
string RegisterAlloc::findVar(string var){
    for(int i=0; i<4; i++){
        if(this->paraReg[i]->hasAlloc && var==this->paraReg[i]->varName) return this->paraReg[i]->regName;
    }
    for(int i=0; i<8; i++){
        if(this->globReg[i]->hasAlloc && var==this->globReg[i]->varName) return this->globReg[i]->regName;
    }
    for(int i=0; i<this->tempNum; i++){
        if(this->tempReg[i]->hasAlloc && var==this->tempReg[i]->varName) return this->tempReg[i]->regName;
    }
    return "";
}
void RegisterAlloc ::clearTemp(int * curRelativeAddress, struct tetraCode * curCode, vector<string> &mips) {
    for(int i=0; i<tempNum; i++){
        tempReg[i]->release(curRelativeAddress, curCode, mips);
    }
    for(int i=0; i<4; i++){
        paraReg[i]->release(curRelativeAddress, curCode, mips);
    }
    this->tempCount = 0;
}
void RegisterAlloc ::clearTemp(vector<string> &mips) {
    for(int i=0; i<tempNum; i++){
        if(tempReg[i]->hasAlloc){
            if(tempReg[i]->varName[0]==':'){
                if(!tempReg[i]->isDirty) tempReg[i]->hasAlloc = false;
            }
            else tempReg[i]->hasAlloc = false;
        }
    }
    for(int i = 0; i<4; i++)
        paraReg[i]->hasAlloc = false;
    this->tempCount = 0;
}