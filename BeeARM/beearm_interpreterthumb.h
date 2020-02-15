#ifndef BEEARM_INTERP_THUMB
#define BEEARM_INTERP_THUMB

#include <iostream>
#include "beearm.h"
#include "beearm_interpreterdefines.h"
using namespace beearm;

namespace beearm
{
    inline void thumb1(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int opcode = ((instr >> 11) & 0x3);

	int dest = (instr & 0x7);
	int src = ((instr >> 3) & 0x7);
	int offs = ((instr >> 6) & 0x1F);

	uint32_t srcreg = arm->getreg(src);

	bool carryout = false;

	switch (opcode)
	{
	    case 0:
	    {
		LSLS(srcreg, offs, carryout);
		LSL(srcreg, offs);

		arm->setnzc(TestBit(srcreg, 31), (srcreg == 0), carryout);

		arm->setreg(dest, srcreg);
	    }
	    break;
	    case 1:
	    {
		LSRS(srcreg, offs, carryout);
		LSR(srcreg, offs);

		arm->setnzc(TestBit(srcreg, 31), (srcreg == 0), carryout);

		arm->setreg(dest, srcreg);
	    }
	    break;
	    case 2:
	    {
		ASRS(srcreg, offs, carryout);
		ASR(srcreg, offs);

		arm->setnzc(TestBit(srcreg, 31), (srcreg == 0), carryout);

		arm->setreg(dest, srcreg);
	    }
	    break;
	    case 3: cout << "THUMB.2" << endl; break; // Shouldn't happen
	    default: cout << "Unrecognized opcode of " << hex << (int)(opcode) << endl; exit(1); break;
	}

	arm->clock(arm->getreg(15), CODE_S16);
    }

    inline void thumb2(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int opcode = ((instr >> 9) & 0x3);
	int src = ((instr >> 3) & 0x7);
	int dst = (instr & 0x7);
	int oper = ((instr >> 6) & 0x7);

	uint32_t srcreg = arm->getreg(src);
	uint32_t operreg = arm->getreg(oper);

	switch (opcode)
	{
	    case 0:
	    {
		uint32_t temp = (srcreg + operreg);
		arm->setreg(dst, temp);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_ADD(srcreg, operreg), OVERFLOW_ADD(srcreg, operreg, temp));
	    }
	    break;
	    case 1:
	    {
		uint32_t temp = (srcreg - operreg);
		arm->setreg(dst, temp);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(srcreg, operreg), OVERFLOW_SUB(srcreg, operreg, temp));
	    }
	    break;
	    case 2:
	    {
		uint32_t temp = (srcreg + oper);
		arm->setreg(dst, temp);
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_ADD(srcreg, oper), OVERFLOW_ADD(srcreg, oper, temp));
	    }
	    break;
	    case 3:
	    {
		uint32_t temp = (srcreg - oper);
		arm->setreg(dst, temp);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(srcreg, oper), OVERFLOW_SUB(srcreg, oper, temp));
	    }
	    break;
	}

	arm->clock(arm->getreg(15), CODE_S16);
    }

    inline void thumb3(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;

	int opcode = ((instr >> 11) & 0x3);
	int dst = ((instr >> 8) & 0x7);
	uint32_t imm = (instr & 0xFF);

	uint32_t dstreg = arm->getreg(dst);

	switch (opcode)
	{
	    case 0:
	    {
		arm->setreg(dst, imm);
		arm->setnz(TestBit(imm, 31), (imm == 0));
	    }
	    break;
	    case 1:
	    {
		uint32_t temp = (dstreg - imm);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(dstreg, imm), OVERFLOW_SUB(dstreg, imm, temp));
	    }
	    break;
	    case 2:
	    {
		uint32_t temp = (dstreg + imm);
		arm->setreg(dst, temp);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_ADD(dstreg, imm), OVERFLOW_ADD(dstreg, imm, temp));
	    }
	    break;
	    case 3:
	    {
		uint32_t temp = (dstreg - imm);
		arm->setreg(dst, temp);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(dstreg, imm), OVERFLOW_SUB(dstreg, imm, temp));
	    }
	    break;
	    default: cout << "Unrecognized operand of " << hex << (int)(opcode) << endl; exit(1); break;
	}

	arm->clock(arm->getreg(15), CODE_S16);
    }

    inline void thumb4(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int dst = (instr & 0x7);
	int src = ((instr >> 3) & 0x7);

	int opcode = ((instr >> 6) & 0xF);

	uint32_t dstreg = arm->getreg(dst);
	uint32_t srcreg = arm->getreg(src);

	uint32_t temp = 0;

	switch (opcode)
	{
	    case 0x0:
	    {
		temp = (dstreg & srcreg);
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setreg(dst, temp);

		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0x1:
	    {
		temp = (dstreg ^ srcreg);
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setreg(dst, temp);

		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0x2:
	    {
		uint32_t input = dstreg;
		uint32_t operand = (srcreg & 0xFF);

		LSL(input, operand);
		temp = input;

		bool carry = false;

		LSLS(input, operand, carry);

		arm->setnzc(TestBit(temp, 31), (temp == 0), carry);

		arm->clock();
		arm->setreg(dst, temp);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 0x3:
	    {
		uint32_t input = dstreg;
		uint32_t operand = (srcreg & 0xFF);

		LSR(input, operand);
		temp = input;

		bool carry = false;

		LSRS(input, operand, carry);

		arm->setnzc(TestBit(temp, 31), (temp == 0), carry);

		arm->clock();
		arm->setreg(dst, temp);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 0x4:
	    {
		uint32_t input = dstreg;
		uint32_t operand = (srcreg & 0xFF);

		ASR(input, operand);
		temp = input;

		bool carry = false;

		ASRS(input, operand, carry);

		arm->setnzc(TestBit(temp, 31), (temp == 0), carry);

		arm->clock();
		arm->setreg(dst, temp);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 0x5:
	    {
		uint32_t input = dstreg;
		uint32_t operand = (srcreg & 0xFF);

		int carry = (arm->getc() ? 1 : 0);
		
		temp = (input + operand + carry);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_ADD(input, operand), OVERFLOW_ADD(input, operand, temp));

		arm->setreg(dst, temp);
		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0x6:
	    {
		uint32_t input = dstreg;
		uint32_t operand = (srcreg & 0xFF);

		int carry = (arm->getc() ? 0 : 1);
		
		temp = (input - operand - carry);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(input, operand), OVERFLOW_SUB(input, operand, temp));

		arm->setreg(dst, temp);
		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0x7:
	    {
		uint32_t input = dstreg;
		uint32_t operand = (srcreg & 0xFF);

		ROR(input, operand);
		temp = input;

		bool carry = false;

		RORS(input, operand, carry);

		arm->setnzc(TestBit(temp, 31), (temp == 0), carry);

		arm->clock();
		arm->setreg(dst, temp);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 0x8:
	    {
		temp = (dstreg & srcreg);
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0x9:
	    {
		temp = (0 - srcreg);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(0, srcreg), OVERFLOW_SUB(0, srcreg, temp));
		arm->setreg(dst, temp);

		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0xA:
	    {
		temp = (dstreg - srcreg);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(dstreg, srcreg), OVERFLOW_SUB(dstreg, srcreg, temp));
		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0xB:
	    {
		temp = (dstreg + srcreg);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_ADD(dstreg, srcreg), OVERFLOW_ADD(dstreg, srcreg, temp));
		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0xC:
	    {
		temp = (dstreg | srcreg);
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setreg(dst, temp);

		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0xD:
	    {
		temp = (dstreg * srcreg);

		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setreg(dst, temp);

		int cycletemp = 0;

		for (int i = 0; i < 4; i++)
		{
		    cycletemp += BitGetVal(temp, ((i << 3) + 7)); // Bits 7, 15, 23 and 31
		}

		arm->clock(arm->getreg(15), CODE_S16);

		for (int i = 0; i < cycletemp; i++)
		{
		    arm->clock();
		}
	    }
	    break;
	    case 0xE:
	    {
		temp = (dstreg & ~srcreg);
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setreg(dst, temp);

		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 0xF:
	    {
		temp = ~dstreg;
		arm->setnz(TestBit(temp, 31), (temp == 0));
		arm->setreg(dst, temp);

		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    default: cout << "Unrecognized THUMB ALU instruction of " << hex << (int)(opcode) << endl; arm->printregs(); exit(1); break;
	}
    }

    inline void thumb5(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int dst = (instr & 0x7);
	int src = ((instr >> 3) & 0x7);
	
	bool srcmsb = TestBit(instr, 6);
	bool dstmsb = TestBit(instr, 7);

	if (srcmsb)
	{
	    src |= 0x8;
	}
	
	if (dstmsb)
	{
	    dst |= 0x8;
	}

	uint8_t opcode = ((instr >> 8) & 0x3);

	uint32_t dstreg = arm->getreg(dst);
	uint32_t srcreg = arm->getreg(src);

	uint32_t temp = 0;

	if ((opcode == 3) && (dstmsb))
	{
	    cout << "Error - R15 used for THUMB BX" << endl;
	    exit(1);
	}

	switch (opcode)
	{
	    case 0:
	    {
		if (dst == 15)
		{
		    srcreg &= ~1;
		}

		if (dst != 15)
		{
		    uint32_t result = (dstreg + srcreg);
		    arm->setreg(dst, result);
		    arm->clock(arm->getreg(15), CODE_S16);
		}
		else
		{
		    arm->clock(arm->getreg(15), CODE_N16);
		    uint32_t result = (dstreg + srcreg);
		    arm->setreg(dst, result);
		    arm->flushpipeline();
		    arm->clock(arm->getreg(15), CODE_S16);
		    arm->clock((arm->getreg(15) + 2), CODE_S16);
		}
	    }
	    break;
	    case 1:
	    {
		uint32_t temp = (dstreg - srcreg);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(dstreg, srcreg), OVERFLOW_SUB(dstreg, srcreg, temp));
		arm->clock(arm->getreg(15), CODE_S16);
	    }
	    break;
	    case 2:
	    {
		if (dst == 15)
		{
		    srcreg &= ~1;
		}

		if (dst != 15)
		{
		    arm->setreg(dst, srcreg);
		    arm->clock(arm->getreg(15), CODE_S16);
		}
		else
		{
		    arm->clock(arm->getreg(15), CODE_N16);

		    arm->setreg(dst, srcreg);
		    arm->flushpipeline();

		    arm->clock(arm->getreg(15), CODE_S16);
		    arm->clock((arm->getreg(15) + 2), CODE_S16);
		}
	    }
	    break;
	    case 3:
	    {
		if (!TestBit(srcreg, 0))
		{
		    arm->setthumbmode(false);
		    srcreg &= ~0x3;
		}
		else
		{
		    srcreg &= ~0x1;
		}

		arm->clock(arm->getreg(15), CODE_N16);

		if (src == 15)
		{
		    arm->setreg(15, (arm->getreg(15) & ~0x2));
		}
		else
		{
		    arm->setreg(15, srcreg);
		}

		arm->clock(arm->getreg(15), CODE_S16);
		arm->clock((arm->getreg(15) + 2), CODE_S16);

		arm->flushpipeline();
	    }
	    break;
	    default: cout << "Unrecognized THUMB.5 instruction of " << dec << (int)(opcode) << endl; exit(1); break;
	}
    }

    inline void thumb6(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;

	uint16_t offs = (instr & 0xFF);
	int dst = ((instr >> 8) & 0x7);

	offs <<= 2;

	uint32_t addr = (((arm->getreg(15) - 2) & ~0x2) + offs);
	arm->clock(addr, DATA_N32);

	arm->setreg(dst, arm->readLong(addr));
	arm->clock();

	arm->clock((arm->getreg(15) + 2), CODE_S16);
    }

    inline void thumb7(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int opcode = ((instr >> 10) & 0x3);
	int offs = ((instr >> 6) & 0x7);
	int src = ((instr >> 3) & 0x7);
	int dst = (instr & 0x7);

	uint32_t srcreg = arm->getreg(src);
	uint32_t offsreg = arm->getreg(offs);
	uint32_t dstreg = arm->getreg(dst);

	switch (opcode)
	{
	    case 0:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->writeLong((addr & ~3), dstreg);

		arm->clock(arm->getreg(15), CODE_N16);
		arm->clock(addr, DATA_N32);
	    }
	    break;
	    case 1:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->clock(addr, DATA_N32);

		uint32_t value = arm->readLong(addr);
		arm->clock();

		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 2:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->writeByte(addr, (dstreg & 0xFF));

		arm->clock(arm->getreg(15), CODE_N16);
		arm->clock(addr, DATA_N16);
	    }
	    break;
	    case 3:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->clock(addr, DATA_N16);

		uint8_t value = arm->readByte(addr);
		arm->clock();

		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    default: cout << "Unrecognized THUMB LDR/STR instruction of " << hex << (int)(opcode) << endl; arm->printregs(); exit(1); break;
	}

    }

    inline void thumb8(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int opcode = ((instr >> 10) & 0x3);
	int offs = ((instr >> 6) & 0x7);
	int src = ((instr >> 3) & 0x7);
	int dst = (instr & 0x7);

	uint32_t srcreg = arm->getreg(src);
	uint32_t offsreg = arm->getreg(offs);
	uint32_t dstreg = arm->getreg(dst);

	switch (opcode)
	{
	    case 0:
	    {
		arm->clock(arm->getreg(15), CODE_N16);
		uint32_t addr = (srcreg + offsreg);
		arm->writeWord(addr, (dstreg & 0xFFFF));
		arm->clock(addr, DATA_N16);
	    }
	    break;
	    case 1:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->clock(addr, DATA_N16);

		uint32_t value = arm->readByte(addr);
		arm->clock();

		if (TestBit(value, 7))
		{
		    value |= 0xFFFFFF00;
		}

		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 2:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->clock(addr, DATA_N16);
		uint32_t value = arm->readLong(addr);
		arm->clock();
		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 3:
	    {
		uint32_t addr = (srcreg + offsreg);
		arm->clock(addr, DATA_N16);

		uint16_t value = arm->readWord(addr);
		arm->clock();

		if (TestBit(value, 15))
		{
		    value |= 0xFFFF0000;
		}

		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    default: cout << "Unrecognized THUMB LDR/STR instruction of " << hex << (int)(opcode) << endl; arm->printregs(); exit(1); break;
	}
    }

    inline void thumb9(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	int opcode = ((instr >> 11) & 0x3);
	int offs = ((instr >> 6) & 0x1F);
	int src = ((instr >> 3) & 0x7);
	int dst = (instr & 0x7);

	uint32_t srcreg = arm->getreg(src);
	uint32_t dstreg = arm->getreg(dst);
	
	switch (opcode)
	{
	    case 0:
	    {
		offs <<= 2;
		uint32_t addr = (srcreg + offs);
		arm->writeLong((addr & ~3), dstreg);

		arm->clock(arm->getreg(15), CODE_N16);
		arm->clock(addr, DATA_N32);
	    }
	    break;
	    case 1:
	    {
		offs <<= 2;
		uint32_t addr = (srcreg + offs);
		arm->clock(addr, DATA_N32);

		uint32_t value = arm->readLong(addr);
		arm->clock();

		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    case 2:
	    {
		uint32_t addr = (srcreg + offs);
		arm->writeByte(addr, (dstreg & 0xFF));

		arm->clock(arm->getreg(15), CODE_N16);
		arm->clock(addr, DATA_N16);
	    }
	    break;
	    case 3:
	    {
		uint32_t addr = (srcreg + offs);
		arm->clock(addr, DATA_N16);

		uint8_t value = arm->readByte(addr);
		arm->clock();

		arm->setreg(dst, value);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    break;
	    default: cout << "Unrecognized THUMB LDR/STR instruction of " << hex << (int)(opcode) << endl; arm->printregs(); exit(1); break;
	}
    }

    inline void thumb10(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	uint8_t srcdst = (instr & 0x7);
	uint8_t base = ((instr >> 3) & 0x7);
	uint16_t offs = ((instr >> 6) & 0x1F);
	bool opcode = TestBit(instr, 11);

	uint32_t addr = (arm->getreg(base) + (offs << 1));

	if (opcode)
	{
	    arm->clock(addr, DATA_N16);
	    uint16_t value = arm->readWord((addr & ~1));
	    arm->clock();
	    arm->setreg(srcdst, value);
	    arm->clock((arm->getreg(15) + 2), CODE_S16);
	}
	else
	{
	    arm->clock(arm->getreg(15), CODE_N16);
	    uint16_t value = (arm->getreg(srcdst) & 0xFFFF);
	    arm->writeWord((addr & ~1), value);
	    arm->clock(addr, DATA_N16);
	}
    }

    inline void thumb11(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	uint16_t offs = (instr & 0xFF);
	uint8_t srcdst = ((instr >> 8) & 0x7);

	bool opcode = TestBit(instr, 11);

	uint32_t addr = (arm->getreg(13) + (offs << 2));

	if (opcode)
	{
	    arm->clock(addr, DATA_N32);
	    uint32_t value = arm->readLong(addr);
	    arm->clock();
	    arm->setreg(srcdst, value);
	    arm->clock((arm->getreg(15) + 2), CODE_S16);
	}
	else
	{
	    arm->clock(arm->getreg(15), CODE_N16);
	    uint32_t value = arm->getreg(srcdst);
	    arm->writeLong((addr & ~3), value);
	    arm->clock(addr, DATA_N32);
	}
    }

    inline void thumb12(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	uint16_t offs = (instr & 0xFF);
	uint8_t dst = ((instr >> 8) & 0x7);

	bool opcode = TestBit(instr, 11);

	offs <<= 2;

	if (opcode)
	{
	    uint32_t value = (arm->getreg(13) + offs);
	    arm->setreg(dst, value);
	}
	else
	{
	    uint32_t value = (((arm->getreg(15) - 2) & ~0x2) + offs);
	    arm->setreg(dst, value);
	}
	
	arm->clock(arm->getreg(15), CODE_S16);
    }

    inline void thumb13(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	uint16_t offs = (instr & 0x7F);
	bool opcode = TestBit(instr, 7);

	offs <<= 2;

	uint32_t r13 = arm->getreg(13);

	if (opcode)
	{
	    r13 -= offs;
	}
	else
	{
	    r13 += offs;
	}

	arm->setreg(13, r13);
	arm->clock(arm->getreg(15), CODE_S16);
    }

    inline void thumb14(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;

	uint32_t r13 = arm->getreg(13);
	uint32_t r14 = arm->getreg(14);

	uint8_t reglist = (instr & 0xFF);

	bool pclrbit = TestBit(instr, 8);

	bool opcode = TestBit(instr, 11);

	uint8_t ncount = 0;

	for (int i = 0; i < 8; i++)
	{
	    if (TestBit(reglist, i))
	    {
		ncount += 1;
	    }
	}

	if (opcode)
	{
	    arm->clock(arm->getreg(15), CODE_N16);

	    for (int i = 0; i < 8; i++)
	    {
		if (TestBit(reglist, 0))
		{
		    uint32_t popvalue = arm->readLong(r13);
		    arm->setreg(i, popvalue);
		    r13 += 4;

		    if (ncount > 1)
		    {
			arm->clock(r13, DATA_S32);
		    }
		}

		reglist >>= 1;
	    }

	    if (pclrbit)
	    {
		arm->clock();
		arm->clock(r13, DATA_N32);

		uint32_t newpc = arm->readLong(r13);
		arm->setreg(15, (newpc & ~0x1));
		r13 += 4;
		arm->flushpipeline();

		arm->clock(arm->getreg(15), CODE_S16);
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    else
	    {
		arm->clock();
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	}
	else
	{
	    arm->clock(arm->getreg(15), CODE_N16);

	    if (pclrbit)
	    {
		r13 -= 4;
		arm->writeLong(r13, r14);
		arm->setreg(14, r14);

		arm->clock(arm->getreg(13), DATA_S32);
	    }

	    for (int i = 7; i >= 0; i--)
	    {
		if (TestBit(reglist, i))
		{
		    r13 -= 4;
		    uint32_t pushvalue = arm->getreg(i);
		    arm->writeLong(r13, pushvalue);

		    if ((ncount - 1) != 0)
		    {
			arm->clock(r13, DATA_S32);
			ncount -= 1;
		    }
		    else
		    {
			arm->clock(r13, DATA_N32);
			i = 10;
			break;
		    }
		}
	    }
	}

	arm->setreg(13, r13);
    }

    inline void thumb15(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	uint8_t reglist = (instr & 0xFF);
	int base = ((instr >> 8) & 0x7);
	bool opcode = TestBit(instr, 11);

	uint32_t baseaddr = arm->getreg(base);
	uint32_t regval = 0;

	uint8_t ncount = 0;

	uint32_t oldbase = baseaddr;
	uint8_t transferreg = 0xFF;
	bool writeback = true;

	for (int i = 0; i < 8; i++)
	{
	    if (TestBit(reglist, i))
	    {
		transferreg = i;
		i = 0xFF;
		break;
	    }
	}

	for (int i = 0; i < 8; i++)
	{
	    if (TestBit(reglist, i))
	    {
		ncount += 1;
	    }
	}

	if (opcode)
	{
	    if (reglist != 0)
	    {
		arm->clock(arm->getreg(15), CODE_N16);

		for (int i = 0; i < 8; i++)
		{
		    if (TestBit(reglist, 0))
		    {
			if ((i == transferreg) && (base == transferreg))
			{
			    writeback = false;
			}

			if ((baseaddr & 0x3) != 0)
			{
			    cout << "Misaligned THUMB LDMIA address" << endl;
			    exit(1);
			}

			uint32_t value = arm->readLong(baseaddr);
			arm->setreg(i, value);

			baseaddr += 4;

			if (writeback)
			{
			    arm->setreg(base, baseaddr);
			}

			if (ncount > 1)
			{
			    arm->clock(baseaddr, DATA_S32);
			}
		    }

		    reglist >>= 1;
		}

		arm->clock();
		arm->clock((arm->getreg(15) + 2), CODE_S16);
	    }
	    else
	    {
		arm->setreg(15, arm->readLong(baseaddr));
		baseaddr += 0x40;
		arm->setreg(base, baseaddr);
	    }
	}
	else
	{
	    if (reglist != 0)
	    {
		arm->clock(arm->getreg(15), CODE_N16);

		for (int i = 0; i < 8; i++)
		{
		    if (TestBit(reglist, 0))
		    {
			regval = arm->getreg(i);

			if ((i == transferreg) && (base == transferreg))
			{
			    arm->writeLong((baseaddr & ~3), oldbase);
			}
			else
			{
			    arm->writeLong((baseaddr & ~3), regval);
			}

			baseaddr += 4;
			arm->setreg(base, baseaddr);

			if ((ncount - 1) != 0)
			{
			    arm->clock(baseaddr, DATA_S32);
			    ncount -= 1;
			}
			else
			{
			    arm->clock(baseaddr, DATA_N32);
			    i = 10;
			    break;
			}
		    }

		    reglist >>= 1;
		}
	    }
	    else
	    {
		arm->writeLong(baseaddr, arm->getreg(15));
		baseaddr += 0x40;
		arm->setreg(base, baseaddr);
	    }
	}
    }

    inline void thumb16(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;

	uint32_t offs = (instr & 0xFF);
	offs <<= 1;

	if (TestBit(offs, 8))
	{
	    offs |= 0xFFFFFE00;
	}

	int opcode = ((instr >> 8) & 0xF);

	if (arm->getcond(opcode))
	{
	    arm->clock(arm->getreg(15), CODE_N16);
	    arm->setreg(15, ((arm->getreg(15) - 2) + offs));
	    arm->clock(arm->getreg(15), CODE_S16);
	    arm->clock((arm->getreg(15) + 2), CODE_S16);
	    arm->flushpipeline();
	}
	else
	{
	    arm->clock(arm->getreg(15), CODE_S16);
	}
    }

    inline void thumb17(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	cout << "THUMB.17" << endl;
	uint8_t comment = (instr & 0xFF);
	arm->softwareinterrupt(comment);
    }

    inline void thumb18(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;
	
	uint32_t offs = (instr & 0x7FF);
	offs <<= 1;

	if (TestBit(offs, 11))
	{
	    offs |= 0xFFFFF000;
	}

	arm->clock(arm->getreg(15), CODE_N16);
	arm->setreg(15, ((arm->getreg(15) - 2) + offs));
	arm->clock(arm->getreg(15), CODE_S16);
	arm->clock((arm->getreg(15) + 2), CODE_S16);
	arm->flushpipeline();
    }

    inline void thumb19(BeeARM *arm)
    {
	uint16_t instr = arm->currentthumbinstr.thumbvalue;

	bool firstop = (((instr >> 11) & 0x1F) == 0x1F) ? false : true;

	uint32_t lbladdr = 0;

	if (firstop)
	{
	    lbladdr = ((instr & 0x7FF) << 12);

	    if (TestBit(lbladdr, 22))
	    {
		lbladdr |= 0xFF800000;
	    }

	    lbladdr += (arm->getreg(15) - 2);

	    arm->setreg(14, lbladdr);
	    arm->clock(arm->getreg(15), CODE_S16);
	}
	else
	{
	    uint32_t nextinstraddr = (arm->getreg(15) - 2);
	    nextinstraddr |= 1;

	    lbladdr = arm->getreg(14);

	    lbladdr += ((instr & 0x7FF) << 1);

	    arm->clock(arm->getreg(15), CODE_N16);

	    arm->setreg(15, (lbladdr & ~1));

	    arm->setreg(14, (nextinstraddr - 2));

	    arm->clock(arm->getreg(15), CODE_S16);
	    arm->clock((arm->getreg(15) + 2), CODE_S16);

	    arm->flushpipeline();	
	}
    }
};


#endif // BEEARM_INTERP_THUMB
