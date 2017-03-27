
/* 
 * Designer:   Li-GuangCheng, 16098537-ii20-0016, sky9475@live.com 
*/

#include "minicpu.h"

/* ALU */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	if(ALUControl == 0) 
		*ALUresult = A + B;//add
	
	
	// The rest 7 R-format Instructions
	else if(ALUControl == 1) 
		*ALUresult = A - B;//subtraction
	
	else if(ALUControl == 2) {//(slt, slti)set less than
		if(A < B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
	}
	
	else if(ALUControl == 3) {//(sltu, sltui)set less than unsigned
		if((int)A < (int)B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
	}
	
	else if(ALUControl == 4) //and
		*ALUresult = A & B;
	
	else if(ALUControl == 5) //or
		*ALUresult = A | B;
	
	else if(ALUControl == 6) //Shift B by 16 bits
		*ALUresult = B << 16;
	
		
	if(*ALUresult == 0)//Assign 0 to 1 if the result is 0; otherwise, assign 0.
		*Zero = 1;
	else
		*Zero = 0;
	
}

/* instruction fetch */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	
	if(PC % 4 == 0 && PC <= 65536) {//Return 1 if an invalid instruction is encountered; otherwise, return 0.
		*instruction = Mem[PC >> 2];
		return 0;
	} else {
		return 1;
	}
	
}


/* instruction partition */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	*op = instruction >> 26; //26~31
	*r1 = instruction >> 21 & 0x1F;//21~25
	*r2 = instruction >> 16 & 0x1F;//16~20
	*r3 = instruction >> 11 & 0x1F;//11~15
	*funct = instruction & 0x3F;//0~5?
	*offset = instruction & 0xFFFF;//0~15?
	*jsec = instruction & 0x3FFFFFF;//shamt?

}



/* instruction decode */
int instruction_decode(unsigned op,struct_controls *controls)
{
	if(op==0x0){//R-format
		controls->RegWrite = 1;
		controls->RegDst = 1;
		controls->ALUOp = 7;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		return 0;
	}
	// for jump (000010).
	else if(op == 2){
		controls->RegDst = 2;
		controls->Jump = 1;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->RegWrite = 0;
		return 0;
	}
	// for beq (000100).
	else if(op == 4){
		controls->RegDst = 2;
		controls->ALUOp = 1;
		controls->Branch = 1;
		controls->Jump = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 0;
		return 0;
	}
	// for addi (001000).
	else if(op == 8){
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		return 0;
	}
	// for slti (001010).
	else if(op == 10){
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		controls->ALUOp = 2;
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		return 0;
	}
	// for sltui (001011).
	else if(op == 11){
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		controls->ALUOp = 3;
		controls->Branch = 2;
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		return 0;
	}
	// for lui (001111).
	else if(op == 15){
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		controls->ALUOp = 6;
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;	
		controls->MemWrite = 0;
		return 0;
	}
	// for lw (100011).
	else if(op == 35){
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		controls->MemRead = 1;
		controls->MemtoReg = 1;
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->ALUOp = 0;		
		controls->MemWrite = 0;
		return 0;
	}
	// for sw (101011).0x2B
	else if(op == 43){
		controls->RegDst = 2;
		controls->MemWrite = 1;
		controls->ALUSrc = 1;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;		
		controls->RegWrite = 0;
		return 0;
	} else return 1;	//invalid instruction
	return 0;
}

/* Read Register */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	if(offset >> 15 == 1){
		// Fill upper 16 with 1s
		*extended_value = offset | 0xFFFF0000;
	}
	else{
		// Upper 16 is already 0
		*extended_value = offset;
	}
}

/* ALU operations */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	if(ALUSrc == 0){// R-type
		switch(ALUOp){
			case 7:
				// funct = 0x20 = 32
				if(funct == 32)	ALU(data1,data2,0,ALUresult,Zero);//add
				else if (funct == 34){//0x22
					ALU(data1, data2, 1, ALUresult, Zero);//subtraction
				}
				else if (funct == 42){//0x2a
					ALU(data1, data2, 2, ALUresult, Zero);//set less than
				}
				else if (funct == 43){//0x2b
					ALU(data1, data2, 3, ALUresult, Zero);//set less than unsigned
				}
				else if (funct == 36){//0x24
					ALU(data1, data2, 4, ALUresult, Zero);//and
				}
				else if (funct == 37){//0x25
					ALU(data1, data2, 5, ALUresult, Zero);//or
				}
				else return 1;	//invalid funct
				break;
			default:
				return 1;		//invalid ALUop
		}
	}
	else if (ALUSrc == 1) {//i-type and branching
		if(ALUOp == 0) {//addi, sw, lw
			ALU(data1, extended_value, ALUOp, ALUresult, Zero); 
			return 0;
		} 
		else if(ALUOp == 1) {//beq
			ALU(data1, extended_value, ALUOp, ALUresult, Zero); 
			return 0;
		}
		else if(ALUOp == 2) {//slti
			ALU(data1, extended_value, ALUOp, ALUresult, Zero); 
			return 0;
		}
		else if(ALUOp == 3) {//sltiu
			ALU(data1, extended_value, ALUOp, ALUresult, Zero); 
			return 0;
		}
		else if(ALUOp == 6) {//lui
			ALU(data1, extended_value, ALUOp, ALUresult, Zero); 
			return 0;
		}
		else
			return 1;
	}
	else
		return 1;
	return 0;
}

/* Read / Write Memory */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	if ((ALUresult % 4) != 0 && (MemWrite == 1 || MemRead == 1))
		return 1;
	if (ALUresult > 65536 && (MemWrite == 1 || MemRead == 1))
		return 1;
	
	ALUresult = ALUresult >> 2;
	if(MemRead == 1) {
		*memdata = Mem[ALUresult];	
	}
	// Save instruction
	else if(MemWrite == 1){
		Mem[ALUresult] = data2;
	}
	return 0;
}

 
/* Write Register */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if(RegWrite == 1){
		if (MemtoReg == 0) {
			if (RegDst == 0)
				Reg[r2] = ALUresult;
			else if (RegDst == 1)
				Reg[r3] = ALUresult;
		}
		else if (MemtoReg == 1) {
			if (RegDst == 0)
				Reg[r2] = memdata;
			else if (RegDst == 1)
				Reg[r3] = memdata;
		}
	}
		
	
}

/* PC update */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	if(Branch == 1 && Zero == 1){
		// PC + 4 + offset.
		*PC = *PC + 4 + (extended_value << 2);
	}
	else if(Jump == 1){// Jump
			// PC(31 - 28) and jsec shifted left 2
			*PC = (*PC & 0xF0000000) + (jsec << 2); 
	}
	// Increment by 4
	else{
		*PC += 4;
	}
	
}
