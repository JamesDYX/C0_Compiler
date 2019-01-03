//
// Created by 段逸骁 on 2019-01-02.
//

#include "newObjectCode.h"
#include "fundBlock.h"
#define STR (string("string")+to_string(STR_COUNT++))
int STR_COUNT = 0;

void newObjectCode ::genObjectCode() {
    struct tetraCode * curCode = &MidCode.code_head;
    while(curCode!= nullptr){
        addCode(curCode);
        curCode = curCode->next;
    }
}

struct tetraCode * effectRange(string label){
    struct tetraCode * curCode = &MidCode.code_head;
    while(curCode!= nullptr){
        if(string(curCode->rd)==label && curCode->op!=JUMP) return curCode->next;
        curCode = curCode->next;
    }
    return nullptr;
}

void newObjectCode ::addCode(struct tetraCode * midCode) {
    switch (midCode->op){
        case LI:
        case CONST_C:
        case CONST_I:{
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            Registers.dirt(rd);
            mips.emplace_back("li "+rd+" "+to_string(midCode->rs_num));
            break;
        }
        case FUNC_DEF:{
            if (!begindef) {
                begindef = true;
                end_golbal_address = relative_address;
            }
            relative_address = 0;
            para_count = 0;
            Registers.clearTemp(mips);
            mips.emplace_back(strcat(midCode->rd + 1, ":"));
            break;
        }
        case PARA_I:
        case PARA_C:{
            if(para_count<4){
                Registers.paraReg[para_count]->hasAlloc = true;
                Registers.paraReg[para_count]->varName = midCode->rd;
                Registers.dirt(Registers.paraReg[para_count]->regName);
                para_count++;
            }
            else{
                NAMap[midCode->rd] = relative_address;
                relative_address+=4;
            }
            break;
        }
        case VAR_C:
        case VAR_I:{
            Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            if(midCode->rd[0]==':'){
                NAMap[midCode->rd] = this->relative_address;
                this->relative_address += 4;
            }
            break;
        }
        case ARRAY_C:
        case ARRAY_I:{
            NAMap[midCode->rd] = this->relative_address;
            this->relative_address += 4*midCode->rs_num;
            break;
        }
        case ADD:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("addu "+rd+" "+rs+" "+rt);
            Registers.dirt(rd);
            break;
        }
        case SUB:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("sub "+rd+" "+rs+" "+rt);
            Registers.dirt(rd);
            break;
        }
        case MULT:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("mul "+rd+" "+rs+" "+rt);
            Registers.dirt(rd);
            break;
        }
        case DIV:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("div "+rs+" "+rt);
            mips.emplace_back("mflo "+rd);
            Registers.dirt(rd);
            break;
        }
        case LOAD_C:
        case LOAD_I:{
            int array = NAMap[midCode->rs];
            string shift = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            mips.emplace_back("sll $v1 "+shift+" 2");
            mips.emplace_back("addu $v1 $v1 "+string((midCode->rs[0] == ':') ? "$gp" : "$fp"));
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            Registers.dirt(rd);
            mips.emplace_back("lw "+rd+" "+to_string(array)+"($v1)");
            break;
        }
        case SAVE_C:
        case SAVE_I:{
            int array = NAMap[midCode->rs];
            string shift = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            mips.emplace_back("sll $v1 "+shift+" 2");
            mips.emplace_back("addu $v1 $v1 "+string((midCode->rs[0] == ':') ? "$gp" : "$fp"));
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("sw "+rd+" "+to_string(array)+"($v1)");
            break;
        }
        case CALL:{
            push_count = 0;
            push_address = 0;
            Registers.clearTemp(&relative_address, midCode, mips);  //将寄存器的值软清空，即写回内存
            mips.emplace_back("sw $ra 0($sp)");
            mips.emplace_back("sw $fp -4($sp)");
            mips.emplace_back("addi $sp $sp -8");
            mips.emplace_back("addi $fp $fp "+to_string(relative_address));
            mips.emplace_back("jal "+string(strcat(midCode->rd+1, "")));
            Registers.clearTemp(mips);       //将寄存器的值硬清空，即放弃该寄存器
            mips.emplace_back("addi $sp $sp 8");
            mips.emplace_back("lw $fp -4($sp)");
            mips.emplace_back("lw $ra 0($sp)");
            break;
        }
        case GET_RET_C:
        case GET_RET_I:{
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("move "+rd+" $v0");
            Registers.dirt(rd);
            break;
        }
        case PUSH_I:
        case PUSH_C:{
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            if(push_count<4){
                Registers.paraReg[push_count]->release(&relative_address, midCode, mips);
                mips.emplace_back("move "+Registers.paraReg[push_count]->regName+" "+rd);
                push_count++;
            }
            else{
                mips.push_back("sw "+rd+" "+to_string(push_address+relative_address)+"($fp)");
                push_address+=4;
            }
            break;
        }
        case BEQ:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            Registers.clearTemp(&relative_address, effectRange(midCode->rd), mips);
            mips.emplace_back("beq "+rs+" "+rt+" "+string(midCode->rd));
            break;
        }
        case BNE:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            Registers.clearTemp(&relative_address, effectRange(midCode->rd), mips);
            mips.emplace_back("bne "+rs+" "+rt+" "+string(midCode->rd));
            break;
        }
        case BGT:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            Registers.clearTemp(&relative_address, effectRange(midCode->rd), mips);
            mips.emplace_back("bgt "+rs+" "+rt+" "+string(midCode->rd));
            break;
        }
        case BGE:{
            string rs = strcmp(midCode->rs,":&zero")==0?"$zero":Registers.alloc(midCode->rs, &relative_address, midCode, mips);
            string rt = strcmp(midCode->rt,":&zero")==0?"$zero":Registers.alloc(midCode->rt, &relative_address, midCode, mips);
            Registers.clearTemp(&relative_address, effectRange(midCode->rd), mips);
            mips.emplace_back("bge "+rs+" "+rt+" "+string(midCode->rd));
            break;
        }
        case JUMP:{
            Registers.clearTemp(&relative_address, effectRange(midCode->rd), mips);
            mips.emplace_back("j "+string(midCode->rd));
            break;
        }
        case LAB:{
            Registers.clearTemp(&relative_address, effectRange(midCode->rd), mips);
            Registers.clearTemp(mips);
            mips.emplace_back(strcat(midCode->rd, ":"));
            break;
        }
        case(READ_I):{
            mips.emplace_back("li $v0 5");
            mips.emplace_back("syscall");
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("move "+rd+" $v0");
            Registers.dirt(rd);
            break;
        }
        case READ_C:{
            mips.emplace_back("li $v0 12");
            mips.emplace_back("syscall");
            string rd = Registers.alloc(midCode->rd, &relative_address, midCode, mips);
            mips.emplace_back("move "+rd+" $v0");
            Registers.dirt(rd);
            break;
        }
        case PRINT_I:{
            Registers.paraReg[0]->release(&relative_address, midCode, mips);
            string reg = Registers.alloc(midCode->rd,&relative_address, midCode, mips);
            Registers.paraReg[0]->hasAlloc = false;
            mips.emplace_back("move $a0 "+reg);
            mips.emplace_back("li $v0 1");
            mips.emplace_back("syscall");
            break;
        }
        case PRINT_C:{
            Registers.paraReg[0]->release(&relative_address, midCode, mips);
            string reg = Registers.alloc(midCode->rd,&relative_address, midCode, mips);
            Registers.paraReg[0]->hasAlloc = false;
            mips.emplace_back("move $a0 "+reg);
            mips.emplace_back("li $v0 11");
            mips.emplace_back("syscall");
            break;
        }
        case PRINT_S:{
            Registers.paraReg[0]->release(&relative_address, midCode, mips);
            Registers.paraReg[0]->hasAlloc = false;
            char * tmp = midCode->rd;
            while (*tmp != ':') tmp++;
            tmp++;
            if (strcmp(tmp, "\\n") == 0) {
                mips.emplace_back("li $a0 10");
                mips.emplace_back("li $v0 11");
                mips.emplace_back("syscall");
                break;
            }
            string s = STR;
            int index = 0;
            for(index=0; mips[index]!="fp: .space 0"; index++) ;
            mips.insert(mips.begin()+index, (s + ": .asciiz \"" + tmp + "\""));
            mips.emplace_back("la $a0 "+s);
            mips.emplace_back("li $v0 4");
            mips.emplace_back("syscall");
            break;
        }
        case RET_C:
        case RET_I:{
            string reg = Registers.findVar(midCode->rd);
            if(reg==""){
                int shift = NAMap[midCode->rd];
                mips.emplace_back("lw $v0 "+to_string(shift)+"("+((midCode->rd[0] == ':') ? "$gp" : "$fp")+")");
            }
            else{
                mips.emplace_back("move $v0 "+reg);
            }
            for(int i=0; i<this->Registers.tempCount; i++){
                if(Registers.tempReg[i]->hasAlloc){
                    if(Registers.tempReg[i]->varName[0]==':'){
                        if(Registers.tempReg[i]->isDirty){
                            int shiftRd = NAMap.find(Registers.tempReg[i]->varName)->second;
                            mips.push_back("sw " + Registers.tempReg[i]->regName + " " + to_string(shiftRd) + "($gp)" );
                        }
                    }
                }
            }
        }
        case RET_V:{
            mips.emplace_back("jr $ra");
        }
        default:{
            break;
        }
    }
}
void newObjectCode ::printMips() {
    ofstream outfp;
    outfp.open("newMipsCode.txt");
    for(string s:mips) outfp<<s<<endl;
    outfp << "addi $fp $fp " << end_golbal_address << endl;
    Registers.clearTemp(mips);
    outfp << "jal main" << endl;
}