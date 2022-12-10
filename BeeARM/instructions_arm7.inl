void arm7_branch(uint32_t instr)
{
    int cond = ((instr >> 28) & 0xF);

    uint32_t offs = ((instr & 0xFFFFFF) << 2);

    if (testbit(instr, 23))
    {
	offs |= 0xFC000000;
    }

    if (cond == 0xF)
    {
	if (isVersionLessThanOrEqual(4))
	{
	    arm7_unknown(instr);
	    return;
	}

	offs += (testbit(instr, 24) << 1);

	setReg(14, (getReg(15) - 4));
	uint32_t r15 = getReg(15);
	setReg(15, (r15 + offs));
	setThumb(true);
	pipeline.is_reload = true;
    }
    else
    {
	bool is_link = testbit(instr, 24);

	if (is_link)
	{
	    setReg(14, (getReg(15) - 4));
	}

	uint32_t r15 = getReg(15);
	setReg(15, (r15 + offs));
	pipeline.is_reload = true;
    }
}

void arm7_bx(uint32_t instr)
{
    // BX is not available on ARMv3 or non-T versions of ARMv4
    if (!isBXSupported())
    {
	throw BeeARM7BXUnsupported();
	return;
    }

    int reg = (instr & 0xF);
    int opcode = ((instr >> 4) & 0xF);

    switch (opcode)
    {
	case 0x1:
	{
	    uint32_t addr = getReg(reg);
	    setReg(15, resetbit(addr, 0));
	    setThumb(testbit(addr, 0));
	    pipeline.is_reload = true;
	}
	break;
	case 0x3:
	{
	    // BLX reg is not supported on ARM v4 or lower
	    if (isVersionLessThan(5))
	    {
		arm7_unknown(instr);
		return;
	    }

	    setReg(14, (getReg(15) - 4));
	    uint32_t addr = getReg(reg);
	    setReg(15, resetbit(addr, 0));
	    setThumb(testbit(addr, 0));
	    pipeline.is_reload = true;
	}
	break;
	default:
	{
	    cout << "Unrecognized BX opcode of " << dec << int(opcode) << endl;
	    arm7_unknown(instr);
	}
	break;
    }
}

void arm7_data_proc(uint32_t instr)
{
    bool use_imm = testbit(instr, 25);
    bool set_cond = testbit(instr, 20);

    int opcode = ((instr >> 21) & 0xF);
    int src1_reg = ((instr >> 16) & 0xF);
    int dst_reg = ((instr >> 12) & 0xF);

    uint32_t src1_param = getReg(src1_reg);
    uint32_t src2_param = 0;

    bool is_carry = isCarry();

    if (use_imm)
    {
	uint16_t imm = (instr & 0xFF);
	int rot_shift = ((instr >> 8) & 0xF);

	src2_param = rotateRightImm(imm, rot_shift, is_carry);
    }
    else
    {
	bool use_shift_reg = testbit(instr, 4);
	int shift_type = ((instr >> 5) & 0x3);
	int src2_reg = (instr & 0xF);
	int shift_operand = 0;

	if (use_shift_reg)
	{
	    int shift_reg = ((instr >> 8) & 0xF);
	    uint32_t shift_param = getReg(shift_reg);
	    shift_operand = (shift_param & 0xFF);
	    idleCycle();
	}
	else
	{
	    shift_operand = ((instr >> 7) & 0x1F);
	}

	uint32_t src2_operand = getReg(src2_reg);

	if (use_shift_reg && (src2_reg == 15))
	{
	    src2_operand += 4;
	}

	if (use_shift_reg && (src1_reg == 15))
	{
	    src1_param += 4;
	}

	switch (shift_type)
	{
	    case 0: src2_param = logicalShiftLeft(src2_operand, shift_operand, is_carry); break;
	    case 1: src2_param = logicalShiftRight(src2_operand, shift_operand, is_carry, !use_shift_reg); break;
	    case 2: src2_param = arithmeticShiftRight(src2_operand, shift_operand, is_carry, !use_shift_reg); break;
	    case 3: src2_param = rotateRight(src2_operand, shift_operand, is_carry, !use_shift_reg); break;
	}
    }

    switch (opcode)
    {
	// AND
	case 0x0:
	{
	    uint32_t result = (src1_param & src2_param);

	    if (set_cond)
	    {
		setNZ(result);
		setCarry(is_carry);
	    }

	    setReg(dst_reg, result);
	}
	break;
	// EOR
	case 0x1:
	{
	    uint32_t result = (src1_param ^ src2_param);

	    if (set_cond)
	    {
		setNZ(result);
		setCarry(is_carry);
	    }

	    setReg(dst_reg, result);
	}
	break;
	// SUB
	case 0x2:
	{
	    uint32_t result = subOperand(src1_param, src2_param, set_cond);
	    setReg(dst_reg, result);
	}
	break;
	// RSB
	case 0x3:
	{
	    uint32_t result = subOperand(src2_param, src1_param, set_cond);
	    setReg(dst_reg, result);
	}
	break;
	// ADD
	case 0x4:
	{
	    uint32_t result = addOperand(src1_param, src2_param, set_cond);
	    setReg(dst_reg, result);
	}
	break;
	// ADC
	case 0x5:
	{
	    uint32_t result = adcOperand(src1_param, src2_param, set_cond);
	    setReg(dst_reg, result);
	}
	break;
	// SBC
	case 0x6:
	{
	    uint32_t result = sbcOperand(src1_param, src2_param, set_cond);
	    setReg(dst_reg, result);
	}
	break;
	// RSC
	case 0x7:
	{
	    uint32_t result = sbcOperand(src2_param, src1_param, set_cond);
	    setReg(dst_reg, result);
	}
	break;
	// TST
	case 0x8:
	{
	    uint32_t result = (src1_param & src2_param);
	    setNZ(result);
	    setCarry(is_carry);
	}
	break;
	// TEQ
	case 0x9:
	{
	    uint32_t result = (src1_param ^ src2_param);
	    setNZ(result);
	    setCarry(is_carry);
	}
	break;
	// CMP
	case 0xA:
	{
	    cmpOperand(src1_param, src2_param);
	}
	break;
	// CMN
	case 0xB:
	{
	    cmnOperand(src1_param, src2_param);
	}
	break;
	// ORR
	case 0xC:
	{
	    uint32_t result = (src1_param | src2_param);

	    if (set_cond)
	    {
		setNZ(result);
		setCarry(is_carry);
	    }

	    setReg(dst_reg, result);
	}
	break;
	// MOV
	case 0xD:
	{
	    if (set_cond)
	    {
		setNZ(src2_param);
		setCarry(is_carry);
	    }

	    setReg(dst_reg, src2_param);
	}
	break;
	// BIC
	case 0xE:
	{
	    uint32_t result = (src1_param & ~src2_param);

	    if (set_cond)
	    {
		setNZ(result);
		setCarry(is_carry);
	    }

	    setReg(dst_reg, result);
	}
	break;
	// MVN
	case 0xF:
	{
	    uint32_t result = ~src2_param;
	    if (set_cond)
	    {
		setNZ(result);
		setCarry(is_carry);
	    }

	    setReg(dst_reg, result);
	}
	break;
	default:
	{
	    arm7_unknown(instr);
	}
	break;
    }

    if (dst_reg == 15)
    {
	if (set_cond)
	{
	    setCPSR(getSPSR());
	}

	if ((opcode < 0x8) || (opcode > 0x0B))
	{
	    pipeline.is_reload = true;
	}
    }
}

void arm7_mrs_msr(uint32_t instr)
{
    bool use_imm = testbit(instr, 25);
    bool is_spsr = testbit(instr, 22);
    bool opcode = testbit(instr, 21);

    uint32_t source_param = 0;
    bool is_carry = false;

    uint32_t msr_mask = 0;

    if (testbit(instr, 19))
    {
	msr_mask |= 0xFF000000;
    }

    if (testbit(instr, 18))
    {
	msr_mask |= 0xFF0000;
    }

    if (testbit(instr, 17))
    {
	msr_mask |= 0xFF00;
    }

    if (testbit(instr, 16))
    {
	msr_mask |= 0xFF;
    }

    if (opcode)
    {
	if (use_imm)
	{
	    uint32_t imm = (instr & 0xFF);
	    int offs = ((instr >> 8) & 0xF);
	    source_param = rotateRightImm(imm, offs, is_carry);
	}
	else
	{
	    int src_reg = (instr & 0xF);
	    source_param = getReg(src_reg);
	}

	if (is_spsr)
	{
	    uint32_t spsr_reg = getSPSR();
	    spsr_reg = ((spsr_reg & ~msr_mask) | (source_param & msr_mask));
	    setSPSR(spsr_reg);
	}
	else
	{
	    uint32_t cpsr_reg = getCPSR();
	    cpsr_reg = ((cpsr_reg & ~msr_mask) | (source_param & msr_mask));
	    setCPSR(cpsr_reg);
	}
    }
    else
    {
	int dst_reg = ((instr >> 12) & 0xF);

	uint32_t status_reg = (is_spsr) ? getSPSR() : getCPSR();
	setReg(dst_reg, status_reg);

	if (dst_reg == 15)
	{
	    pipeline.is_reload = true;
	}
    }
}

void arm7_mul_mla(uint32_t instr)
{
    bool is_accum = testbit(instr, 21);
    bool set_cond = testbit(instr, 20);
    int dst_reg = ((instr >> 16) & 0xF);
    int accum_reg = ((instr >> 12) & 0xF);
    int src_reg = ((instr >> 8) & 0xF);
    int mul_reg = (instr & 0xF);

    uint32_t accum_param = getReg(accum_reg);
    uint32_t src_param = getReg(src_reg);
    uint32_t mul_param = getReg(mul_reg);

    uint32_t result = (mul_param * src_param);

    multiplyCycles(src_param);

    if (is_accum)
    {
	result += accum_param;
	idleCycle();
    }

    if (set_cond)
    {
	setNZ(result);
    }

    setReg(dst_reg, result);

    if (dst_reg == 15)
    {
	pipeline.is_reload = true;
    }
}

void arm7_mul_mla_long(uint32_t instr)
{
    if (!isMultLongSupported())
    {
	throw BeeARM7MultLongUnsupported();
	return;
    }

    bool is_signed = testbit(instr, 22);
    bool is_accum = testbit(instr, 21);
    bool set_cond = testbit(instr, 20);

    int dst_reg_hi = ((instr >> 16) & 0xF);
    int dst_reg_lo = ((instr >> 12) & 0xF);
    int src_reg = ((instr >> 8) & 0xF);
    int mul_reg = (instr & 0xF);

    uint32_t src_param = getReg(src_reg);
    uint32_t mul_param = getReg(mul_reg);

    int64_t result = 0;

    if (is_signed)
    {
	result = (int64_t(int32_t(mul_param)) * int64_t(int32_t(src_param)));
    }
    else
    {
	result = (uint64_t(mul_param) * uint64_t(src_param));
    }

    multiplyCycles(src_param, is_signed);
    idleCycle();

    if (is_accum)
    {
	int64_t accum_value = getReg(dst_reg_hi);
	accum_value <<= 16;
	accum_value <<= 16;
	accum_value |= getReg(dst_reg_lo);
	result += accum_value;
	idleCycle();
    }

    uint32_t res_hi = (result >> 32);

    if (set_cond)
    {
	setSign(testbit(res_hi, 31));
	setZero(result == 0);
    }

    setReg(dst_reg_lo, uint32_t(result));
    setReg(dst_reg_hi, res_hi);

    if ((dst_reg_lo == 15) || (dst_reg_hi == 15))
    {
	pipeline.is_reload = true;
    }
}

void arm7_half_transfer_reg(uint32_t instr)
{
    // Halfword transfers are not supported on ARMv3 or lower
    if (isVersionLessThan(4))
    {
	throw new BeeARM7HalfTransferUnsupported();
	return;
    }

    bool is_pre_index = testbit(instr, 24);
    bool is_offset_add = testbit(instr, 23);
    bool is_write_back = testbit(instr, 21);
    bool is_load = testbit(instr, 20);

    int base_reg = ((instr >> 16) & 0xF);
    int dest_reg = ((instr >> 12) & 0xF);

    int opcode = ((instr >> 5) & 0x3);
    int offs_reg = (instr & 0xF);

    uint32_t offs = getReg(offs_reg);

    uint32_t base_addr = getReg(base_reg);

    if (!is_offset_add)
    {
	offs = -offs;
    }

    if (is_pre_index)
    {
	base_addr += offs;
    }

    switch (opcode)
    {
	case 0: break;
	case 1:
	{
	    if (is_load)
	    {
		uint32_t data = loadMemory(Word | Nonsequential, base_addr);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}

		setReg(dest_reg, data);
	    }
	    else
	    {
		uint32_t data = getReg(dest_reg);

		if (dest_reg == 15)
		{
		    data += 4;
		}

		storeMemory(Word | Nonsequential, base_addr, data);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}
	    }
	}
	break;
	case 2:
	{
	    if (is_load)
	    {
		uint32_t data = loadMemory(Byte | Signed | Nonsequential, base_addr);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}

		setReg(dest_reg, data);
	    }
	    else
	    {
		if (isVersion(4))
		{
		    idleCycle();

		    if (!is_pre_index || is_write_back)
		    {
			setReg(base_reg, (getReg(base_reg) + offs));
		    }

		    idleCycle();
		}
		else
		{
		    arm7_unknown(instr);
		}
	    }
	}
	break;
	case 3:
	{
	    if (is_load)
	    {
		uint32_t data = loadMemory(Word | Signed | Nonsequential, base_addr);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}

		setReg(dest_reg, data);
	    }
	    else
	    {
		if (isVersion(4))
		{
		    idleCycle();

		    if (!is_pre_index || is_write_back)
		    {
			setReg(base_reg, (getReg(base_reg) + offs));
		    }
		}
		else
		{
		    arm7_unknown(instr);
		}
	    }
	}
	break;
    }

    if (is_load && (dest_reg == 15))
    {
	pipeline.is_reload = true;
    }
}

void arm7_half_transfer_imm(uint32_t instr)
{
    // Halfword transfers are not supported on ARMv3 or lower
    if (isVersionLessThan(4))
    {
	arm7_unknown(instr);
	return;
    }

    bool is_pre_index = testbit(instr, 24);
    bool is_offset_add = testbit(instr, 23);
    bool is_write_back = testbit(instr, 21);
    bool is_load = testbit(instr, 20);

    int base_reg = ((instr >> 16) & 0xF);
    int dest_reg = ((instr >> 12) & 0xF);

    int opcode = ((instr >> 5) & 0x3);

    int offs_msb = ((instr >> 8) & 0xF);
    int offs_lsb = (instr & 0xF);

    uint32_t offs = ((offs_msb << 4) | offs_lsb);

    uint32_t base_addr = getReg(base_reg);

    if (!is_offset_add)
    {
	offs = -offs;
    }

    if (is_pre_index)
    {
	base_addr += offs;
    }

    switch (opcode)
    {
	case 0: break;
	case 1:
	{
	    if (is_load)
	    {
		uint32_t data = loadMemory(Word | Nonsequential, base_addr);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}

		setReg(dest_reg, data);
	    }
	    else
	    {
		uint32_t data = getReg(dest_reg);

		if (dest_reg == 15)
		{
		    data += 4;
		}

		storeMemory(Word | Nonsequential, base_addr, data);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}
	    }
	}
	break;
	case 2:
	{
	    if (is_load)
	    {
		uint32_t data = loadMemory(Byte | Signed | Nonsequential, base_addr);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}

		setReg(dest_reg, data);
	    }
	    else
	    {
		if (isVersion(4))
		{
		    idleCycle();

		    if (!is_pre_index || is_write_back)
		    {
			setReg(base_reg, (getReg(base_reg) + offs));
		    }

		    idleCycle();
		}
		else
		{
		    arm7_unknown(instr);
		}
	    }
	}
	break;
	case 3:
	{
	    if (is_load)
	    {
		uint32_t data = loadMemory(Word | Signed | Nonsequential, base_addr);

		if (!is_pre_index || is_write_back)
		{
		    setReg(base_reg, (getReg(base_reg) + offs));
		}

		setReg(dest_reg, data);
	    }
	    else
	    {
		if (isVersion(4))
		{
		    idleCycle();

		    if (!is_pre_index || is_write_back)
		    {
			setReg(base_reg, (getReg(base_reg) + offs));
		    }
		}
		else
		{
		    arm7_unknown(instr);
		}
	    }
	}
	break;
    }

    if (is_load && (dest_reg == 15))
    {
	pipeline.is_reload = true;
    }
}

void arm7_swap(uint32_t instr)
{
    bool is_byte = testbit(instr, 22);

    int base_reg = ((instr >> 16) & 0xF);
    int dst_reg = ((instr >> 12) & 0xF);
    int src_reg = (instr & 0xF);

    uint32_t base_param = getReg(base_reg);
    uint32_t src_param = getReg(src_reg);

    if (base_reg == 15)
    {
	base_param += 4;
    }

    if (src_reg == 15)
    {
	src_param += 4;
    }

    int flags = (is_byte) ? Byte : Long;

    uint32_t dst_data = loadMemory(flags | Nonsequential, base_param);
    storeMemory(flags | Nonsequential, base_param, src_param);

    setReg(dst_reg, dst_data);

    if (dst_reg == 15)
    {
	pipeline.is_reload = true;
    }
}

void arm7_single_transfer(uint32_t instr)
{
    bool use_reg = testbit(instr, 25);
    bool is_pre_index = testbit(instr, 24);
    bool is_offset_add = testbit(instr, 23);
    bool is_byte = testbit(instr, 22);
    bool is_write_back = testbit(instr, 21);
    bool is_load = testbit(instr, 20);

    int base_reg = ((instr >> 16) & 0xF);
    int dst_reg = ((instr >> 12) & 0xF);

    uint32_t offset_param = 0;
    bool is_carry = isCarry();

    if (use_reg)
    {
	int offs_reg = (instr & 0xF);
	int shift_type = ((instr >> 5) & 0x3);
	int shift_offs = ((instr >> 7) & 0x1F);

	uint32_t offset_reg = getReg(offs_reg);

	switch (shift_type)
	{
	    case 0: offset_param = logicalShiftLeft(offset_reg, shift_offs, is_carry); break;
	    case 1: offset_param = logicalShiftRight(offset_reg, shift_offs, is_carry, true); break;
	    case 2: offset_param = arithmeticShiftRight(offset_reg, shift_offs, is_carry, true); break;
	    case 3: offset_param = rotateRight(offset_reg, shift_offs, is_carry, true); break;
	}
    }
    else
    {
	offset_param = (instr & 0xFFF);
    }
    

    uint32_t address = getReg(base_reg);

    if (!is_offset_add)
    {
	offset_param = -offset_param;
    }

    if (is_pre_index)
    {
	address += offset_param;
    }

    int flags = Nonsequential;
    flags |= (is_byte) ? Byte : Long;

    if (is_load)
    {
	uint32_t data = loadMemory(flags, address);

	if (is_write_back || !is_pre_index)
	{
	    setReg(base_reg, (getReg(base_reg) + offset_param));
	}

	setReg(dst_reg, data);
    }
    else
    {
	uint32_t data = getReg(dst_reg);

	if (dst_reg == 15)
	{
	    data += 4;
	}

	storeMemory(flags, address, data);

	if (is_write_back || !is_pre_index)
	{
	    setReg(base_reg, (getReg(base_reg) + offset_param));
	}
    }

    if (is_load && (dst_reg == 15))
    {
	if (isVersionGreaterThanOrEqual(5))
	{
	    cout << "Possible switch to THUMB mode..." << endl;
	    exit(0);
	}

	pipeline.is_reload = true;
    }
}

void arm7_block_transfer(uint32_t instr)
{
    bool is_pre_index = testbit(instr, 24);
    bool is_offset_add = testbit(instr, 23);
    bool is_psr_mode = testbit(instr, 22);
    bool is_writeback = testbit(instr, 21);
    bool is_load = testbit(instr, 20);
    int base_reg = ((instr >> 16) & 0xF);
    uint16_t rlist = (instr & 0xFFFF);

    bool is_transfer_pc = testbit(rlist, 15);

    uint32_t address = getReg(base_reg);

    int first = 0;
    int count = 0;

    if (rlist != 0)
    {
	for (int reg = 15; reg >= 0; reg--)
	{
	    if (testbit(rlist, reg))
	    {
		count += 1;
		first = reg;
	    }
	}
    }
    else
    {
	rlist = isVersionLessThanOrEqual(4) ? (1 << 15) : 0;
	first = 15;
	is_transfer_pc = true;
	count = 16;
    }

    bool is_switch_mode = (is_psr_mode && (!is_load || !is_transfer_pc));

    BeeARM7Mode prev_mode;

    if (is_switch_mode)
    {
	prev_mode = current_mode;
	setMode(Usr);
    }

    uint32_t base_new = address;
    // uint32_t base_old = address;

    bool is_pre_dec = is_pre_index;

    if (!is_offset_add)
    {
	is_pre_dec = !is_pre_dec;
	address -= (count * 4);
	base_new -= (count * 4);
    }
    else
    {
	base_new += (count * 4);
    }

    int flags = Nonsequential;

    for (int i = first; i < 16; i++)
    {
	if (!testbit(rlist, i))
	{
	    continue;
	}

	if (is_pre_dec)
	{
	    address += 4;
	}

	if (is_load)
	{
	    uint32_t value = readMultiple(Long | flags, address);

	    if (is_writeback && (i == first))
	    {
		setReg(base_reg, base_new);
	    }

	    setReg(i, value);
	}
	else
	{
	    uint32_t data = getReg(i);

	    if (i == 15)
	    {
		data += 4;
	    }

	    writeMultiple(Long | flags, address, data);

	    if (is_writeback && (i == first))
	    {
		setReg(base_reg, base_new);
	    }
	}

	if (!is_pre_dec)
	{
	    address += 4;
	}

	flags = Sequential;
    }

    if (is_load)
    {
	idleCycle();
	if (is_transfer_pc)
	{
	    if (is_psr_mode)
	    {
		setCPSR(getSPSR());
	    }

	    pipeline.is_reload = true;
	}
    }

    if (is_switch_mode)
    {
	setMode(prev_mode);
    }
}

void arm7_coproc_reg_transfer(uint32_t instr)
{
    int cond = (instr >> 28);

    if (cond == 0xF)
    {
	cout << "MRC2/MCR2 opcode" << endl;
	exit(0);
    }

    bool is_load = testbit(instr, 20);
    int coproc_op = ((instr >> 21) & 0x7);
    int coproc_number = ((instr >> 8) & 0xF);
    int coproc_reg = ((instr >> 16) & 0xF);
    int arm_reg = ((instr >> 12) & 0xF);
    int coproc_info = ((instr >> 5) & 0x7);
    int coproc_oper = (instr & 0xF);

    if (coprocs.at(coproc_number) == NULL)
    {
	cout << "Invalid coprocessor" << endl;
	exit(0);
    }

    uint32_t coproc_id = ((coproc_op << 12) | (coproc_reg << 8) | (coproc_oper << 4) | coproc_info);

    if (is_load)
    {
	setReg(arm_reg, coprocs.at(coproc_number)->readReg(coproc_id));
    }
    else
    {
	uint32_t data = getReg(arm_reg);
	coprocs.at(coproc_number)->writeReg(coproc_id, data);
    }
}

void arm7_swi(uint32_t instr)
{
    uint32_t comment = (instr & 0xFFFFFF);

    if (isSWIOverride(comment, false))
    {
	softwareInterruptHLE(comment, false);
	return;
    }

    throwException(Svc, 0x8);
}