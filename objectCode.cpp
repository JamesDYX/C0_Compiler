#include "objectCode.h"

int STRING_COUNT = 0;
using namespace std;

objectCode::objectCode()
{
	this->head = (struct objectNode *)malloc(sizeof(objectNode));
	this->now = head;
	this->head->next = NULL;
	this->head->prev = NULL;
	this->head->mipsCode = nop;
	this->relative_address = 0;
	this->push_address = 0;
	this->outfp.open("mipsCode.txt");

	objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, ".data");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;



	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, "fp: .space 0");
	tmp->next = nullptr;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

	this->fp = now;

	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, ".text");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, "la $gp, fp");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, "li $t0, 0xfffffffc");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, "and $gp, $gp, $t0");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, "addi $gp, $gp, 4");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

	tmp = (struct objectNode *)malloc(sizeof(objectNode));
	tmp->mipsCode = lab;
	strcpy(tmp->rd_reg, "move $fp, $gp");
	tmp->next = NULL;
	tmp->prev = now;
	now->next = tmp;
	now = tmp;

}

void objectCode::addNode(tetraCode midCode) {
	switch (midCode.op)
	{
	case(CONST_I):
	case(CONST_C):
	{
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			this->relative_address += 4;
		}

		int shiftRd = Map.find(midCode.rd)->second;
		
		struct objectNode * node1 = (struct objectNode *)malloc(sizeof(objectNode));
		node1->mipsCode = li;
		strcpy(node1->rd_reg, "$t0");
		node1->rs_num = midCode.rs_num;
		node1->next = NULL;
		node1->prev = now;
		now->next = node1;
		now = node1;

		struct objectNode * node2 = (struct objectNode *)malloc(sizeof(objectNode));
		node2->mipsCode = sw;
		strcpy(node2->rd_reg, "$t0");
		strcpy(node2->rs_reg, (midCode.rd[0] == ':') ? "$gp" : "$fp");
		node2->rt_num = shiftRd;
		node2->next = NULL;
		node2->prev = now;
		now->next = node2;
		now = node2;

		break;
	}
	case (FUNC_DEF):
	{
		if (!begindef) {
			begindef = true;
			end_golbal_address = relative_address;
		}
		relative_address = 0;
		struct objectNode * node1 = (struct objectNode *)malloc(sizeof(objectNode));
		node1->mipsCode = lab;
		strcpy(node1->rd_reg, strcat(midCode.rd + 1, ":"));
		node1->next = NULL;
		node1->prev = now;
		now->next = node1;
		now = node1;
		break;
	}
	case (PARA_I):
	case (PARA_C):
	case (VAR_I):
	case (VAR_C):
	{
		Map[string(midCode.rd)] = this->relative_address;
		this->relative_address += 4;
		break;
	}
	case (ARRAY_I):
	case (ARRAY_C):
	{
		Map[string(midCode.rd)] = this->relative_address;
		this->relative_address += 4*midCode.rs_num;
		break;
	}
	case (ADD):
	{
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			relative_address += 4;
		}
		int shiftRd = Map.find(midCode.rd)->second;
		
		if (strcmp(midCode.rs, ":&zero") != 0) {
			int shift1 = Map.find(midCode.rs)->second;
			struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
			load1->mipsCode = lw;
			strcpy(load1->rd_reg, "$t0");
			strcpy(load1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
			load1->rt_num = shift1;
			load1->next = NULL;
			load1->prev = now;
			now->next = load1;
			now = load1;
		}

		if (strcmp(midCode.rt, ":&zero") != 0) {
			int shift2 = Map.find(midCode.rt)->second;
			struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
			load2->mipsCode = lw;
			strcpy(load2->rd_reg, "$t1");
			strcpy(load2->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
			load2->rt_num = shift2;
			load2->next = NULL;
			load2->prev = now;
			now->next = load2;
			now = load2;
		}

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = add;

		strcpy(tmp->rd_reg, "$t2");
		strcpy(tmp->rs_reg, (strcmp(midCode.rs, ":&zero") != 0) ? "$t0" : "$0");
		strcpy(tmp->rt_reg, (strcmp(midCode.rt, ":&zero") != 0) ? "$t1" : "$0");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;

		strcpy(save->rd_reg, "$t2");
		strcpy(save->rs_reg, (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shiftRd;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (SUB):
	{
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			relative_address += 4;
		}
		int shiftRd = Map.find(midCode.rd)->second;

		if (strcmp(midCode.rs, ":&zero") != 0) {
			int shift1 = Map.find(midCode.rs)->second;
			struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
			load1->mipsCode = lw;
			strcpy(load1->rd_reg, "$t0");
			strcpy(load1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
			load1->rt_num = shift1;
			load1->next = NULL;
			load1->prev = now;
			now->next = load1;
			now = load1;
		}

		if (strcmp(midCode.rt, ":&zero") != 0) {
			int shift2 = Map.find(midCode.rt)->second;
			struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
			load2->mipsCode = lw;
			strcpy(load2->rd_reg, "$t1");
			strcpy(load2->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
			load2->rt_num = shift2;
			load2->next = NULL;
			load2->prev = now;
			now->next = load2;
			now = load2;
		}

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = sub;

		strcpy(tmp->rd_reg, "$t2");
		strcpy(tmp->rs_reg, (strcmp(midCode.rs, ":&zero") != 0) ? "$t0" : "$0");
		strcpy(tmp->rt_reg, (strcmp(midCode.rt, ":&zero") != 0) ? "$t1" : "$0");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;

		strcpy(save->rd_reg, "$t2");
		strcpy(save->rs_reg, (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shiftRd;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (MULT):
	{
		int shift1 = Map.find(midCode.rs)->second;
		int shift2 = Map.find(midCode.rt)->second;
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			relative_address += 4;
		}
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
		load1->mipsCode = lw;
		strcpy(load1->rd_reg, "$t0");
		strcpy(load1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
		load1->rt_num = shift1;
		load1->next = NULL;
		load1->prev = now;
		now->next = load1;
		now = load1;

		struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
		load2->mipsCode = lw;
		strcpy(load2->rd_reg, "$t1");
		strcpy(load2->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
		load2->rt_num = shift2;
		load2->next = NULL;
		load2->prev = now;
		now->next = load2;
		now = load2;

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = mult;
		strcpy(tmp->rs_reg, "$t0");
		strcpy(tmp->rt_reg, "$t1");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;

		struct objectNode * tmp2 = (struct objectNode *)malloc(sizeof(objectNode));
		tmp2->mipsCode = mflo;
		strcpy(tmp2->rd_reg, "$t2");
		tmp2->next = NULL;
		tmp2->prev = now;
		now->next = tmp2;
		now = tmp2;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$t2");
		strcpy(save->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shiftRd;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (DIV):
	{
		int shift1 = Map.find(midCode.rs)->second;
		int shift2 = Map.find(midCode.rt)->second;
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			relative_address += 4;
		}
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
		load1->mipsCode = lw;
		strcpy(load1->rd_reg, "$t0");
		strcpy(load1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
		load1->rt_num = shift1;
		load1->next = NULL;
		load1->prev = now;
		now->next = load1;
		now = load1;

		struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
		load2->mipsCode = lw;
		strcpy(load2->rd_reg, "$t1");
		strcpy(load2->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
		load2->rt_num = shift2;
		load2->next = NULL;
		load2->prev = now;
		now->next = load2;
		now = load2;

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = divid;
		strcpy(tmp->rs_reg, "$t0");
		strcpy(tmp->rt_reg, "$t1");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;

		struct objectNode * tmp2 = (struct objectNode *)malloc(sizeof(objectNode));
		tmp2->mipsCode = mflo;
		strcpy(tmp2->rd_reg, "$t2");
		tmp2->next = NULL;
		tmp2->prev = now;
		now->next = tmp2;
		now = tmp2;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg, "$t2");
		strcpy(save->rs_reg, (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shiftRd;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (LOAD_I):
	case (LOAD_C):
	{
		int shift_array = Map.find(midCode.rs)->second;
		int shift_shift = Map.find(midCode.rt)->second;
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			relative_address += 4;
		}
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
		load1->mipsCode = lw;
		strcpy(load1->rd_reg, "$t0");
		strcpy(load1->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
		load1->rt_num = shift_shift;
		load1->next = NULL;
		load1->prev = now;
		now->next = load1;
		now = load1;

		struct objectNode * sll1 = (struct objectNode *)malloc(sizeof(objectNode));
		sll1->mipsCode = sll;
		strcpy(sll1->rd_reg, "$t0");
		strcpy(sll1->rs_reg, "$t0");
		sll1->rt_num = 2;
		sll1->next = NULL;
		sll1->prev = now;
		now->next = sll1;
		now = sll1;

		struct objectNode * add1 = (struct objectNode *)malloc(sizeof(objectNode));
		add1->mipsCode = add;
		strcpy(add1->rd_reg , "$t0");
		strcpy(add1->rs_reg , (midCode.rs[0] == ':') ? "$gp" : "$fp");
		strcpy(add1->rt_reg , "$t0");
		add1->next = NULL;
		add1->prev = now;
		now->next = add1;
		now = add1;

		struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
		load2->mipsCode = lw;
		strcpy(load2->rd_reg , "$t1");
		strcpy(load2->rs_reg , "$t0");
		load2->rt_num = shift_array;
		load2->next = NULL;
		load2->prev = now;
		now->next = load2;
		now = load2;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$t1");
		strcpy(save->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shiftRd;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (SAVE_I):
	case (SAVE_C):
	{
		int shift_array = Map.find(midCode.rs)->second;
		int shift_shift = Map.find(midCode.rt)->second;
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
		load1->mipsCode = lw;
		strcpy(load1->rd_reg , "$t0");
		strcpy(load1->rs_reg , (midCode.rt[0] == ':') ? "$gp" : "$fp");
		load1->rt_num = shift_shift;
		load1->next = NULL;
		load1->prev = now;
		now->next = load1;
		now = load1;

		struct objectNode * sll1 = (struct objectNode *)malloc(sizeof(objectNode));
		sll1->mipsCode = sll;
		strcpy(sll1->rd_reg, "$t0");
		strcpy(sll1->rs_reg, "$t0");
		sll1->rt_num = 2;
		sll1->next = NULL;
		sll1->prev = now;
		now->next = sll1;
		now = sll1;

		struct objectNode * add1 = (struct objectNode *)malloc(sizeof(objectNode));
		add1->mipsCode = add;
		strcpy(add1->rd_reg, "$t0");
		strcpy(add1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
		strcpy(add1->rt_reg, "$t0");
		add1->next = NULL;
		add1->prev = now;
		now->next = add1;
		now = add1;

		struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
		load2->mipsCode = lw;
		strcpy(load2->rd_reg , "$t1");
		strcpy(load2->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		load2->rt_num = shiftRd;
		load2->next = NULL;
		load2->prev = now;
		now->next = load2;
		now = load2;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$t1");
		strcpy(save->rs_reg , "$t0");
		save->rt_num = shift_array;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (CALL):
	{
		this->push_address = 0;
		struct objectNode * save_ra = (struct objectNode *)malloc(sizeof(objectNode));
		save_ra->mipsCode = sw;
		strcpy(save_ra->rd_reg , "$ra");
		strcpy(save_ra->rs_reg , "$sp");
		save_ra->rt_num = 0;
		save_ra->next = NULL;
		save_ra->prev = now;
		now->next = save_ra;
		now = save_ra;
		
		struct objectNode * push = (struct objectNode *)malloc(sizeof(objectNode));
		push->mipsCode = addi;
		strcpy(push->rd_reg , "$sp");
		strcpy(push->rs_reg , "$sp");
		push->rt_num = -4;
		push->next = NULL;
		push->prev = now;
		now->next = push;
		now = push;
		
		struct objectNode * save_fp = (struct objectNode *)malloc(sizeof(objectNode));
		save_fp->mipsCode = sw;
		strcpy(save_fp->rd_reg, "$fp");
		strcpy(save_fp->rs_reg, "$sp");
		save_fp->rt_num = 0;
		save_fp->next = NULL;
		save_fp->prev = now;
		now->next = save_fp;
		now = save_fp;

		push = (struct objectNode *)malloc(sizeof(objectNode));
		push->mipsCode = addi;
		strcpy(push->rd_reg, "$sp");
		strcpy(push->rs_reg, "$sp");
		push->rt_num = -4;
		push->next = NULL;
		push->prev = now;
		now->next = push;
		now = push;
		
		struct objectNode * set_fp = (struct objectNode *)malloc(sizeof(objectNode));
		set_fp->mipsCode = addi;
		strcpy(set_fp->rd_reg , "$fp");
		strcpy(set_fp->rs_reg , "$fp");
		set_fp->rt_num = relative_address;
		set_fp->next = NULL;
		set_fp->prev = now;
		now->next = set_fp;
		now = set_fp;

		
		struct objectNode * call = (struct objectNode *)malloc(sizeof(objectNode));
		call->mipsCode = jal;
		strcpy(call->rd_reg , midCode.rd+1);
		call->next = NULL;
		call->prev = now;
		now->next = call;
		now = call;

		struct objectNode * pop = (struct objectNode *)malloc(sizeof(objectNode));
		pop->mipsCode = addi;
		strcpy(pop->rd_reg , "$sp");
		strcpy(pop->rs_reg , "$sp");
		pop->rt_num = 4;
		pop->next = NULL;
		pop->prev = now;
		now->next = pop;
		now = pop;

		save_fp = (struct objectNode *)malloc(sizeof(objectNode));
		save_fp->mipsCode = lw;
		strcpy(save_fp->rd_reg , "$fp");
		strcpy(save_fp->rs_reg , "$sp");
		save_fp->rt_num = 0;
		save_fp->next = NULL;
		save_fp->prev = now;
		now->next = save_fp;
		now = save_fp;

		pop = (struct objectNode *)malloc(sizeof(objectNode));
		pop->mipsCode = addi;
		strcpy(pop->rd_reg, "$sp");
		strcpy(pop->rs_reg, "$sp");
		pop->rt_num = 4;
		pop->next = NULL;
		pop->prev = now;
		now->next = pop;
		now = pop;

		save_ra = (struct objectNode *)malloc(sizeof(objectNode));
		save_ra->mipsCode = lw;
		strcpy(save_ra->rd_reg , "$ra");
		strcpy(save_ra->rs_reg , "$sp");
		save_ra->rt_num = 0;
		save_ra->next = NULL;
		save_ra->prev = now;
		now->next = save_ra;
		now = save_ra;

		break;


	}
	case (GET_RET_C):
	case (GET_RET_I):
	{
		if (Map.find(midCode.rd) == Map.end()) {
			Map[string(midCode.rd)] = this->relative_address;
			relative_address += 4;
		}
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$v0");
		strcpy(save->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shiftRd;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		break;
	}
	case (PUSH_C):
	case (PUSH_I):
	{
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * load = (struct objectNode *)malloc(sizeof(objectNode));
		load->mipsCode = lw;
		strcpy(load->rd_reg , "$t0");
		strcpy(load->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		load->rt_num = shiftRd;
		load->next = NULL;
		load->prev = now;
		now->next = load;
		now = load;


		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$t0");
		strcpy(save->rs_reg , "$fp");
		save->rt_num = push_address+relative_address;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;

		push_address += 4;
		break;
	}
	case (BEQ):
	{

		if (strcmp(midCode.rs, ":&zero") != 0) {
			int shift1 = Map.find(midCode.rs)->second;
			struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
			load1->mipsCode = lw;
			strcpy(load1->rd_reg , "$t0");
			strcpy(load1->rs_reg , (midCode.rs[0] == ':') ? "$gp" : "$fp");
			load1->rt_num = shift1;
			load1->next = NULL;
			load1->prev = now;
			now->next = load1;
			now = load1;
		}

		if (strcmp(midCode.rt, ":&zero") != 0) {
			int shift2 = Map.find(midCode.rt)->second;
			struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
			load2->mipsCode = lw;
			strcpy(load2->rd_reg , "$t1");
			strcpy(load2->rs_reg , (midCode.rt[0] == ':') ? "$gp" : "$fp");
			load2->rt_num = shift2;
			load2->next = NULL;
			load2->prev = now;
			now->next = load2;
			now = load2;
		}

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = beq;
		strcpy(tmp->rd_reg , midCode.rd);
		strcpy(tmp->rs_reg , (strcmp(midCode.rs, ":&zero") != 0) ? "$t0" : "$0");
		strcpy(tmp->rt_reg , (strcmp(midCode.rt, ":&zero") != 0) ? "$t1" : "$0");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;
		break;
	}
	case (BNE):
	{
		if (strcmp(midCode.rs, ":&zero") != 0) {
			int shift1 = Map.find(midCode.rs)->second;
			struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
			load1->mipsCode = lw;
			strcpy(load1->rd_reg, "$t0");
			strcpy(load1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
			load1->rt_num = shift1;
			load1->next = NULL;
			load1->prev = now;
			now->next = load1;
			now = load1;
		}

		if (strcmp(midCode.rt, ":&zero") != 0) {
			int shift2 = Map.find(midCode.rt)->second;
			struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
			load2->mipsCode = lw;
			strcpy(load2->rd_reg, "$t1");
			strcpy(load2->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
			load2->rt_num = shift2;
			load2->next = NULL;
			load2->prev = now;
			now->next = load2;
			now = load2;
		}

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = bne;
		strcpy(tmp->rd_reg, midCode.rd);
		strcpy(tmp->rs_reg, (strcmp(midCode.rs, ":&zero") != 0) ? "$t0" : "$0");
		strcpy(tmp->rt_reg, (strcmp(midCode.rt, ":&zero") != 0) ? "$t1" : "$0");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;
		break;
	}
	case (BGT):
	{
		int shift1 = Map.find(midCode.rs)->second;
		int shift2 = Map.find(midCode.rt)->second;

		struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
		load1->mipsCode = lw;
		strcpy(load1->rd_reg , "$t0");
		strcpy(load1->rs_reg , (midCode.rs[0] == ':') ? "$gp" : "$fp");
		load1->rt_num = shift1;
		load1->next = NULL;
		load1->prev = now;
		now->next = load1;
		now = load1;

		struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
		load2->mipsCode = lw;
		strcpy(load2->rd_reg , "$t1");
		strcpy(load2->rs_reg , (midCode.rt[0] == ':') ? "$gp" : "$fp");
		load2->rt_num = shift2;
		load2->next = NULL;
		load2->prev = now;
		now->next = load2;
		now = load2;

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = bgt;
		strcpy(tmp->rd_reg , midCode.rd);
		strcpy(tmp->rs_reg , "$t0");
		strcpy(tmp->rt_reg , "$t1");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;
		break;
	}
	case (BGE):
	{
		int shift1 = Map.find(midCode.rs)->second;
		int shift2 = Map.find(midCode.rt)->second;

		struct objectNode * load1 = (struct objectNode *)malloc(sizeof(objectNode));
		load1->mipsCode = lw;
		strcpy(load1->rd_reg, "$t0");
		strcpy(load1->rs_reg, (midCode.rs[0] == ':') ? "$gp" : "$fp");
		load1->rt_num = shift1;
		load1->next = NULL;
		load1->prev = now;
		now->next = load1;
		now = load1;

		struct objectNode * load2 = (struct objectNode *)malloc(sizeof(objectNode));
		load2->mipsCode = lw;
		strcpy(load2->rd_reg, "$t1");
		strcpy(load2->rs_reg, (midCode.rt[0] == ':') ? "$gp" : "$fp");
		load2->rt_num = shift2;
		load2->next = NULL;
		load2->prev = now;
		now->next = load2;
		now = load2;

		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = bge;
		strcpy(tmp->rd_reg, midCode.rd);
		strcpy(tmp->rs_reg, "$t0");
		strcpy(tmp->rt_reg, "$t1");
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;
		break;
	}
	case (JUMP):
	{
		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = j;
		strcpy(tmp->rd_reg , midCode.rd);
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;
		break;
	}
	case (LAB):
	{
		struct objectNode * tmp = (struct objectNode *)malloc(sizeof(objectNode));
		tmp->mipsCode = lab;
		strcpy(tmp->rd_reg , strcat(midCode.rd, ":"));
		tmp->next = NULL;
		tmp->prev = now;
		now->next = tmp;
		now = tmp;
		break;
	}
	case (READ_I):
	{
		struct objectNode * v0 = (struct objectNode *)malloc(sizeof(objectNode));
		v0->mipsCode = li;
		strcpy(v0->rd_reg , "$v0");
		v0->rs_num = 5;
		v0->next = NULL;
		v0->prev = now;
		now->next = v0;
		now = v0;

		struct objectNode * sys = (struct objectNode *)malloc(sizeof(objectNode));
		sys->mipsCode = syscall;
		sys->next = NULL;
		sys->prev = now;
		now->next = sys;
		now = sys;

		int shift = Map.find(midCode.rd)->second;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$v0");
		strcpy(save->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shift;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;
		break;
	}
	case (READ_C):
	{
		struct objectNode * v0 = (struct objectNode *)malloc(sizeof(objectNode));
		v0->mipsCode = li;
		strcpy(v0->rd_reg , "$v0");
		v0->rs_num = 12;
		v0->next = NULL;
		v0->prev = now;
		now->next = v0;
		now = v0;

		struct objectNode * sys = (struct objectNode *)malloc(sizeof(objectNode));
		sys->mipsCode = syscall;
		sys->next = NULL;
		sys->prev = now;
		now->next = sys;
		now = sys;

		int shift = Map.find(midCode.rd)->second;

		struct objectNode * save = (struct objectNode *)malloc(sizeof(objectNode));
		save->mipsCode = sw;
		strcpy(save->rd_reg , "$v0");
		strcpy(save->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		save->rt_num = shift;
		save->next = NULL;
		save->prev = now;
		now->next = save;
		now = save;
		break;
	}
	case (PRINT_I):
	{
		int shift = Map.find(midCode.rd)->second;

		struct objectNode * load = (struct objectNode *)malloc(sizeof(objectNode));
		load->mipsCode = lw;
		strcpy(load->rd_reg , "$a0");
		strcpy(load->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		load->rt_num = shift;
		load->next = NULL;
		load->prev = now;
		now->next = load;
		now = load;

		struct objectNode * v0 = (struct objectNode *)malloc(sizeof(objectNode));
		v0->mipsCode = li;
		strcpy(v0->rd_reg , "$v0");
		v0->rs_num = 1;
		v0->next = NULL;
		v0->prev = now;
		now->next = v0;
		now = v0;

		struct objectNode * sys = (struct objectNode *)malloc(sizeof(objectNode));
		sys->mipsCode = syscall;
		sys->next = NULL;
		sys->prev = now;
		now->next = sys;
		now = sys;

		break;
	}
	case (PRINT_C):
	{
		int shift = Map.find(midCode.rd)->second;

		struct objectNode * load = (struct objectNode *)malloc(sizeof(objectNode));
		load->mipsCode = lw;
		strcpy(load->rd_reg, "$a0");
		strcpy(load->rs_reg, (midCode.rd[0] == ':') ? "$gp" : "$fp");
		load->rt_num = shift;
		load->next = NULL;
		load->prev = now;
		now->next = load;
		now = load;

		struct objectNode * v0 = (struct objectNode *)malloc(sizeof(objectNode));
		v0->mipsCode = li;
		strcpy(v0->rd_reg, "$v0");
		v0->rs_num = 11;
		v0->next = NULL;
		v0->prev = now;
		now->next = v0;
		now = v0;

		struct objectNode * sys = (struct objectNode *)malloc(sizeof(objectNode));
		sys->mipsCode = syscall;
		sys->next = NULL;
		sys->prev = now;
		now->next = sys;
		now = sys;

		break;
	}
	case (PRINT_S):
	{
		char * tmp = midCode.rd;
		while (*tmp != ':') tmp++;
		tmp++;
		if (strcmp(tmp, "\\n") == 0) {
			struct objectNode * v0 = (struct objectNode *)malloc(sizeof(objectNode));
			v0->mipsCode = li;
			strcpy(v0->rd_reg, "$a0");
			v0->rs_num = 10;
			v0->next = NULL;
			v0->prev = now;
			now->next = v0;
			now = v0;

			v0 = (struct objectNode *)malloc(sizeof(objectNode));
			v0->mipsCode = li;
			strcpy(v0->rd_reg, "$v0");
			v0->rs_num = 11;
			v0->next = NULL;
			v0->prev = now;
			now->next = v0;
			now = v0;

			v0 = (struct objectNode *)malloc(sizeof(objectNode));
			v0->mipsCode = syscall;
			v0->next = NULL;
			v0->prev = now;
			now->next = v0;
			now = v0;

			break;
		}
		string s = STRING;
		struct objectNode * str = (struct objectNode *)malloc(sizeof(objectNode));
		str->mipsCode = lab;
		strcpy(str->rd_reg , (s + ": .asciiz \"" + tmp + "\"").c_str());
		str->next = fp;
		str->prev = fp->prev;
		fp->prev->next = str;
		fp->prev = str;

		struct objectNode * load = (struct objectNode *)malloc(sizeof(objectNode));
		load->mipsCode = la;
		strcpy(load->rd_reg , "$a0");
		strcpy(load->rs_reg , s.c_str());
		load->next = NULL;
		load->prev = now;
		now->next = load;
		now = load;

		struct objectNode * v0 = (struct objectNode *)malloc(sizeof(objectNode));
		v0->mipsCode = li;
		strcpy(v0->rd_reg , "$v0");
		v0->rs_num = 4;
		v0->next = NULL;
		v0->prev = now;
		now->next = v0;
		now = v0;

		struct objectNode * sys = (struct objectNode *)malloc(sizeof(objectNode));
		sys->mipsCode = syscall;
		sys->next = NULL;
		sys->prev = now;
		now->next = sys;
		now = sys;

		break;
	}
	case (RET_C):
	case (RET_I):
	{
		int shiftRd = Map.find(midCode.rd)->second;

		struct objectNode * load = (struct objectNode *)malloc(sizeof(objectNode));
		load->mipsCode = lw;
		strcpy(load->rd_reg , "$v0");
		strcpy(load->rs_reg , (midCode.rd[0] == ':') ? "$gp" : "$fp");
		load->rt_num = shiftRd;
		load->next = NULL;
		load->prev = now;
		now->next = load;
		now = load;

	}
	case (RET_V):
	{

		struct objectNode * call = (struct objectNode *)malloc(sizeof(objectNode));
		call->mipsCode = jr;
		strcpy(call->rd_reg , "$ra");
		call->next = NULL;
		call->prev = now;
		now->next = call;
		now = call;

		break;
	}
	default:
	{
		break;
	}
	}
}

void objectCode ::printMips() {
	tetraCode * curCode = &MidCode.code_head;
	while (curCode != NULL) {
		addNode(*curCode);
		curCode = curCode->next;
	}
	struct objectNode * curMips = this->head;
	while (curMips!=NULL)
	{
		switch (curMips->mipsCode)
		{
		case(li):
		{
			outfp << "li " << curMips->rd_reg << " " << curMips->rs_num << endl;
			break;
		}
		case(add):
		{
			outfp << "add " << curMips->rd_reg << " " << curMips->rs_reg << " " << curMips->rt_reg << endl;
			break;
		}
		case(addi):
		{
			outfp << "addi " << curMips->rd_reg << " " << curMips->rs_reg << " " << curMips->rt_num << endl;
			break;
		}
		case(sub):
		{
			outfp << "sub " << curMips->rd_reg << " " << curMips->rs_reg << " " << curMips->rt_reg << endl;
			break;
		}
		case(sll):
		{
			outfp << "sll " << curMips->rd_reg << " " << curMips->rs_reg << " " << curMips->rt_num << endl;
			break;
		}
		case(mult):
		{
			outfp << "mult " << curMips->rs_reg << " " << curMips->rt_reg << endl;
			break;
		}
		case(divid):
		{
			outfp << "div " << curMips->rs_reg << " " << curMips->rt_reg << endl;
			break;
		}
		case(mflo):
		{
			outfp << "mflo " << curMips->rd_reg << endl;
			break;
		}
		case(sw):
		{
			outfp << "sw " << curMips->rd_reg << " " << curMips->rt_num << "(" << curMips->rs_reg << ")" << endl;
			break;
		}
		case(lw):
		{
			outfp << "lw " << curMips->rd_reg << " " << curMips->rt_num << "(" << curMips->rs_reg << ")"  <<endl;
			break;
		}
		case(la):
		{
			outfp << "la " << curMips->rd_reg << " " << curMips->rs_reg << endl;
			break;
		}
		case (beq):
		{
			outfp << "beq " << curMips->rs_reg << " " << curMips->rt_reg << " " << curMips->rd_reg <<  endl;
			break;
		}
		case (bne):
		{
			outfp << "bne " << curMips->rs_reg << " " << curMips->rt_reg << " " << curMips->rd_reg << endl;
			break;
		}
		case (bge):
		{
			outfp << "bge " << curMips->rs_reg << " " << curMips->rt_reg << " " << curMips->rd_reg << endl;
			break;
		}
		case (bgt):
		{
			outfp << "bgt " << curMips->rs_reg << " " << curMips->rt_reg << " " << curMips->rd_reg << endl;
			break;
		}
		case(j):
		{
			outfp << "j " << curMips->rd_reg << endl;
			break;
		}
		case(jal):
		{
			outfp << "jal " << curMips->rd_reg << endl;
			break;
		}
		case(jr):
		{
			outfp << "jr " << curMips->rd_reg << endl;
			break;
		}
		case(lab):
		{
			outfp << curMips->rd_reg << endl;
			break;
		}
		case(syscall):
		{
			outfp << "syscall" << endl;
			break;
		}
		default:
			break;
		}

		curMips = curMips->next;
	}

	outfp << "addi $fp $fp " << end_golbal_address << endl;
	outfp << "jal main" << endl;
}
