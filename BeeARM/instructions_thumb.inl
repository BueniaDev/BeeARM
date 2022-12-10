void thumb_shift_imm(uint16_t instr)
{
    int opcode = ((instr >> 11) & 0x3);

    int shift_offs = ((instr >> 6) & 0x1F);
    int src_reg = ((instr >> 3) & 0x7);
    int dst_reg = (instr & 0x7);

    uint32_t src_param = getReg(src_reg);
    bool is_carry = isCarry();

    switch (opcode)
    {
	case 0:
	{
	    uint32_t operand = logicalShiftLeft(src_param, shift_offs, is_carry);
	    setNZ(operand);
	    setCarry(is_carry);
	    setReg(dst_reg, operand);
	}
	break;
	case 1:
	{
	    uint32_t operand = logicalShiftRight(src_param, shift_offs, is_carry, true);
	    setNZ(operand);
	    setCarry(is_carry);
	    setReg(dst_reg, operand);
	}
	break;
	case 2:
	{
	    uint32_t operand = arithmeticShiftRight(src_param, shift_offs, is_carry, true);
	    setNZ(operand);
	    setCarry(is_carry);
	    setReg(dst_reg, operand);
	}
	break;
	default:
	{
	    cout << "Unrecongized shift opcode of " << hex << int(opcode) << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_add_subtract(uint16_t instr)
{
    int opcode = ((instr >> 9) & 0x3);
    int src_reg = ((instr >> 3) & 0x7);
    int dst_reg = (instr & 0x7);

    int oper_reg = ((instr >> 6) & 0x7);

    uint32_t src_param = getReg(src_reg);
    uint32_t oper_param = 0;

    switch (opcode)
    {
	case 0:
	{
	    oper_param = getReg(oper_reg);
	    uint32_t result = addOperand(src_param, oper_param);
	    setReg(dst_reg, result);
	}
	break;
	case 1:
	{
	    oper_param = getReg(oper_reg);
	    uint32_t result = subOperand(src_param, oper_param);
	    setReg(dst_reg, result);
	}
	break;
	case 2:
	{
	    oper_param = oper_reg;
	    uint32_t result = addOperand(src_param, oper_param);
	    setReg(dst_reg, result);
	}
	break;
	case 3:
	{
	    oper_param = oper_reg;
	    uint32_t result = subOperand(src_param, oper_param);
	    setReg(dst_reg, result);
	}
	break;
	default:
	{
	    cout << "Unrecognized THUMB add/subtract mode of " << hex << int(opcode) << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_immed(uint16_t instr)
{
    int opcode = ((instr >> 11) & 0x3);
    int dst_reg = ((instr >> 8) & 0x7);
    uint32_t imm = (instr & 0xFF);
    uint32_t operand = 0;

    switch (opcode)
    {
	case 0:
	{
	    operand = imm;
	    setNZ(operand);
	    setReg(dst_reg, operand);
	}
	break;
	case 1:
	{
	    uint32_t src_param = getReg(dst_reg);
	    cmpOperand(src_param, imm);
	}
	break;
	case 2:
	{
	    uint32_t src_param = getReg(dst_reg);
	    operand = addOperand(src_param, imm);
	    setReg(dst_reg, operand);
	}
	break;
	case 3:
	{
	    uint32_t src_param = getReg(dst_reg);
	    operand = subOperand(src_param, imm);
	    setReg(dst_reg, operand);
	}
	break;
	default:
	{
	    cout << "Unrecognized THUMB immediate opcode of " << dec << int(opcode) << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_alu(uint16_t instr)
{
    int src_reg = ((instr >> 3) & 0x7);
    int dst_reg = (instr & 0x7);

    int opcode = ((instr >> 6) & 0xF);
    bool is_carry = isCarry();

    switch (opcode)
    {
	// AND
	case 0x0:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    uint32_t result = (dst_param & src_param);
	    setNZ(result);
	    setReg(dst_reg, result);
	}
	break;
	// EOR
	case 0x1:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    uint32_t result = (dst_param ^ src_param);
	    setNZ(result);
	    setReg(dst_reg, result);
	}
	break;
	// LSL
	case 0x2:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = logicalShiftLeft(dst_param, (src_param & 0xFF), is_carry);
	    setNZ(result);
	    setCarry(is_carry);
	    setReg(dst_reg, result);
	}
	break;
	// LSR
	case 0x3:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = logicalShiftRight(dst_param, (src_param & 0xFF), is_carry, false);
	    setNZ(result);
	    setCarry(is_carry);
	    setReg(dst_reg, result);
	}
	break;
	// ASR
	case 0x4:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = arithmeticShiftRight(dst_param, (src_param & 0xFF), is_carry, false);
	    setNZ(result);
	    setCarry(is_carry);
	    setReg(dst_reg, result);
	}
	break;
	// ADC
	case 0x5:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = adcOperand(dst_param, src_param);
	    setReg(dst_reg, result);
	}
	break;
	// SBC
	case 0x6:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = sbcOperand(dst_param, src_param);
	    setReg(dst_reg, result);
	}
	break;
	// ROR
	case 0x7:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = rotateRight(dst_param, (src_param & 0xFF), is_carry, false);
	    setNZ(result);
	    setCarry(is_carry);
	    setReg(dst_reg, result);
	}
	break;
	// TST
	case 0x8:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    uint32_t result = (dst_param & src_param);
	    setNZ(result);
	}
	break;
	// NEG
	case 0x9:
	{
	    uint32_t src_param = getReg(src_reg);

	    uint32_t result = negOperand(src_param);
	    setReg(dst_reg, result);
	}
	break;
	// CMP
	case 0xA:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    cmpOperand(dst_param, src_param);
	}
	break;
	// CMN
	case 0xB:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    cmnOperand(dst_param, src_param);
	}
	break;
	// ORR
	case 0xC:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    uint32_t result = (dst_param | src_param);
	    setNZ(result);
	    setReg(dst_reg, result);
	}
	break;
	// MUL
	case 0xD:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = (dst_param * src_param);
	    setNZ(result);
	    setReg(dst_reg, result);
	    multiplyThumbCycles(dst_param);
	}
	break;
	// BIC
	case 0xE:
	{
	    uint32_t dst_param = getReg(dst_reg);
	    uint32_t src_param = getReg(src_reg);

	    uint32_t result = (dst_param & ~src_param);
	    setNZ(result);
	    setReg(dst_reg, result);
	}
	break;
	// MVN
	case 0xF:
	{
	    uint32_t src_param = getReg(src_reg);
	    uint32_t result = (~src_param);
	    setNZ(result);
	    setReg(dst_reg, result);
	}
	break;
	default:
	{
	    cout << "Unrecognized THUMB ALU opcode of " << dec << int(opcode) << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_hi_reg(uint16_t instr)
{
    int opcode = ((instr >> 8) & 0x3);
    int dst_reg = (instr & 0x7);
    int src_reg = ((instr >> 3) & 0x7);
    bool msb_d = testbit(instr, 7);
    bool msb_s = testbit(instr, 6);

    src_reg |= (msb_s << 3);
    dst_reg |= (msb_d << 3);

    if (isVersionGreaterThanOrEqual(5))
    {
	if ((opcode == 3) && msb_d)
	{
	    opcode = 4;
	}
    }

    switch (opcode)
    {
	// ADD Rd, Rs
	case 0:
	{
	    uint32_t source1 = getReg(dst_reg);
	    uint32_t source2 = getReg(src_reg);

	    setReg(dst_reg, (source1 + source2));

	    if (dst_reg == 15)
	    {
		pipeline.is_reload = true;
	    }
	}
	break;
	// CMP Rd, Rs
	case 1:
	{
	    uint32_t source1 = getReg(dst_reg);
	    uint32_t source2 = getReg(src_reg);
	    cmpOperand(source1, source2);
	}
	break;
	// MOV Rd, Rs
	case 2:
	{
	    uint32_t operand = getReg(src_reg);
	    setReg(dst_reg, operand);

	    if (dst_reg == 15)
	    {
		pipeline.is_reload = true;
	    }
	}
	break;
	// BX Rs
	// TODO: Implement BLX (for ARM v5T)
	case 3:
	{
	    uint32_t addr = getReg(src_reg);
	    setThumb(testbit(addr, 0));
	    setReg(15, resetbit(addr, 0));
	    pipeline.is_reload = true;
	}
	break;
	default:
	{
	    cout << "Unrecognized THUMB high reg opcode of " << dec << opcode << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_load_pc_rel(uint16_t instr)
{
    int dst_reg = ((instr >> 8) & 0x7);
    uint32_t imm = ((instr & 0xFF) << 2);

    uint32_t addr = ((getReg(15) & ~3) + imm);
    setReg(dst_reg, loadMemory(Long | Nonsequential, addr));
}

void thumb_load_store_reg(uint16_t instr)
{
    int opcode = ((instr >> 10) & 0x3);
    int dst_reg = (instr & 0x7);
    int base_reg = ((instr >> 3) & 0x7);
    int offs_reg = ((instr >> 6) & 0x7);

    uint32_t base_addr = (getReg(base_reg) + getReg(offs_reg));

    switch (opcode)
    {
	// STR
	case 0:
	{
	    uint32_t data = getReg(dst_reg);
	    storeMemory(Long | Nonsequential, base_addr, data);
	}
	break;
	// STRB
	case 1:
	{
	    uint32_t data = getReg(dst_reg);
	    storeMemory(Byte | Nonsequential, base_addr, data);
	}
	break;
	// LDR
	case 2:
	{
	    uint32_t data = loadMemory(Long | Nonsequential, base_addr);
	    setReg(dst_reg, data);
	}
	break;
	// LDR
	case 3:
	{
	    uint32_t data = loadMemory(Byte | Nonsequential, base_addr);
	    setReg(dst_reg, data);
	}
	break;
	default:
	{
	    cout << "Unrecognized THUMB load/store register offset opcode of " << dec << int(opcode) << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_load_store_half_sb(uint16_t instr)
{
    int opcode = ((instr >> 10) & 0x3);
    int dst_reg = (instr & 0x7);
    int base_reg = ((instr >> 3) & 0x7);
    int offs_reg = ((instr >> 6) & 0x7);

    uint32_t base_addr = (getReg(base_reg) + getReg(offs_reg));

    switch (opcode)
    {
	// STRH
	case 0:
	{
	    uint32_t data = getReg(dst_reg);
	    storeMemory(Word | Nonsequential, base_addr, data);
	}
	break;
	// LDSB
	case 1:
	{
	    uint32_t data = loadMemory(Byte | Nonsequential | Signed, base_addr);
	    setReg(dst_reg, data);
	}
	break;
	// LDRH
	case 2:
	{
	    uint32_t data = loadMemory(Word | Nonsequential, base_addr);
	    setReg(dst_reg, data);
	}
	break;
	// LDSH
	case 3:
	{
	    uint32_t data = loadMemory(Word | Nonsequential | Signed, base_addr);
	    setReg(dst_reg, data);
	}
	break;
	default:
	{
	    cout << "Unrecognized THUMB load/store signed byte/word opcode of " << dec << int(opcode) << endl;
	    exit(0);
	}
	break;
    }
}

void thumb_load_store_imm(uint16_t instr)
{
    int dst_reg = (instr & 0x7);
    int src_reg = ((instr >> 3) & 0x7);

    int offset = ((instr >> 6) & 0x1F);

    bool is_load = testbit(instr, 11);
    bool is_byte = testbit(instr, 12);

    uint32_t src_addr = getReg(src_reg);

    if (is_byte)
    {
	src_addr += offset;
    }
    else
    {
	src_addr += (offset << 2);
    }

    int flags = Nonsequential;

    if (is_load)
    {
	flags |= (is_byte) ? Byte : Long;
	setReg(dst_reg, loadMemory(flags, src_addr));
    }
    else
    {
	flags |= (is_byte) ? Byte : Long;
	storeMemory(flags, src_addr, getReg(dst_reg));
    }
}

void thumb_load_store_half(uint16_t instr)
{
    int dst_reg = (instr & 0x7);
    int src_reg = ((instr >> 3) & 0x7);

    int offset = ((instr >> 6) & 0x1F);

    bool is_load = testbit(instr, 11);

    uint32_t src_addr = (getReg(src_reg) + (offset << 1));

    if (is_load)
    {
	setReg(dst_reg, loadMemory(Word | Nonsequential, src_addr));
    }
    else
    {
	storeMemory(Word | Nonsequential, src_addr, getReg(dst_reg));
    }
}

void thumb_load_store_sp(uint16_t instr)
{
    bool is_load = testbit(instr, 11);
    int base_reg = ((instr >> 8) & 0x7);
    uint32_t offs = (instr & 0xFF);
    uint32_t mem_addr = (getReg(13) + (offs * 4));

    if (is_load)
    {
	setReg(base_reg, loadMemory(Long | Nonsequential, mem_addr));
    }
    else
    {
	storeMemory(Long | Nonsequential, mem_addr, getReg(base_reg));
    }
}

void thumb_get_rel_addr(uint16_t instr)
{
    bool is_sp = testbit(instr, 11);
    int base_reg = ((instr >> 8) & 0x7);
    uint32_t offs = (instr & 0xFF);

    uint32_t base_addr = 0;

    if (is_sp)
    {
	base_addr = getReg(13);
    }
    else
    {
	base_addr = (getReg(15) & ~3);
    }

    base_addr += (offs * 4);
    setReg(base_reg, base_addr);
}

void thumb_add_sp(uint16_t instr)
{
    bool is_sub = testbit(instr, 7);

    uint32_t offs = ((instr & 0x7F) * 4);
    uint32_t sp = getReg(13);

    if (is_sub)
    {
	sp -= offs;
    }
    else
    {
	sp += offs;
    }

    setReg(13, sp);
}

void thumb_push_pop(uint16_t instr)
{
    if (instr == 0xC306)
    {
	debugoutput();
    }

    bool is_pop = testbit(instr, 11);
    bool is_pc_lr = testbit(instr, 8);
    uint8_t rlist = (instr & 0xFF);

    // TODO: Implement edge case for empty register list
    if ((rlist == 0) && !is_pc_lr)
    {
	uint32_t base_addr = getReg(13);
	if (isVersionLessThanOrEqual(4))
	{
	    if (is_pop)
	    {
		setReg(15, readMultiple(Long | Nonsequential, base_addr));
	    }
	    else
	    {
		writeMultiple(Long | Nonsequential, base_addr, getReg(15));
	    }
	}

	setReg(13, (base_addr + 0x40));
	return;
    }

    if (is_pop)
    {
	uint32_t base_addr = getReg(13);
	int flags = Nonsequential;

	for (int reg = 0; reg < 8; reg++)
	{
	    if (!testbit(rlist, reg))
	    {
		continue;
	    }

	    setReg(reg, readMultiple(Long | flags, base_addr));
	    flags = Sequential;
	    base_addr += 4;
	}

	if (is_pc_lr)
	{
	    setReg(15, readMultiple(Long | flags, base_addr));
	    pipeline.is_reload = true;
	    base_addr += 4;
	}

	setReg(13, base_addr);
	idleCycle();
    }
    else
    {
	int count = 0;

	for (int reg = 0; reg < 8; reg++)
	{
	    if (testbit(rlist, reg))
	    {
		count += 1;
	    }
	}

	if (is_pc_lr)
	{
	    count += 1;
	}

	uint32_t base_addr = (getReg(13) - (count * 4));
	setReg(13, base_addr);

	int flags = Nonsequential;

	for (int reg = 0; reg < 8; reg++)
	{
	    if (!testbit(rlist, reg))
	    {
		continue;
	    }

	    writeMultiple(Long | flags, base_addr, getReg(reg));
	    flags = Sequential;
	    base_addr += 4;
	}

	if (is_pc_lr)
	{
	    writeMultiple(Long | flags, base_addr, getReg(14));
	    base_addr += 4;
	}

	pipeline.is_ns = true;
    }
}

void thumb_load_store_multiple(uint16_t instr)
{
    bool is_load = testbit(instr, 11);
    int base_reg = ((instr >> 8) & 0x7);
    uint8_t rlist = (instr & 0xFF);

    // TODO: Implement edge case for empty register list
    if (rlist == 0)
    {
	uint32_t base_addr = getReg(base_reg);
	if (isVersionLessThanOrEqual(4))
	{
	    if (is_load)
	    {
		setReg(15, readMultiple(Long | Nonsequential, base_addr));
		pipeline.is_reload = true;
	    }
	    else
	    {
		writeMultiple(Long | Nonsequential, base_addr, (getReg(15) + 2));
	    }
	}

	setReg(base_reg, (base_addr + 0x40));
	return;
    }

    if (is_load)
    {
	uint32_t base_addr = getReg(base_reg);

	int flags = Nonsequential;

	for (int reg = 0; reg < 8; reg++)
	{
	    if (!testbit(rlist, reg))
	    {
		continue;
	    }

	    setReg(reg, readMultiple(Long | flags, base_addr));
	    flags = Sequential;
	    base_addr += 4;
	}

	if (!testbit(rlist, base_reg))
	{
	    setReg(base_reg, base_addr);
	}

	idleCycle();
    }
    else
    {
	int count = 0;
	int first = 0;

	for (int reg = 7; reg >= 0; reg--)
	{
	    if (testbit(rlist, reg))
	    {
		count += 1;
		first = reg;
	    }
	}

	uint32_t base_addr = getReg(base_reg);
	uint32_t wb_addr = (base_addr + (count * 4));

	// Write first register in list
	writeMultiple(Long | Nonsequential, base_addr, getReg(first));
	base_addr += 4;

	// Writeback logic for ARMv4 (according to GBATEK):
	// Store OLD base if Rb is FIRST entry in Rlist, otherwise store NEW base.
	if (isVersion(4))
	{
	    setReg(base_reg, wb_addr);
	}

	// Store other registers
	for (int reg = (first + 1); reg < 8; reg++)
	{
	    if (!testbit(rlist, reg))
	    {
		continue;
	    }

	    writeMultiple(Long | Sequential, base_addr, getReg(reg));
	    base_addr += 4;
	}

	// Writeback logic for ARMv5+ (according to GBATEK):
	// Always store OLD base.
	// TODO: Confirm this specific behavior
	if (isNotVersion(4))
	{
	    setReg(base_reg, base_addr);
	}
    }
}

void thumb_cond_branch(uint16_t instr)
{
    int cond = ((instr >> 8) & 0xFF);
    uint32_t imm = (instr & 0xFF);

    if (testbit(imm, 7))
    {
	imm |= 0xFFFFFF00;
    }

    if (isCondition(cond))
    {
	uint32_t offs = (imm * 2);
	setReg(15, (getReg(15) + offs));
	pipeline.is_reload = true;
    }
}

void thumb_swi(uint16_t instr)
{
    uint8_t comment = (instr & 0xFF);

    if (isSWIOverride(comment, true))
    {
	softwareInterruptHLE(comment, true);
	return;
    }

    throwException(Svc, 0x8);
}

void thumb_branch(uint16_t instr)
{
    uint32_t imm = (instr & 0x7FF);

    if (testbit(instr, 10))
    {
	imm |= 0xFFFFF800;
    }

    uint32_t offs = (imm * 2);
    setReg(15, (getReg(15) + offs));
    pipeline.is_reload = true;
}

void thumb_long_branch(uint16_t instr)
{
    bool is_first_instr = (((instr >> 11) & 0x1F) == 0x1E);
    bool is_blx_instr = (((instr >> 11) & 0x1F) == 0x1D);

    if (is_first_instr)
    {
	uint32_t link_offs = (instr & 0x7FF);

	if (testbit(instr, 10))
	{
	    link_offs |= 0xFFFFF800;
	}

	uint32_t pc_reg = getReg(15);
	uint32_t link_val = (pc_reg + (link_offs << 12));
	setReg(14, link_val);
    }
    else
    {
	uint32_t link_offs = (instr & 0x7FF);
	uint32_t link_val = (getReg(14) + (link_offs << 1));
	uint32_t ret_addr = setbit(pipeline.decode.addr, 0);
	setReg(15, link_val);
	setReg(14, ret_addr);
	pipeline.is_reload = true;

	if (is_blx_instr && isVersionGreaterThanOrEqual(5))
	{
	    setThumb(false);
	}
    }
}