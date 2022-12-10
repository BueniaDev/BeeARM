#include "beearm.h"
using namespace beearm;

BeeARM::BeeARM()
{
    for (uint32_t index = 0; index < 4096; index++)
    {
	uint32_t instr = (((index & 0xFF0) << 16) | ((index & 0xF) << 4));
	arm_func_table.push_back(decodeARMInstr(instr));
    }
}

BeeARM::~BeeARM()
{
    arm_func_table.clear();
}

BeeARM::beeARMmapping BeeARM::decodeARMInstr(uint32_t instr)
{
    for (auto it = funcmappings.rbegin(); it != funcmappings.rend(); it++)
    {
	auto mapping = *it;
	if ((instr & mapping.mask) == mapping.value)
	{
	    return mapping;
	}
    }

    return arminstruction(0xFFFFFFFF, instr, unknown);
}

void BeeARM::init()
{
    pipeline = {};
    arm_regs = {};
    cout << "BeeARM::Initialized" << endl;
}

void BeeARM::shutdown()
{
    cout << "BeeARM::Shutting down..." << endl;
}

void BeeARM::setInterface(BeeARMInterface &cb)
{
    inter = unique_ptr<BeeARMInterface>(&cb);
}

void BeeARM::runInstruction()
{
    exit(0);
}

BeeARM7::BeeARM7()
{
    for (uint32_t index = 0; index < 4096; index++)
    {
	uint32_t instr = (((index & 0xFF0) << 16) | ((index & 0xF) << 4));
	arm_func_table.push_back(decodeARMInstr(instr));
    }

    for (uint32_t index = 0; index < 1024; index++)
    {
	uint32_t instr = (index << 6);
	thumb_func_table.push_back(decodeTHUMBInstr(instr));
    }
}

BeeARM7::~BeeARM7()
{
    thumb_func_table.clear();
    arm_func_table.clear();
}

BeeARM7::beeARM7mapping BeeARM7::decodeARMInstr(uint32_t instr)
{
    for (auto it = arm7funcmappings.rbegin(); it != arm7funcmappings.rend(); it++)
    {
	auto mapping = *it;

	if ((instr & mapping.mask) == mapping.value)
	{
	    return mapping;
	}
    }

    return arm7instruction(0xFFFFFFFF, instr, unknown);
}

BeeARM7::beeTHUMBmapping BeeARM7::decodeTHUMBInstr(uint16_t instr)
{
    for (auto it = thumbfuncmappings.rbegin(); it != thumbfuncmappings.rend(); it++)
    {
	auto mapping = *it;

	if ((instr & mapping.mask) == mapping.value)
	{
	    return mapping;
	}
    }

    return thumbinstruction(0xFFFF, instr, unknown);
}

void BeeARM7::setArchitecture(string arch_name)
{
    auto value = arch_strings.find(arch_name);

    if (value == arch_strings.end())
    {
	stringstream ss;
	ss << "Invalid architecture name of " << arch_name;
	throw invalid_argument(ss.str());
	return;
    }

    setArchitecture(value->second);
}

void BeeARM7::init()
{
    pipeline = {};

    arm_regs = {};
    arm_regs.r15 = 0;

    setCPSR(0xD3);

    cout << "BeeARM7::Initialized" << endl;
}

void BeeARM7::throwException(BeeARM7Mode mode, uint32_t address)
{
    setMode(mode);
    uint32_t cpsr = getCPSR();
    setSPSR(cpsr);
    cpsr = resetbit(cpsr, 5);
    cpsr = ((cpsr & ~0x1F) | int(mode));

    if (mode == Fiq)
    {
	cpsr = setbit(cpsr, 6);
    }

    cpsr = setbit(cpsr, 7);
    setCPSR(cpsr);
    setReg(14, pipeline.decode.addr);
    setReg(15, address);
    pipeline.is_reload = true;
}

void BeeARM7::setArchitecture(BeeARM7Architecture arch)
{
    architecture = arch;

    switch (arch)
    {
	case v3G:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = false;
	    is_bx_enabled = false;
	    version_num = 3;
	}
	break;
	case v3M:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = true;
	    is_bx_enabled = false;
	    version_num = 3;
	}
	break;
	case v4:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = true;
	    is_bx_enabled = false;
	    version_num = 4;
	}
	break;
	case v4T:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = true;
	    is_bx_enabled = true;
	    version_num = 4;
	}
	break;
	case v4xM:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = false;
	    is_bx_enabled = false;
	    version_num = 4;
	}
	break;
	case v4TxM:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = false;
	    is_bx_enabled = true;
	    version_num = 4;
	}
	break;
	case v5:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = true;
	    is_bx_enabled = false;
	    version_num = 5;
	}
	break;
	case v5T:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = true;
	    is_bx_enabled = true;
	    version_num = 5;
	}
	break;
	case v5xM:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = false;
	    is_bx_enabled = false;
	    version_num = 5;
	}
	break;
	case v5TxM:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = false;
	    is_bx_enabled = true;
	    version_num = 5;
	}
	break;
	case v5E:
	{
	    is_thumb_enabled = false;
	    is_mult_long_enabled = true;
	    is_bx_enabled = false;
	    version_num = 5;
	}
	break;
	case v5TE:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = true;
	    is_bx_enabled = true;
	    version_num = 5;
	}
	break;
	case v5TExP:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = true;
	    is_bx_enabled = true;
	    version_num = 5;
	}
	break;
	case v5TEJ:
	{
	    is_thumb_enabled = true;
	    is_mult_long_enabled = true;
	    is_bx_enabled = true;
	    version_num = 5;
	}
	break;
    }
}

void BeeARM7::initGBA()
{
    setArchitecture("armv4t");
    pipeline = {};

    arm_regs = {};

    arm_regs.r13 = 0x3007F00;
    arm_regs.r13_fiq = 0x3007F00;
    arm_regs.r13_abt = 0x3007F00;
    arm_regs.r13_und = 0x3007F00;
    arm_regs.r13_svc = 0x3007FE0;
    arm_regs.r13_irq = 0x3007FA0;

    arm_regs.r15 = 0x8000000;
    setCPSR(0x5F);

    cout << "BeeARM7::Initialized" << endl;
}

void BeeARM7::shutdown()
{
    cout << "BeeARM7::Shutting down..." << endl;
}

void BeeARM7::setInterface(BeeARMInterface *cb)
{
    inter = cb;
}

void BeeARM7::setCoprocessor(int num, BeeARM7Coprocessor *cb)
{
    if ((num < 0) || (num > 15))
    {
	throw out_of_range("Invalid coprocessor number");
    }

    if (cb == NULL)
    {
	cout << "Pointer is NULL" << endl;
	throw runtime_error("BeeARM error");
    }

    if (!coprocs.at(num))
    {
	coprocs.at(num) = unique_ptr<BeeARM7Coprocessor>(cb);
    }

    if (coprocs.at(num))
    {
	coprocs.at(num)->reset();
    }
}

uint32_t BeeARM7::getReg(int reg)
{
    uint32_t data = 0;
    if ((reg < 0) || (reg > 15))
    {
	throw out_of_range("Invalid register number");
    }

    switch (reg)
    {
	case 0: data = arm_regs.r0; break;
	case 1: data = arm_regs.r1; break;
	case 2: data = arm_regs.r2; break;
	case 3: data = arm_regs.r3; break;
	case 4: data = arm_regs.r4; break;
	case 5: data = arm_regs.r5; break;
	case 6: data = arm_regs.r6; break;
	case 7: data = arm_regs.r7; break;
	case 8:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		data = arm_regs.r8_fiq;
	    }
	    else
	    {
		data = arm_regs.r8;
	    }
	}
	break;
	case 9:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		data = arm_regs.r9_fiq;
	    }
	    else
	    {
		data = arm_regs.r9;
	    }
	}
	break;
	case 10:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		data = arm_regs.r10_fiq;
	    }
	    else
	    {
		data = arm_regs.r10;
	    }
	}
	break;
	case 11:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		data = arm_regs.r11_fiq;
	    }
	    else
	    {
		data = arm_regs.r11;
	    }
	}
	break;
	case 12:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		data = arm_regs.r12_fiq;
	    }
	    else
	    {
		data = arm_regs.r12;
	    }
	}
	break;
	case 13:
	{
	    switch (current_mode)
	    {
		case BeeARM7Mode::Fiq: data = arm_regs.r13_fiq; break;
		case BeeARM7Mode::Irq: data = arm_regs.r13_irq; break;
		case BeeARM7Mode::Svc: data = arm_regs.r13_svc; break;
		case BeeARM7Mode::Abt: data = arm_regs.r13_abt; break;
		case BeeARM7Mode::Und: data = arm_regs.r13_und; break;
		default: data = arm_regs.r13; break;
	    }
	}
	break;
	case 14:
	{
	    switch (current_mode)
	    {
		case BeeARM7Mode::Fiq: data = arm_regs.r14_fiq; break;
		case BeeARM7Mode::Irq: data = arm_regs.r14_irq; break;
		case BeeARM7Mode::Svc: data = arm_regs.r14_svc; break;
		case BeeARM7Mode::Abt: data = arm_regs.r14_abt; break;
		case BeeARM7Mode::Und: data = arm_regs.r14_und; break;
		default: data = arm_regs.r14; break;
	    }
	}
	break;
	case 15: data = arm_regs.r15; break;
    }

    return data;
}

void BeeARM7::setReg(int reg, uint32_t data)
{
    if ((reg < 0) || (reg > 15))
    {
	throw out_of_range("Invalid register number");
    }

    switch (reg)
    {
	case 0: arm_regs.r0 = data; break;
	case 1: arm_regs.r1 = data; break;
	case 2: arm_regs.r2 = data; break;
	case 3: arm_regs.r3 = data; break;
	case 4: arm_regs.r4 = data; break;
	case 5: arm_regs.r5 = data; break;
	case 6: arm_regs.r6 = data; break;
	case 7: arm_regs.r7 = data; break;
	case 8:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r8_fiq = data;
	    }
	    else
	    {
		arm_regs.r8 = data;
	    }
	}
	break;
	case 9:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r9_fiq = data;
	    }
	    else
	    {
		arm_regs.r9 = data;
	    }
	}
	break;
	case 10:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r10_fiq = data;
	    }
	    else
	    {
		arm_regs.r10 = data;
	    }
	}
	break;
	case 11:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r11_fiq = data;
	    }
	    else
	    {
		arm_regs.r11 = data;
	    }
	}
	break;
	case 12:
	{
	    if (current_mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r12_fiq = data;
	    }
	    else
	    {
		arm_regs.r12 = data;
	    }
	}
	break;
	case 13:
	{
	    switch (current_mode)
	    {
		case BeeARM7Mode::Fiq: arm_regs.r13_fiq = data; break;
		case BeeARM7Mode::Irq: arm_regs.r13_irq = data; break;
		case BeeARM7Mode::Svc: arm_regs.r13_svc = data; break;
		case BeeARM7Mode::Abt: arm_regs.r13_abt = data; break;
		case BeeARM7Mode::Und: arm_regs.r13_und = data; break;
		default: arm_regs.r13 = data; break;
	    }
	}
	break;
	case 14:
	{
	    switch (current_mode)
	    {
		case BeeARM7Mode::Fiq: arm_regs.r14_fiq = data; break;
		case BeeARM7Mode::Irq: arm_regs.r14_irq = data; break;
		case BeeARM7Mode::Svc: arm_regs.r14_svc = data; break;
		case BeeARM7Mode::Abt: arm_regs.r14_abt = data; break;
		case BeeARM7Mode::Und: arm_regs.r14_und = data; break;
		default: arm_regs.r14 = data; break;
	    }
	}
	break;
	case 15: arm_regs.r15 = data; break;
    }
}

void BeeARM7::setReg(BeeARM7Mode mode, int reg, uint32_t data)
{
    if ((reg < 0) || (reg > 15))
    {
	throw out_of_range("Invalid register number");
    }

    switch (reg)
    {
	case 0: arm_regs.r0 = data; break;
	case 1: arm_regs.r1 = data; break;
	case 2: arm_regs.r2 = data; break;
	case 3: arm_regs.r3 = data; break;
	case 4: arm_regs.r4 = data; break;
	case 5: arm_regs.r5 = data; break;
	case 6: arm_regs.r6 = data; break;
	case 7: arm_regs.r7 = data; break;
	case 8:
	{
	    if (mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r8_fiq = data;
	    }
	    else
	    {
		arm_regs.r8 = data;
	    }
	}
	break;
	case 9:
	{
	    if (mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r9_fiq = data;
	    }
	    else
	    {
		arm_regs.r9 = data;
	    }
	}
	break;
	case 10:
	{
	    if (mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r10_fiq = data;
	    }
	    else
	    {
		arm_regs.r10 = data;
	    }
	}
	break;
	case 11:
	{
	    if (mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r11_fiq = data;
	    }
	    else
	    {
		arm_regs.r11 = data;
	    }
	}
	break;
	case 12:
	{
	    if (mode == BeeARM7Mode::Fiq)
	    {
		arm_regs.r12_fiq = data;
	    }
	    else
	    {
		arm_regs.r12 = data;
	    }
	}
	break;
	case 13:
	{
	    switch (mode)
	    {
		case BeeARM7Mode::Fiq: arm_regs.r13_fiq = data; break;
		case BeeARM7Mode::Irq: arm_regs.r13_irq = data; break;
		case BeeARM7Mode::Svc: arm_regs.r13_svc = data; break;
		case BeeARM7Mode::Abt: arm_regs.r13_abt = data; break;
		case BeeARM7Mode::Und: arm_regs.r13_und = data; break;
		default: arm_regs.r13 = data; break;
	    }
	}
	break;
	case 14:
	{
	    switch (mode)
	    {
		case BeeARM7Mode::Fiq: arm_regs.r14_fiq = data; break;
		case BeeARM7Mode::Irq: arm_regs.r14_irq = data; break;
		case BeeARM7Mode::Svc: arm_regs.r14_svc = data; break;
		case BeeARM7Mode::Abt: arm_regs.r14_abt = data; break;
		case BeeARM7Mode::Und: arm_regs.r14_und = data; break;
		default: arm_regs.r14 = data; break;
	    }
	}
	break;
	case 15: arm_regs.r15 = data; break;
    }
}

uint32_t BeeARM7::getCPSR()
{
    return arm_regs.cpsr;
}

void BeeARM7::setCPSR(uint32_t data)
{
    arm_regs.cpsr = data;

    switch (data & 0x1F)
    {
	case 0x10: setMode(Usr); break;
	case 0x11: setMode(Fiq); break;
	case 0x12: setMode(Irq); break;
	case 0x13: setMode(Svc); break;
	case 0x17: setMode(Abt); break;
	case 0x1B: setMode(Und); break;
	case 0x1F: setMode(Sys); break;
	default:
	{
	    cout << "Unrecongized CPSR mode of " << hex << int(data & 0x1F) << endl;
	}
	break;
    }
}

uint32_t BeeARM7::getSPSR()
{
    uint32_t data = 0;

    switch (current_mode)
    {
	case BeeARM7Mode::Fiq: data = arm_regs.spsr_fiq; break;
	case BeeARM7Mode::Svc: data = arm_regs.spsr_svc; break;
	case BeeARM7Mode::Abt: data = arm_regs.spsr_abt; break;
	case BeeARM7Mode::Irq: data = arm_regs.spsr_irq; break;
	case BeeARM7Mode::Und: data = arm_regs.spsr_und; break;
	default: data = arm_regs.cpsr; break;
    }

    return data;
}

void BeeARM7::setSPSR(uint32_t data)
{
    switch (current_mode)
    {
	case BeeARM7Mode::Fiq: arm_regs.spsr_fiq = data; break;
	case BeeARM7Mode::Svc: arm_regs.spsr_svc = data; break;
	case BeeARM7Mode::Abt: arm_regs.spsr_abt = data; break;
	case BeeARM7Mode::Irq: arm_regs.spsr_irq = data; break;
	case BeeARM7Mode::Und: arm_regs.spsr_und = data; break;
	default: break;
    }
}

void BeeARM7::debugoutput(bool print_dasm)
{
    for (int i = 0; i < 16; i++)
    {
	cout << "R" << dec << i << ": " << hex << int(getReg(i)) << endl;
    }

    cout << "CPSR: " << hex << int(arm_regs.cpsr) << endl;
    cout << "Current instruction: " << hex << int(pipeline.execute.instr) << endl;

    cout << endl;
}

void BeeARM7::fetch()
{
    pipeline.execute = pipeline.decode;
    pipeline.decode = pipeline.fetch;
    pipeline.decode.is_thumb = isThumb();

    int flags = BeeARMFlags::Sequential;

    if (pipeline.is_ns)
    {
	pipeline.is_ns = false;
	flags = BeeARMFlags::Nonsequential;
    }

    uint32_t mask = isThumb() ? 1 : 3;
    int size = isThumb() ? BeeARMFlags::Word : BeeARMFlags::Long;

    arm_regs.r15 += (isThumb() ? 2 : 4);
    pipeline.fetch.addr = (arm_regs.r15 & ~mask);
    pipeline.fetch.instr = readInstruction(size, flags, pipeline.fetch.addr);
}

void BeeARM7::runInstruction()
{
    try
    {
	uint32_t mask = isThumb() ? 1 : 3;
	int size = isThumb() ? BeeARMFlags::Word : BeeARMFlags::Long;

	if (pipeline.is_reload)
	{
	    pipeline.is_reload = false;
	    arm_regs.r15 &= ~mask;
	    pipeline.fetch.addr = (getReg(15) & ~mask);
	    pipeline.fetch.instr = readPrefetch(size, pipeline.fetch.addr);
	    fetch();
	}

	fetch();

	if (is_interrupt && !testbit(arm_regs.cpsr, 7))
	{
	    throwException(Irq, 0x18);

	    if (pipeline.execute.is_thumb)
	    {
		setReg(14, (getReg(14) + 2));
	    }

	    is_interrupt = false;

	    return;
	}

	uint32_t opcode = pipeline.execute.instr;

	if (!pipeline.execute.is_thumb)
	{
	    int cond = ((opcode >> 28) & 0xF);

	    // Don't execute an instruction if its condition is false
	    if (!isCondition(cond))
	    {
		return;
	    }

	    uint16_t instr = (((opcode >> 16) & 0xFF0) | ((opcode >> 4) & 0xF));
	    arm_func_table.at(instr).function(opcode);
	}
	else
	{
	    uint16_t instr = (uint16_t(opcode) >> 6);
	    thumb_func_table.at(instr).function(opcode);
	}
    }
    catch (exception &ex)
    {
	cout << ex.what() << endl;
	debugoutput();
	throw runtime_error("BeeARM error");
    }
}

void BeeARM7::fireInterrupt(bool line)
{
    is_interrupt = line;
}