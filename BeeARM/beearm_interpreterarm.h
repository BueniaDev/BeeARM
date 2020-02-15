#ifndef BEEARM_INTERP_ARM
#define BEEARM_INTERP_ARM

#include <iostream>
#include "beearm.h"
#include "beearm_interpreterdefines.h"
using namespace beearm;
using namespace std;

namespace beearm
{
    inline void arm3(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;

	int opcode = ((instr >> 4) & 0xF);
	int reg = (instr & 0xF);
	uint32_t setreg = arm->getreg(reg);
	
	if (opcode == 1)
	{
	    arm->clock(arm->getreg(15), CODE_N32);
	    arm->setreg(15, (arm->getreg(reg) & ~1));
	    arm->setthumbmode(TestBit(setreg, 0));
	    arm->flushpipeline();
	    arm->clock(arm->getreg(15), CODE_S32);
	    arm->clock((arm->getreg(15) + 4), CODE_S32);
	}
	else
	{
	    cout << "ARMv5 exclusive opcode" << endl;
	    exit(1);
	}
    }

    inline void arm4(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;

	int offs = (instr & 0xFFFFFF);
	offs <<= 2;

	if (TestBit(instr, 24))
	{
	    arm->setreg(14, (arm->getreg(15) - 8));
	    arm->setreg(15, (arm->getreg(15) - 4 + offs));
	    arm->flushpipeline();
	}
	else
	{
	    arm->setreg(15, (arm->getreg(15) - 4 + offs));
	    arm->flushpipeline();
	}

	arm->clock(arm->getreg(15), CODE_N32);
	arm->clock(arm->getreg(15), CODE_S32);
	arm->clock((arm->getreg(15) + 4), CODE_S32);
    }

    inline void arm5(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;

	bool useimm = TestBit(instr, 25);
	bool setcond = TestBit(instr, 20);
	bool useregimm = TestBit(instr, 4);

	int opcode = ((instr >> 21) & 0xF);
	int dest = ((instr >> 12) & 0xF);
	int src = ((instr >> 16) & 0xF);

	uint32_t srcreg = arm->getreg(src);

	if (src == 15)
	{
	    srcreg -= 4;
	}

	uint32_t offs = 0;
	uint32_t destval = 0;
	bool setdest = true;

	if (useimm)
	{
	    int immoffs = (instr & 0xFF);
	    int immshift = ((instr >> 8) & 0xF);
	    immshift <<= 1;

	    offs = RORBASE(immoffs, immshift);
	}
	else
	{
	    int shifttype = ((instr >> 5) & 0x3);
	    if (useregimm)
	    {
		cout << "Shift register used" << endl;
		exit(1);
	    }
	    else
	    {
		int shiftoffs = ((instr >> 7) & 0x1F);
		int oper = (instr & 0xF);
		uint32_t operreg = arm->getreg(oper);

		if (oper == 15)
		{
		    operreg -= 4;
		}

		if (shifttype != 0)
		{
		    cout << "Other shift" << endl;
		    exit(1);
		}
		else
		{
		    if (shiftoffs != 0)
		    {
			cout << "Non-zero shift offset" << endl;
			exit(1);
		    }
		    else
		    {
			offs = operreg;
		    }
		}
	    }
	}

	if (dest == 15)
	{
	    arm->clock(arm->getreg(15), CODE_N32);

	    if (setcond)
	    {
		cout << "Change CPSR to SPSR" << endl;
		exit(1);
	    }
	}

	switch (opcode)
	{
	    case 0x0:
	    {
		destval = (srcreg & offs);
	    }
	    break;
	    case 0x1:
	    {
		destval = (srcreg ^ offs);
	    }
	    break;
	    case 0x2:
	    {
		destval = (srcreg - offs);
	    }
	    break;
	    case 0x3:
	    {
		destval = (offs - srcreg);
	    }
	    break;
	    case 0x4:
	    {
		destval = (srcreg + offs);
	    }
	    break;
	    case 0xC:
	    {
		destval = (srcreg | offs);
	    }
	    break;
	    case 0xD:
	    {
		destval = offs;
	    }
	    break;
	    case 0xE:
	    {
		destval = (srcreg & ~offs);
	    }
	    break;
	    case 0xF:
	    {
		destval = (~srcreg);
	    }
	    break;
	    default: cout << "Unrecognized ALU instruction" << endl; exit(1); break;
	}

	if (setdest)
	{
	    arm->setreg(dest, destval);
	}

	if (dest == 15)
	{
	    if (TestBit(arm->getreg(15), 0))
	    {
		arm->setthumbmode(true);
		arm->setreg(15, (BitReset(arm->getreg(15), 0)));
	    }
	    else
	    {
		arm->setreg(15, (arm->getreg(15) & ~3));
	    }

	    arm->flushpipeline();

	    arm->clock(arm->getreg(15), CODE_S32);
	}

	arm->clock((arm->getreg(15) + 4), CODE_S32);
    }

    inline void arm6(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	
	bool useimm = TestBit(instr, 25);
	bool isspsr = TestBit(instr, 22);
	bool ismsr = TestBit(instr, 21);

	uint32_t mask = 0;
	uint32_t offs = 0;

	if (TestBit(instr, 19))
	{
	    mask |= 0xFF000000;
	}

	if (TestBit(instr, 16))
	{
	    mask |= 0xFF;
	}

	if (ismsr)
	{
	    if (isspsr)
	    {
		cout << "MSR SPSR" << endl;
		exit(1);
	    }
	    else
	    {
		if (useimm)
		{
		    int immoffs = (instr & 0xFF);
		    int immshift = ((instr >> 8) & 0xF);

		    offs = RORBASE(immoffs, immshift);
		}
		else
		{
		    int regoffs = (instr & 0xF);

		    if (regoffs == 15)
		    {
			cout << "Used R15 for MSR" << endl;
			exit(1);
		    }
		    else
		    {
			offs = arm->getreg(regoffs);
		    }
		}

		uint32_t temp = arm->getcpsr();
		temp &= ~mask;
		temp |= offs;

		arm->setcpsr(temp);
	    }
	}
	else
	{
	    if (isspsr)
	    {
		cout << "MRS SPSR" << endl;
		exit(1);
	    }
	    else
	    {
		uint32_t temp = arm->getcpsr();
	    }
	}

	arm->clock((arm->getreg(15) + 4), CODE_S32);
    }

    inline void arm7(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	cout << "ARM.7" << endl;
	cout << hex << (int)(instr) << endl;
	exit(1);
    }

    inline void arm8(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	cout << "ARM.8" << endl;
	cout << hex << (int)(instr) << endl;
	exit(1);
    }

    inline void arm9(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	
	if ((instr >> 28) == 0xF)
	{
	    cout << "PLD" << endl;
	    exit(1);
	}

	bool ldrorstr = TestBit(instr, 20);
	bool usereg = TestBit(instr, 25);
	bool preorpost = TestBit(instr, 24);
	bool upordown = TestBit(instr, 23);
	bool byteorword = TestBit(instr, 22);
	bool iswriteback = TestBit(instr, 21);

	int dst = ((instr >> 12) & 0xF);
	int src = ((instr >> 16) & 0xF);

	uint32_t srcreg = arm->getreg(src);
	uint32_t dstreg = arm->getreg(dst);

	if (src == 15)
	{
	    srcreg -= 4;
	}

	uint32_t addr = srcreg;

	int immoffs = (instr & 0xFFF);
	int offs = 0;

	arm->clock(arm->getreg(15), CODE_N32);

	if (ldrorstr)
	{
	    if (usereg)
	    {
		cout << "Shifted register" << endl;
		exit(1);
	    }
	    else
	    {
		offs = immoffs;
		if (preorpost)
		{
		    addr = (upordown) ? (addr + offs) : (addr - offs);
		}


		if (byteorword)
		{
		    arm->setreg(dst, arm->readByte(addr));
		    arm->clock();
		}
		else
		{
		    if ((addr & 0x3) != 0)
		    {
			cout << "Misaligned LDR address" << endl;
			exit(1);
		    }

		    arm->setreg(dst, arm->readLong(addr));
		    arm->clock();
		}

		arm->clock((arm->getreg(15) + 4), (byteorword) ? DATA_N16 : DATA_N32);

		if (!preorpost)
		{
		    addr = (upordown) ? (addr + offs) : (addr - offs);
		}

		if (!preorpost && (src != dst))
		{
		    arm->setreg(src, addr);
		}
		else if (preorpost && iswriteback && (src != dst))
		{
		    arm->setreg(src, addr);
		}

		if (dst != 15)
		{
		    arm->clock(arm->getreg(15), CODE_S32);
		}
		else
		{
		    cout << "R15 used as register" << endl;
		    exit(1);
		}
	    }
	}
	else
	{
	    if (usereg)
	    {
		cout << "Shifted register" << endl;
		exit(1);
	    }
	    else
	    {
		offs = immoffs;
		if (preorpost)
		{
		    addr = (upordown) ? (addr + offs) : (addr - offs);
		}


		if (byteorword)
		{
		    arm->writeByte(addr, (dstreg & 0xFF));
		    arm->clock(addr, DATA_N16);
		}
		else
		{
		    arm->writeLong((addr & ~3), dstreg);
		    arm->clock(addr, DATA_N32);
		}

		if (!preorpost)
		{
		    addr = (upordown) ? (addr + offs) : (addr - offs);
		}

		if (!preorpost && (src != dst))
		{
		    arm->setreg(src, addr);
		}
		else if (preorpost && iswriteback && (src != dst))
		{
		    arm->setreg(src, addr);
		}
	    }
	}
    }

    inline void arm10(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	cout << "ARM.10" << endl;
	
	bool prepost = TestBit(instr, 24);
	bool updown = TestBit(instr, 23);
	bool offsisreg = TestBit(instr, 22);
	bool writeback = TestBit(instr, 21);
	bool loadstore = TestBit(instr, 20);

	int base = ((instr >> 16) & 0xF);
	int dst = ((instr >> 12) & 0xF);
	int opcode = ((instr >> 5) & 0x3);
	int offs = (instr & 0xF);
	int offsup = ((instr >> 8) & 0xF);

	if (!prepost)
	{
	    writeback = true;
	}

	uint32_t baseoffs = 0;
	uint32_t basereg = arm->getreg(base);
	uint32_t addr = 0;

	uint32_t destreg = arm->getreg(dst);

	if (!offsisreg)
	{
	    baseoffs = arm->getreg(offs);

	    if (offs == 15)
	    {
		cout << "Warning - ARM.10 offset register is PC" << endl;
	    }
	}
	else
	{
	    baseoffs = ((offsup << 4) | offs);
	}

	if (prepost)
	{
	    if (updown)
	    {
		addr = (basereg + baseoffs);
	    }
	    else
	    {
		addr = (basereg - baseoffs);
	    }
	}

	switch (opcode)
	{
	    case 1:
	    {
		if (!loadstore)
		{
		    if (dst == 15)
		    {
			destreg += 4;
		    }

		    arm->writeWord(addr, (destreg & 0xFFFF));

		    arm->clock(arm->getreg(15), CODE_N16);
		    arm->clock(addr, DATA_N16);
		}
		else
		{
		    if (dst == 15)
		    {
			arm->clock(arm->getreg(15), CODE_S16);
			arm->clock((arm->getreg(15) + 2), CODE_N16);
		    }

		    arm->clock(arm->getreg(15), CODE_S16);

		    arm->setreg(dst, arm->readWord(addr));

		    arm->clock(addr, DATA_N16);
		    arm->clock();
		}
	    }
	    break;
	    case 0x2:
	    {
		if (dst == 15)
		{
		    arm->clock(arm->getreg(15), CODE_S32);
		    arm->clock((arm->getreg(15) + 2), CODE_N32);
		}

		arm->clock(arm->getreg(15), CODE_S32);

		uint32_t value = arm->readByte(addr);

		if (TestBit(value, 7))
		{
		    value |= 0xFFFFFF00;
		}

		arm->setreg(dst, value);

		arm->clock(addr, DATA_N32);
		arm->clock();
	    }
	    break;
	    case 0x3:
	    {
		if (dst == 15)
		{
		    arm->clock(arm->getreg(15), CODE_S16);
		    arm->clock((arm->getreg(15) + 2), CODE_N16);
		}

		arm->clock(arm->getreg(15), CODE_S16);

		uint32_t value = arm->readWord(addr);

		if (TestBit(value, 15))
		{
		    value |= 0xFFFF0000;
		}

		arm->setreg(dst, value);

		arm->clock(addr, DATA_N16);
		arm->clock();
	    }
	    break;
	    default: cout << "ARM.12 Swap" << endl; exit(1); break;
	}

	if (!prepost)
	{
	    if (updown)
	    {
		addr = (basereg + baseoffs);
	    }
	    else
	    {
		addr = (basereg - baseoffs);
	    }
	}

	if (writeback && (base != dst))
	{
	    arm->setreg(base, addr);
	}
    }

    inline void arm11(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	cout << "ARM.11" << endl;
	cout << hex << (int)(instr) << endl;
	exit(1);
    }

    inline void arm12(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	cout << "ARM.12" << endl;
	cout << hex << (int)(instr) << endl;
	exit(1);
    }

    inline void arm13(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	uint32_t comment = (instr & 0xFFFFFF);
	comment >>= 16;
	arm->softwareinterrupt(comment);	
    }

    inline void arm14(BeeARM *arm)
    {
	cout << "ARM.14-CDP" << endl;
	exit(1);
    }

    inline void arm15(BeeARM *arm)
    {
	cout << "ARM.15-LDC/STC" << endl;
	exit(1);
    }

    inline void arm16(BeeARM *arm)
    {
	cout << "ARM.16-MRC/MCR" << endl;
	exit(1);
    }

    inline void arm17(BeeARM *arm)
    {
	cout << "ARM.17-Undefined Instruction" << endl;
	exit(1);
    }
};

#endif // BEEARM_INTERP_ARM
