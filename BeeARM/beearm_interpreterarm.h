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

	if (TestBit(offs, 25))
	{
	    offs |= 0xFC000000;
	}

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
	uint32_t destval = arm->getreg(dest);
	bool setdest = true;

	bool carryout = false;

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

		if (shiftoffs == 0)
		{
		    offs = operreg;
		}
		else
		{
		    switch (shifttype)
		    {
			case 0:
			{
			    uint32_t temp = operreg;
			    LSLS(temp, shiftoffs, carryout);
			    LSL(temp, shiftoffs);
			    offs = temp;
			}
			break;
			case 1:
			{
			    uint32_t temp = operreg;
			    LSRS(temp, shiftoffs, carryout);
			    LSR(temp, shiftoffs);
			    offs = temp;
			}
			break;
			default: cout << "Unrecognized shift of " << hex << (int)(shifttype) << endl; exit(1); break;
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

		if (setcond)
		{
		    arm->setnz(TestBit(destval, 31), (destval == 0));
		}
	    }
	    break;
	    case 0x1:
	    {
		destval = (srcreg ^ offs);

		if (setcond)
		{
		    arm->setnz(TestBit(destval, 31), (destval == 0));
		}
	    }
	    break;
	    case 0x2:
	    {
		destval = (srcreg - offs);

		if (setcond)
		{
		    arm->setnzcv(TestBit(destval, 31), (destval == 0), CARRY_SUB(srcreg, offs), OVERFLOW_SUB(srcreg, offs, destval));
		}
	    }
	    break;
	    case 0x3:
	    {
		destval = (offs - srcreg);

		if (setcond)
		{
		    arm->setnzcv(TestBit(destval, 31), (destval == 0), CARRY_SUB(offs, srcreg), OVERFLOW_SUB(offs, srcreg, destval));
		}
	    }
	    break;
	    case 0x4:
	    {
		destval = (srcreg + offs);
	    }
	    break;
	    case 0x8:
	    {
		uint32_t temp = (srcreg & offs);
		arm->setnz(TestBit(temp, 31), (temp == 0));
	    }
	    break;
	    case 0x9:
	    {
		uint32_t temp = (srcreg ^ offs);
		arm->setnz(TestBit(temp, 31), (temp == 0));
	    }
	    break;
	    case 0xA:
	    {
		uint32_t temp = (srcreg - offs);
		arm->setnzcv(TestBit(temp, 31), (temp == 0), CARRY_SUB(srcreg, offs), OVERFLOW_SUB(srcreg, offs, temp));
	    }
	    break;
	    case 0xC:
	    {
		destval = (srcreg | offs);

		if (setcond)
		{
		    arm->setnz(TestBit(destval, 31), (destval == 0));
		}
	    }
	    break;
	    case 0xD:
	    {
		destval = offs;

		if (setcond)
		{
		    arm->setnz(TestBit(destval, 31), (destval == 0));
		}
	    }
	    break;
	    case 0xE:
	    {
		destval = (srcreg & ~offs);

		if (setcond)
		{
		    arm->setnz(TestBit(destval, 31), (destval == 0));
		}
	    }
	    break;
	    case 0xF:
	    {
		destval = (~srcreg);

		if (setcond)
		{
		    arm->setnz(TestBit(destval, 31), (destval == 0));
		}
	    }
	    break;
	    default: cout << "Unrecognized ALU instruction of " << hex << (int)(opcode) << endl; exit(1); break;
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

	uint8_t oprmreg = (instr & 0xF);
	uint8_t oprsreg = ((instr >> 8) & 0xF);
	uint8_t accumreg = ((instr >> 12) & 0xF);
	uint8_t destreg = ((instr >> 16) & 0xF);

	bool setcond = TestBit(instr, 20);
	uint8_t opcode = ((instr >> 21) & 0xF);

	if ((oprmreg == 15) || (oprsreg == 15) || (accumreg == 15) || (destreg == 15))
	{
	    cout << "Warning - ARM.7 PC used as register" << endl;
	}

	uint32_t rm = arm->getreg(oprmreg);
	uint32_t rs = arm->getreg(oprsreg);
	uint32_t rn = arm->getreg(accumreg);
	uint32_t rd = arm->getreg(destreg);

	uint64_t val64 = 1;
	uint64_t hilo = 0;
	int64_t vals64 = 1;
	uint32_t val32 = 0;

	switch (opcode)
	{
	    case 0:
	    {
		val32 = (rm * rs);
		arm->setreg(destreg, val32);

		if (setcond)
		{
		    arm->setnz(TestBit(val32, 31), (val32 == 0));
		}
	    }
	    break;
	    case 1:
	    {
		val32 = ((rm * rs) + rn);
		arm->setreg(destreg, val32);

		if (setcond)
		{
		    arm->setnz(TestBit(val32, 31), (val32 == 0));
		}
	    }
	    break;
	    case 4:
	    {
		val64 = (val64 * rm * rs);
		rn = (val64 & 0xFFFFFFFF);
		rd = (val64 >> 32);

		arm->setreg(accumreg, rn);
		arm->setreg(destreg, rd);

		if (setcond)
		{
		    arm->setnz(TestBit(val64, 63), (val64 == 0));
		}
	    }
	    break;
	    case 5:
	    {
		hilo = rd;
		hilo <<= 16;
		hilo <<= 16;
		hilo |= rn;

		val64 = ((val64 * rm * rs) + hilo);
		rn = (val64 & 0xFFFFFFFF);
		rd = (val64 >> 32);

		arm->setreg(accumreg, rn);
		arm->setreg(destreg, rd);

		if (setcond)
		{
		    arm->setnz(TestBit(val64, 63), (val64 == 0));
		}
	    }
	    break;
	    case 6:
	    {
		vals64 = (vals64 * (int32_t)rm * (int32_t)rs);
		val64 = vals64;
		rn = (vals64 & 0xFFFFFFFF);
		rd = (vals64 >> 32);

		arm->setreg(accumreg, rn);
		arm->setreg(destreg, rd);

		if (setcond)
		{
		    arm->setnz(TestBit(vals64, 63), (vals64 == 0));
		}
	    }
	    break;
	    case 7:
	    {
		hilo = rd;
		hilo <<= 16;
		hilo <<= 16;
		hilo |= rn;

		vals64 = ((vals64 * (int32_t)rm * (int32_t)rs) + hilo);
		val64 = vals64;
		rn = (vals64 & 0xFFFFFFFF);
		rd = (vals64 >> 32);

		arm->setreg(accumreg, rn);
		arm->setreg(destreg, rd);

		if (setcond)
		{
		    arm->setnz(TestBit(vals64, 63), (vals64 == 0));
		}
	    }
	    break;
	    default: cout << "Unrecognized ARM.7 opcode of " << hex << (int)(opcode) << endl; exit(1); break;
	}
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

	uint32_t baseoffs = 0;
	uint32_t baseaddr = srcreg;

	if (!usereg)
	{
	    baseoffs = (instr & 0xFFF);
	}
	else
	{
	    uint8_t offsreg = (instr & 0xF);
	    baseoffs = arm->getreg(offsreg);
	    uint8_t shifttype = ((instr >> 5) & 0x3);
	    uint8_t shiftoffs = ((instr >> 7) & 0x1F);

	    switch (shifttype)
	    {
		case 0: LSL(baseoffs, shiftoffs); break;
		case 1: LSR(baseoffs, shiftoffs); break;
		case 2: ASR(baseoffs, shiftoffs); break;
		case 3: ROR(baseoffs, shiftoffs); break;
	    }
	}

	if (preorpost)
	{
	    if (upordown)
	    {
		baseaddr += baseoffs;
	    }
	    else
	    {
		baseaddr -= baseoffs;
	    }
	}

	arm->clock(arm->getreg(15), CODE_N32);

	if (!ldrorstr)
	{
	    if (byteorword)
	    {
		uint32_t value = dstreg;

		if (dst == 15)
		{
		    value += 4;
		}

		arm->writeByte(baseaddr, (value & 0xFF));
		arm->clock(baseaddr, DATA_N16);
	    }
	    else
	    {
		uint32_t value = dstreg;

		if (dst == 15)
		{
		    value += 4;
		}

		arm->writeLong(baseaddr, value);
		arm->clock(baseaddr, DATA_N32);
	    }
	}
	else
	{
	    if (byteorword)
	    {
		uint32_t value = arm->readByte(baseaddr);
		arm->clock();

		if (dst == 15)
		{
		    arm->clock((arm->getreg(15) + 4), DATA_N16);
		}

		arm->setreg(dst, value);
	    }
	    else
	    {
		uint32_t value = arm->readLong(baseaddr);
		arm->clock();

		if (dst == 15)
		{
		    arm->clock((arm->getreg(15) + 4), DATA_N32);
		}

		arm->setreg(dst, value);
	    }
	}

	if (!preorpost)
	{
	    if (upordown)
	    {
		baseaddr += baseoffs;
	    }
	    else
	    {
		baseaddr -= baseoffs;
	    }
	}

	if (!preorpost && (src != dst))
	{
	    arm->setreg(src, baseaddr);
	}
	else if (preorpost && iswriteback && (src != dst))
	{
	    arm->setreg(src, baseaddr);
	}

	if ((dst == 15) && ldrorstr)
	{
	    if (TestBit(arm->getreg(15), 0))
	    {
		arm->setthumbmode(true);
		arm->setreg(15, BitReset(arm->getreg(15), 0));
	    }

	    arm->clock(arm->getreg(15), CODE_S32);
	    arm->clock((arm->getreg(15) + 4), CODE_S32);
	    arm->flushpipeline();
	}
	else if ((dst != 15) && ldrorstr)
	{
	    arm->clock(arm->getreg(15), CODE_S32);
	}
    }

    inline void arm10(BeeARM *arm)
    {
	uint32_t instr = arm->currentarminstr.armvalue;
	
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
	uint32_t addr = basereg;

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
	// TODO: Clock cycle timings (may need hardware testing)
	uint32_t instr = arm->currentarminstr.armvalue;
	cout << "ARM.11" << endl;
	
	bool prepost = TestBit(instr, 24);
	bool updown = TestBit(instr, 23);
	bool psr = TestBit(instr, 22);
	bool writeback = TestBit(instr, 21);
	bool loadstore = TestBit(instr, 20);
	
	int base = ((instr >> 16) & 0xF);
	uint16_t reglist = (instr & 0xFFFF);

	if (base == 15)
	{
	    cout << "Warning - PC used as base register" << endl;
	}

	if (psr)
	{
	    cout << "Setting USR mode..." << endl;
	}

	uint32_t baseaddr = arm->getreg(base);
	uint32_t oldbase = baseaddr;
	uint8_t transferreg = 0xFF;

	for (int i = 0; i < 16; i++)
	{
	    if (TestBit(reglist, i))
	    {
		transferreg = i;
		i = 0xFF;
		break;
	    }
	}

	if (updown && (reglist != 0))
	{
	    for (int i = 0; i < 16; i++)
	    {
		if (TestBit(reglist, i))
		{
		    if (prepost)
		    {
			baseaddr += 4;
		    }

		    if (!loadstore)
		    {
			if ((i == transferreg) && (base == transferreg))
			{
			    arm->writeLong(baseaddr, oldbase);
			}
			else
			{
			    arm->writeLong(baseaddr, arm->getreg(i));
			}
		    }
		    else
		    {
			if ((i == transferreg) && (base == transferreg))
			{
			    writeback = false;
			}

			arm->setreg(i, arm->readLong(baseaddr));

			if (i == 15)
			{
			    arm->flushpipeline();
			}
		    }

		    if (!prepost)
		    {
			baseaddr += 4;
		    }
		}

		if (writeback)
		{
		    arm->setreg(base, baseaddr);
		}
	    }
	}
	else if (!updown && (reglist != 0))
	{
	    for (int i = 15; i >= 0; i--)
	    {
		if (TestBit(reglist, i))
		{
		    if (prepost)
		    {
			baseaddr -= 4;
		    }

		    if (!loadstore)
		    {
			if ((i == transferreg) && (base == transferreg))
			{
			    arm->writeLong(baseaddr, oldbase);
			}
			else
			{
			    arm->writeLong(baseaddr, arm->getreg(i));
			}
		    }
		    else
		    {
			if ((i == transferreg) && (base == transferreg))
			{
			    writeback = false;
			}

			arm->setreg(i, arm->readLong(baseaddr));

			if (i == 15)
			{
			    arm->flushpipeline();
			}
		    }

		    if (!prepost)
		    {
			baseaddr -= 4;
		    }
		}

		if (writeback)
		{
		    arm->setreg(base, baseaddr);
		}
	    }
	}
	else
	{
	    if (!loadstore)
	    {
		arm->writeLong(baseaddr, arm->getreg(15));
	    }
	    else
	    {
		arm->setreg(15, arm->readLong(baseaddr));
		arm->flushpipeline();
	    }

	    if (updown)
	    {
		arm->setreg(base, (baseaddr + 0x40));
	    }
	    else
	    {
		arm->setreg(base, (baseaddr - 0x40));
	    }
	}

	if (psr)
	{
	    cout << "Setting current mode..." << endl;
	}
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
