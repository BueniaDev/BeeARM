#ifndef BEEARM_INTERP_DEFINES
#define BEEARM_INTERP_DEFINES

#define CARRY_ADD(x, y) carryadd(x, y)

#define CARRY_SUB(x, y) carrysub(x, y)

#define OVERFLOW_ADD(x, y, res) overflowadd(x, y, res)

#define OVERFLOW_SUB(x, y, res) overflowsub(x, y, res)

inline bool carryadd(uint32_t x, uint32_t y)
{
    uint64_t op1 = (uint64_t)(x);
    uint64_t op2 = (uint64_t)(y);
    return ((op1 + op2) > 0xFFFFFFFF);
}

inline bool carrysub(uint32_t x, uint32_t y)
{
    uint64_t op1 = (uint64_t)(x);
    uint64_t op2 = (uint64_t)(y);
    return (op2 <= op1);
}

inline bool overflowadd(uint32_t x, uint32_t y, uint32_t result)
{
    bool op1 = (x >> 31);
    bool op2 = (y >> 31);
    bool res = (result >> 31);
    return ((op1 == op2) && (op1 != res));
}

inline bool overflowsub(uint32_t x, uint32_t y, uint32_t result)
{
    bool op1 = (x >> 31);
    bool op2 = (y >> 31);
    bool res = (result >> 31);
    return ((op2 != op1) && (op2 == res));
}

#define RORBASE(x, offs) ((x >> offs) | (x << (32 - offs)))

#define LSL(x, offs) \
    x <<= offs;

#define LSLS(x, offs, carry) \
    if (offs > 0) \
    { \
	carry = TestBit(x, (32 - offs)); \
    } \
    else \
    { \
	carry = TestBit(arm->getcpsr(), 29); \
    } \

#define LSLREG(x, offs) \
    if (offs > 31) \
    { \
	x = 0; \
    } \
    else \
    { \
	x <<= offs; \
    } \

#define LSLREGS(x, offs, carry) \
    if (offs > 31) \
    { \
	carry = (offs > 32) ? false : TestBit(x, 0); \
    } \
    else if (offs > 0) \
    { \
	carry = TestBit(x, (32 - offs)); \
    } \
    else \
    { \
	carry = TestBit(arm->getcpsr(), 29); \
    } \

#define LSR(x, offs) \
    if (offs == 0) \
    { \
	x = 0; \
    } \
    else \
    { \
	x >>= offs; \
    }

#define LSRS(x, offs, carry) \
    if (offs == 0) \
    { \
	carry = TestBit(x, 31); \
    } \
    else \
    { \
	carry = TestBit(x, (offs - 1)); \
    }

#define ASR(x, offs) \
    if (offs == 0) \
    { \
	x = (((int32_t)x) >> 31); \
    } \
    else \
    { \
	x= (((int32_t)x) >> offs); \
    }

#define ASRS(x, offs, carry) \
    if (offs == 0) \
    { \
	carry = TestBit(x, 31); \
    } \
    else \
    { \
	carry = TestBit(x, (offs - 1)); \
    }

#define ROR(x, offs) \
    if (offs == 0) \
    { \
	x = ((x >> 1) | (TestBit(arm->getcpsr(), 29) << 31)); \
    } \
    else \
    { \
	x = RORBASE(x, offs); \
    }

#define RORS(x, offs, carry) \
    if (offs == 0) \
    { \
	carry = TestBit(x, 0); \
    } \
    else \
    { \
	carry = TestBit(x, (offs - 1)); \
    }

#define RORREG(x, offs) \
    x = RORBASE(x, (offs & 0x1F));

#define RORREGS(x, offs, carry) \
    if (offs > 0) \
    { \
	carry = TestBit(x, (offs - 1)); \
    } \
    else \
    { \
	carry = TestBit(arm->getcpsr(), 29); \
    }


#endif // BEEARM_INTERP_DEFINES
