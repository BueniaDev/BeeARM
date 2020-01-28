#ifndef BEEARM_INTERP_DEFINES
#define BEEARM_INTERP_DEFINES

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

#endif // BEEARM_INTERP_DEFINES
