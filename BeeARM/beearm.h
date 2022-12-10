#ifndef BEEARM_H
#define BEEARM_H

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <functional>
using namespace std;
using namespace std::placeholders;

namespace beearm
{
    enum BeeARMFlags : int
    {
	Nonsequential = (1 << 0), // N cycle
	Sequential = (1 << 1), // S cycle
	Prefetch = (1 << 2), // Instruction fetch
	Byte = (1 << 3), // 8-bit access
	Word = (1 << 4), // 16-bit access
	Long = (1 << 5), // 32-bit access
	Load = (1 << 6), // Load operation
	Store = (1 << 7), // Store operation
	Signed = (1 << 8), // Sign-extend
    };

    class BeeARMUnknownInstruction : public exception
    {
	public:
	    BeeARMUnknownInstruction(string id_str, uint32_t instr, int version = 0)
	    {
		stringstream ver_str;
		ver_str << id_str;

		if (version > 0)
		{
		    ver_str << " v" << dec << version;
		}

		stringstream ss;
		ss << "Unrecognized " << ver_str.str() << " instruction of " << hex << int(instr);
		message = ss.str();
	    }

	    virtual const char *what() const noexcept override
	    {
		return message.c_str();
	    }

	private:
	    string message;
    };

    class BeeARMInterface
    {
	public:
	    BeeARMInterface()
	    {

	    }

	    ~BeeARMInterface()
	    {

	    }

	    virtual uint32_t readMemory(int flags, uint32_t addr)
	    {
		cout << "Memory read: " << endl;
		cout << "Flags: " << hex << int(flags) << endl;
		cout << "Address: " << hex << int(addr) << endl;
		exit(0);
		return 0;
	    }

	    virtual void writeMemory(int flags, uint32_t addr, uint32_t data)
	    {
		cout << "Memory write: " << endl;
		cout << "Flags: " << hex << int(flags) << endl;
		cout << "Address: " << hex << int(addr) << endl;
		cout << "Data: " << hex << int(data) << endl;
		exit(0);
	    }

	    virtual void idleCycle()
	    {
		return;
	    }

	    virtual bool isSWIOverride(uint32_t comment, bool is_thumb)
	    {
		(void)comment;
		(void)is_thumb;
		return false;
	    }

	    virtual void softwareInterrupt(uint32_t comment, bool is_thumb)
	    {
		(void)is_thumb;
		cout << "SWI #" << hex << int(comment) << endl;
		exit(0);
	    }
    };

    enum BeeARM7Mode : int
    {
	Usr = 0x10,
	Fiq = 0x11,
	Irq = 0x12,
	Svc = 0x13,
	Abt = 0x17,
	Und = 0x1B,
	Sys = 0x1F
    };

    class BeeARM
    {
	public:
	    BeeARM();
	    ~BeeARM();

	    void init();
	    void shutdown();
	    void setInterface(BeeARMInterface &cb);

	    uint32_t getReg(int reg);
	    void setReg(int reg, uint32_t data);

	    uint32_t getSPSR();
	    void setSPSR(uint32_t data);

	    void runInstruction();

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 1) ? true : false;
	    }

	    void fetch();

	    uint32_t readInstruction(int size, int sequential, uint32_t addr)
	    {
		int mem_flags = (BeeARMFlags::Prefetch | size | sequential);
		return readMemory(mem_flags, addr);
	    }

	    uint32_t readPrefetch(int size, uint32_t addr)
	    {
		int flags = (BeeARMFlags::Prefetch | size | BeeARMFlags::Nonsequential);
		return readMemory(flags, addr);
	    }

	    uint32_t readMemory(int flags, uint32_t addr)
	    {
		if (inter)
		{
		    return inter->readMemory(flags, addr);
		}

		return 0;
	    }

	    struct BeeARMPipeline
	    {
		struct Instruction
		{
		    uint32_t addr = 0;
		    uint32_t instr = 0;
		    bool is_thumb = false;
		};

		bool is_reload = true;
		bool is_ns = true;

		Instruction fetch;
		Instruction decode;
		Instruction execute;
	    } pipeline;

	    uint32_t opcode = 0;

	    struct armregisters
	    {
		uint32_t r0 = 0;
		uint32_t r1 = 0;
		uint32_t r2 = 0;
		uint32_t r3 = 0;
		uint32_t r4 = 0;
		uint32_t r5 = 0;
		uint32_t r6 = 0;
		uint32_t r7 = 0;
		uint32_t r8 = 0;
		uint32_t r9 = 0;

		uint32_t r10 = 0;
		uint32_t r10_fiq = 0;

		uint32_t r11 = 0;
		uint32_t r11_fiq = 0;

		uint32_t r12 = 0;
		uint32_t r12_fiq = 0;

		uint32_t r13 = 0;
		uint32_t r13_fiq = 0;
		uint32_t r13_irq = 0;
		uint32_t r13_svc = 0;

		uint32_t r14 = 0;
		uint32_t r14_fiq = 0;
		uint32_t r14_irq = 0;
		uint32_t r14_svc = 0;

		uint32_t r15 = 0;
	    };

	    armregisters arm_regs;

	    unique_ptr<BeeARMInterface> inter;

	    using beearmfunc = function<void(uint32_t)>;
	    // using beearmdasmfunc = function<size_t(ostream&, uint32_t, uint32_t)>;

	    // #include "instructions.inl"

	    struct beeARMmapping
	    {
		uint32_t mask = 0;
		uint32_t value = 0;
		beearmfunc function;
	    };

	    vector<beeARMmapping> arm_func_table;

	    beeARMmapping decodeARMInstr(uint32_t instr);

	    #include "instr_tables_arm1.inl"
    };

    enum BeeARM7Architecture
    {
	v3G,
	v3M,
	v4,
	v4T,
	v4xM,
	v4TxM,
	v5,
	v5T,
	v5xM,
	v5TxM,
	v5E,
	v5TE,
	v5TExP,
	v5TEJ
    };

    class BeeARM7UnknownInstruction : public BeeARMUnknownInstruction
    {
	public:
	    BeeARM7UnknownInstruction(int version, uint32_t instr) : BeeARMUnknownInstruction("ARM", instr, version)
	    {

	    }
    };

    class BeeARM7ThumbUnknownInstruction : public BeeARMUnknownInstruction
    {
	public:
	    BeeARM7ThumbUnknownInstruction(int version, uint16_t instr) : BeeARMUnknownInstruction("THUMB", instr, version)
	    {

	    }
    };

    class BeeARM7UnsupportedFeature : public exception
    {
	public:
	    BeeARM7UnsupportedFeature(string feature_name)
	    {
		stringstream ss;
		ss << feature_name << " instructions are unsupported in this architecture version.";
		message = ss.str();
	    }

	    virtual const char *what() const noexcept override
	    {
		return message.c_str();
	    }

	private:
	    string message;
    };

    class BeeARM7MultLongUnsupported : public BeeARM7UnsupportedFeature
    {
	public:
	    BeeARM7MultLongUnsupported() : BeeARM7UnsupportedFeature("Multiply long")
	    {

	    }
    };

    class BeeARM7BXUnsupported : public BeeARM7UnsupportedFeature
    {
	public:
	    BeeARM7BXUnsupported() : BeeARM7UnsupportedFeature("Branch and exchange")
	    {

	    }
    };

    class BeeARM7HalfTransferUnsupported : public BeeARM7UnsupportedFeature
    {
	public:
	    BeeARM7HalfTransferUnsupported() : BeeARM7UnsupportedFeature("Halfword transfer")
	    {

	    }
    };

    class BeeARM7BLXUnsupported : public BeeARM7UnsupportedFeature
    {
	public:
	    BeeARM7BLXUnsupported() : BeeARM7UnsupportedFeature("Branch, link and exchange")
	    {

	    }
    };

    class BeeARM7Coprocessor
    {
	public:
	    BeeARM7Coprocessor()
	    {

	    }

	    ~BeeARM7Coprocessor()
	    {

	    }

	    virtual void reset()
	    {
		return;
	    }

	    virtual uint32_t readReg(uint32_t)
	    {
		return 0;
	    }

	    virtual void writeReg(uint32_t, uint32_t)
	    {
		return;
	    }
    };

    class BeeARM7
    {
	public:
	    BeeARM7();
	    ~BeeARM7();

	    void init();
	    void setArchitecture(string arch_name);
	    void initGBA();
	    void shutdown();
	    void setInterface(BeeARMInterface *cb);
	    void setCoprocessor(int num, BeeARM7Coprocessor *cb);

	    void setReg(BeeARM7Mode mode, int reg, uint32_t data);

	    uint32_t getReg(int reg);
	    void setReg(int reg, uint32_t data);

	    uint32_t getCPSR();
	    void setCPSR(uint32_t data);

	    uint32_t getSPSR();
	    void setSPSR(uint32_t data);

	    void runInstruction();
	    void debugoutput(bool print_dasm = false);

	    bool isIRQDisabled()
	    {
		return testbit(arm_regs.cpsr, 7);
	    }

	    bool isThumb()
	    {
		return (is_thumb_enabled && testbit(arm_regs.cpsr, 5));
	    }

	    void fireInterrupt(bool line = true);

	    uint32_t getFetchedOpcode(int slot)
	    {
		slot &= 1;

		if (slot == 0)
		{
		    return pipeline.decode.instr;
		}
		else
		{
		    return pipeline.fetch.instr;
		}
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 1) ? true : false;
	    }

	    template<typename T>
	    T setbit(T reg, int bit)
	    {
		return (reg | (1 << bit));
	    }

	    template<typename T>
	    T resetbit(T reg, int bit)
	    {
		return (reg & ~(1 << bit));
	    }

	    template<typename T>
	    T changebit(T reg, int bit, bool is_set)
	    {
		if (is_set)
		{
		    return setbit(reg, bit);
		}
		else
		{
		    return resetbit(reg, bit);
		}
	    }

	    array<unique_ptr<BeeARM7Coprocessor>, 16> coprocs;

	    unordered_map<string, BeeARM7Architecture> arch_strings = 
	    {
		{"armv3", v3G},
		{"armv3m", v3M},
		{"armv4", v4},
		{"armv4t", v4T},
		{"armv4xm", v4xM},
		{"armv4txm", v4TxM},
		{"armv5", v5},
		{"armv5t", v5T},
		{"armv5xm", v5xM},
		{"armv5txm", v5TxM},
		{"armv5e", v5E},
		{"armv5te", v5TE},
		{"armv5texp", v5TExP},
		{"armv5tej", v5TEJ},
	    };

	    void throwException(BeeARM7Mode mode, uint32_t address);

	    int getVersion()
	    {
		return version_num;
	    }

	    bool isVersion(int num)
	    {
		return (version_num == num);
	    }

	    bool isNotVersion(int num)
	    {
		return (version_num != num);
	    }

	    bool isVersionLessThan(int num)
	    {
		return (version_num < num);
	    }

	    bool isVersionLessThanOrEqual(int num)
	    {
		return (version_num <= num);
	    }

	    bool isVersionGreaterThan(int num)
	    {
		return (version_num > num);
	    }

	    bool isVersionGreaterThanOrEqual(int num)
	    {
		return (version_num >= num);
	    }

	    bool isThumbEnabled()
	    {
		return is_thumb_enabled;
	    }

	    bool isBXSupported()
	    {
		return is_bx_enabled;
	    }

	    bool isMultLongSupported()
	    {
		return is_mult_long_enabled;
	    }

	    bool is_bx_enabled = false;
	    int version_num = 0;

	    void setArchitecture(BeeARM7Architecture arch);

	    BeeARM7Architecture architecture;

	    void setNZ(uint32_t result)
	    {
		setSign(testbit(result, 31));
		setZero(result == 0);
	    }

	    bool isCondition(int cond)
	    {
		bool result = false;
		cond &= 0xF;
		switch (cond)
		{
		    case 0x0: result = isZero(); break; // EQ
		    case 0x1: result = !isZero(); break; // NE
		    case 0x2: result = isCarry(); break; // HS
		    case 0x3: result = !isCarry(); break; // LO
		    case 0x4: result = isSign(); break; // MI
		    case 0x5: result = !isSign(); break; // PL
		    case 0x6: result = isOverflow(); break; // VS
		    case 0x7: result = !isOverflow(); break; // VC
		    case 0x8: result = (isCarry() && !isZero()); break; // HI
		    case 0x9: result = (!isCarry() || isZero()); break; // LS
		    case 0xA: result = (isSign() == isOverflow()); break; // GE
		    case 0xB: result = (isSign() != isOverflow()); break; // LT
		    case 0xC: result = (!isZero() && (isSign() == isOverflow())); break; // GT
		    case 0xD: result = (isZero() || (isSign() != isOverflow())); break; // LE
		    case 0xE: result = true; break; // AL
		    case 0xF: result = true; break; // Reserved
		}

		return result;
	    }

	    uint32_t addInternal(uint32_t source1, uint32_t source2, bool is_carry, bool set_cond)
	    {
		// Flag calculations borrowed from ares-emu
		uint32_t result = (source1 + source2 + is_carry);

		uint32_t overflow = (~(source1 ^ source2) & (source1 ^ result));
		uint32_t carry = (overflow ^ source1 ^ source2 ^ result);

		if (set_cond == true)
		{
		    setNZ(result);
		    setCarry(testbit(carry, 31));
		    setOverflow(testbit(overflow, 31));
		}

		return result;
	    }

	    uint32_t subInternal(uint32_t source1, uint32_t source2, bool is_carry, bool set_cond)
	    {
		return addInternal(source1, ~source2, is_carry, set_cond);
	    }

	    uint32_t addOperand(uint32_t source1, uint32_t source2, bool set_cond = true)
	    {
		return addInternal(source1, source2, false, set_cond);
	    }

	    uint32_t subOperand(uint32_t source1, uint32_t source2, bool set_cond = true)
	    {
		return subInternal(source1, source2, true, set_cond);
	    }

	    uint32_t adcOperand(uint32_t source1, uint32_t source2, bool set_cond = true)
	    {
		return addInternal(source1, source2, isCarry(), set_cond);
	    }

	    uint32_t sbcOperand(uint32_t source1, uint32_t source2, bool set_cond = true)
	    {
		return subInternal(source1, source2, isCarry(), set_cond);
	    }

	    void cmpOperand(uint32_t source1, uint32_t source2, bool set_cond = true)
	    {
		subInternal(source1, source2, true, set_cond);
	    }

	    void cmnOperand(uint32_t source1, uint32_t source2, bool set_cond = true)
	    {
		addInternal(source1, source2, false, set_cond);
	    }

	    uint32_t negOperand(uint32_t source)
	    {
		return subInternal(0, source, true, true);
	    }

	    void fetch();

	    uint32_t readInstruction(int size, int sequential, uint32_t addr)
	    {
		int mem_flags = (Prefetch | size | sequential);
		return readMemory(mem_flags, addr);
	    }

	    uint32_t readPrefetch(int size, uint32_t addr)
	    {
		int flags = (Prefetch | size | Nonsequential);
		return readMemory(flags, addr);
	    }

	    uint32_t loadMemory(int mode, uint32_t addr)
	    {
		pipeline.is_ns = true;
		int flags = (Load | mode);

		int addr_bits = 0;

		if (flags & Long)
		{
		    addr_bits = (addr & 3);
		}
		else if (flags & Word)
		{
		    addr_bits = (addr & 1);
		}

		uint32_t data = readMemory(flags, addr);

		if ((flags & Word) != 0)
		{
		    if (flags & Signed)
		    {
			data = int16_t(data);
		    }
		    else
		    {
			data = uint16_t(data);
		    }
		} 

		if ((flags & Byte) != 0)
		{
		    if (flags & Signed)
		    {
			data = int8_t(data);
		    }
		    else
		    {
			data = uint8_t(data);
		    }
		}

		int shift = (addr_bits * 8);
		bool is_carry = false;

		if (shift != 0)
		{
		    if (flags & Signed)
		    {
			data = arithmeticShiftRight(data, shift, is_carry, true);
		    }
		    else
		    {
			data = rotateRight(data, shift, is_carry, true);
		    }
		}

		idleCycle();
		return data;
	    }

	    void storeMemory(int mode, uint32_t addr, uint32_t data)
	    {
		pipeline.is_ns = true;

		uint32_t data_u32 = data;

		int flags = (Store | mode);

		if (flags & Long)
		{
		    addr &= ~3;
		}
		else if (flags & Word)
		{
		    addr &= ~1;
		}

		if ((flags & Word) != 0)
		{
		    data_u32 &= 0xFFFF;
		    data_u32 |= (data_u32 << 16);
		}

		if ((flags & Byte) != 0)
		{
		    data_u32 &= 0xFF;
		    data_u32 |= (data_u32 << 8);
		    data_u32 |= (data_u32 << 16);
		}

		writeMemory(flags, addr, data_u32);
	    }

	    uint32_t readMultiple(int flags, uint32_t addr)
	    {
		return readMemory(flags, addr);
	    }

	    void writeMultiple(int flags, uint32_t addr, uint32_t data)
	    {
		pipeline.is_ns = true;
		writeMemory(flags, addr, data);
	    }

	    uint32_t readMemory(int flags, uint32_t addr)
	    {
		if (inter)
		{
		    return inter->readMemory(flags, addr);
		}

		return 0;
	    }

	    void writeMemory(int flags, uint32_t addr, uint32_t data)
	    {
		if (inter)
		{
		    inter->writeMemory(flags, addr, data);
		}
	    }

	    void idleCycle()
	    {
		pipeline.is_ns = true;

		if (inter)
		{
		    inter->idleCycle();
		}
	    }

	    bool isSWIOverride(uint32_t comment, bool is_thumb)
	    {
		if (inter)
		{
		    return inter->isSWIOverride(comment, is_thumb);
		}

		return false;
	    }

	    void softwareInterruptHLE(uint32_t comment, bool is_thumb)
	    {
		if (inter)
		{
		    inter->softwareInterrupt(comment, is_thumb);
		}
	    }

	    uint32_t logicalShiftLeft(uint32_t operand, int offs, bool &is_carry)
	    {
		if (offs == 0)
		{
		    return operand;
		}
		else if (offs >= 32)
		{
		    is_carry = (offs > 32) ? false : testbit(operand, 0);
		    return 0;
		}
		else
		{
		    for (int i = 0; i < offs; i++)
		    {
			is_carry = testbit(operand, 31);
			operand <<= 1;
		    }

		    return operand;
		}
	    }

	    uint32_t logicalShiftRight(uint32_t operand, int offs, bool &is_carry, bool use_imm)
	    {
		if (offs == 0)
		{
		    if (use_imm)
		    {
			offs = 32;
		    }
		    else
		    {
			return operand;
		    }
		}

		if (offs >= 32)
		{
		    is_carry = (offs > 32) ? false : testbit(operand, 31);
		    return 0;
		}
		else
		{
		    for (int i = 0; i < offs; i++)
		    {
			is_carry = testbit(operand, 0);
			operand >>= 1;
		    }

		    return operand;
		}
	    }

	    uint32_t arithmeticShiftRight(uint32_t operand, int offs, bool &is_carry, bool use_imm)
	    {
		if (offs == 0)
		{
		    if (use_imm)
		    {
			offs = 32;
		    }
		    else
		    {
			return operand;
		    }
		}

		if (offs >= 32)
		{
		    bool is_msb = testbit(operand, 31);
		    is_carry = is_msb;
		    return (is_msb) ? 0xFFFFFFFF : 0;
		}
		else
		{
		    for (int i = 0; i < offs; i++)
		    {
			bool is_msb = testbit(operand, 31);
			is_carry = testbit(operand, 0);
			operand = ((operand >> 1) | (is_msb << 31));
		    }

		    return operand;
		}
	    }

	    uint32_t rotateRight(uint32_t operand, int offs, bool &is_carry, bool use_imm)
	    {
		if ((offs != 0) || !use_imm)
		{
		    if (offs == 0)
		    {
			return operand;
		    }

		    offs %= 32;

		    if (offs == 0)
		    {
			is_carry = testbit(operand, 31);
			return operand;
		    }

		    for (int i = 0; i < offs; i++)
		    {
			bool is_lsb = testbit(operand, 0);
			operand = ((operand >> 1) | (is_lsb << 31));
			is_carry = is_lsb;
		    }

		    return operand;
		}
		else
		{
		    bool is_lsb = testbit(operand, 0);
		    operand = ((operand >> 1) | (is_carry << 31));
		    is_carry = is_lsb;
		    return operand;
		}
	    }

	    uint32_t rotateRightImm(uint32_t operand, int offs, bool &is_carry)
	    {
		int shift = ((offs & 0xF) * 2);

		if (shift == 0)
		{
		    return operand;
		}

		return rotateRight(operand, shift, is_carry, true);
	    }

	    void multiplyThumbCycles(uint32_t rd)
	    {
		if (isVersionLessThanOrEqual(4))
		{
		    multiplyCycles(rd);
		}
		else
		{
		    for (int i = 0; i < 3; i++)
		    {
			idleCycle();
		    }
		}
	    }

	    void multiplyCycles(uint32_t rd, bool is_signed = true)
	    {
		uint32_t mult = rd;
		uint32_t mask = 0xFFFFFF00;
		idleCycle();

		while (true)
		{
		    mult &= mask;

		    if (mult == 0)
		    {
			break;
		    }

		    if (is_signed && (mult == mask))
		    {
			break;
		    }

		    mask <<= 8;
		    idleCycle();
		}
	    }

	    struct BeeARMPipeline
	    {
		struct Instruction
		{
		    uint32_t addr = 0;
		    uint32_t instr = 0;
		    bool is_thumb = false;
		};

		bool is_reload = true;
		bool is_ns = true;

		Instruction fetch;
		Instruction decode;
		Instruction execute;
	    } pipeline;

	    uint32_t opcode = 0;

	    struct armregisters
	    {
		uint32_t r0 = 0;
		uint32_t r1 = 0;
		uint32_t r2 = 0;
		uint32_t r3 = 0;
		uint32_t r4 = 0;
		uint32_t r5 = 0;
		uint32_t r6 = 0;
		uint32_t r7 = 0;

		uint32_t r8 = 0;
		uint32_t r8_fiq = 0;

		uint32_t r9 = 0;
		uint32_t r9_fiq = 0;

		uint32_t r10 = 0;
		uint32_t r10_fiq = 0;

		uint32_t r11 = 0;
		uint32_t r11_fiq = 0;

		uint32_t r12 = 0;
		uint32_t r12_fiq = 0;

		uint32_t r13 = 0;
		uint32_t r13_fiq = 0;
		uint32_t r13_irq = 0;
		uint32_t r13_svc = 0;
		uint32_t r13_abt = 0;
		uint32_t r13_und = 0;

		uint32_t r14 = 0;
		uint32_t r14_fiq = 0;
		uint32_t r14_irq = 0;
		uint32_t r14_svc = 0;
		uint32_t r14_abt = 0;
		uint32_t r14_und = 0;

		uint32_t r15 = 0;

		uint32_t cpsr = 0;

		uint32_t spsr_fiq = 0;
		uint32_t spsr_svc = 0;
		uint32_t spsr_abt = 0;
		uint32_t spsr_irq = 0;
		uint32_t spsr_und = 0;
	    };

	    armregisters arm_regs;

	    bool is_interrupt = false;

	    BeeARM7Mode current_mode;

	    void setMode(BeeARM7Mode mode)
	    {
		current_mode = mode;
	    }

	    bool isIRQDisable()
	    {
		return testbit(arm_regs.cpsr, 7);
	    }

	    void setIRQDisable(bool val)
	    {
		arm_regs.cpsr = changebit(arm_regs.cpsr, 7, val);
	    }

	    void setFIQDisable(bool val)
	    {
		arm_regs.cpsr = changebit(arm_regs.cpsr, 6, val);
	    }

	    void setThumb(bool val)
	    {
		if (!is_thumb_enabled)
		{
		    return;
		}

		arm_regs.cpsr = changebit(arm_regs.cpsr, 5, val);
	    }

	    bool isSign()
	    {
		return testbit(arm_regs.cpsr, 31);
	    }

	    void setSign(bool val)
	    {
		arm_regs.cpsr = changebit(arm_regs.cpsr, 31, val);
	    }

	    bool isZero()
	    {
		return testbit(arm_regs.cpsr, 30);
	    }

	    void setZero(bool val)
	    {
		arm_regs.cpsr = changebit(arm_regs.cpsr, 30, val);
	    }

	    bool isCarry()
	    {
		return testbit(arm_regs.cpsr, 29);
	    }

	    void setCarry(bool val)
	    {
		arm_regs.cpsr = changebit(arm_regs.cpsr, 29, val);
	    }

	    bool isOverflow()
	    {
		return testbit(arm_regs.cpsr, 28);
	    }

	    void setOverflow(bool val)
	    {
		arm_regs.cpsr = changebit(arm_regs.cpsr, 28, val);
	    }

	    BeeARMInterface *inter = NULL;

	    bool is_thumb_enabled = false;
	    bool is_mult_long_enabled = false;

	    using beearm7func = function<void(uint32_t)>;
	    using beethumbfunc = function<void(uint16_t)>;
	    // using beearmdasmfunc = function<size_t(ostream&, uint32_t, uint32_t)>;

	    #include "instructions_arm7.inl"
	    #include "instructions_thumb.inl"

	    struct beeARM7mapping
	    {
		uint32_t mask;
		uint32_t value;
		beearm7func function;
	    };

	    vector<beeARM7mapping> arm_func_table;

	    beeARM7mapping decodeARMInstr(uint32_t instr);

	    struct beeTHUMBmapping
	    {
		uint16_t mask;
		uint16_t value;
		beethumbfunc function;
	    };

	    vector<beeTHUMBmapping> thumb_func_table;

	    beeTHUMBmapping decodeTHUMBInstr(uint16_t instr);

	    #include "instr_tables_arm7.inl"
	    #include "instr_tables_thumb.inl"
    };
};


#endif // BEEARM_H