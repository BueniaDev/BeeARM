#ifndef BEEARM_H
#define BEEARM_H

#include <cstdint>
#include <string>
#include <array>
#include <iostream>
using namespace std;

#define CODE_ACC 0
#define DATA_ACC 4

#define SEQ_ACC 0
#define NSEQ_ACC 2

#define B16_ACC 0
#define B32_ACC 1

#define CODE_S16 (CODE_ACC | SEQ_ACC | B16_ACC)
#define CODE_S32 (CODE_ACC | SEQ_ACC | B32_ACC)
#define CODE_N16 (CODE_ACC | NSEQ_ACC | B16_ACC)
#define CODE_N32 (CODE_ACC | NSEQ_ACC | B32_ACC)
#define DATA_S16 (DATA_ACC | SEQ_ACC | B16_ACC)
#define DATA_S32 (DATA_ACC | SEQ_ACC | B32_ACC)
#define DATA_N16 (DATA_ACC | NSEQ_ACC | B16_ACC)
#define DATA_N32 (DATA_ACC | NSEQ_ACC | B32_ACC)

namespace beearm
{
  inline bool TestBit(uint32_t reg, int bit)
  {
    return (reg & (1 << bit)) ? true : false;
  }

  inline uint32_t BitSet(uint32_t reg, int bit)
  {
    return (reg | (1 << bit));
  }

  inline uint32_t BitReset(uint32_t reg, int bit)
  {
    return (reg & ~(1 << bit));
  }

  inline uint32_t BitChange(uint32_t reg, int bit, bool cond)
  {
    return (cond) ? BitSet(reg, bit) : BitReset(reg, bit);
  }

  class BeeARMInterface
  {
    public:
      BeeARMInterface();
      ~BeeARMInterface();

      virtual uint8_t readByte(uint32_t addr) = 0;
      virtual void writeByte(uint32_t addr, uint8_t val) = 0;
      virtual uint16_t readWord(uint32_t addr) = 0;
      virtual void writeWord(uint32_t addr, uint16_t val) = 0;
      virtual uint32_t readLong(uint32_t addr) = 0;
      virtual void writeLong(uint32_t addr, uint32_t val) = 0;
      virtual int clockcycle(uint32_t val, int flags) = 0;
      virtual void update() = 0;
  };

  class BeeARM
  {
    public:
      BeeARM();
      ~BeeARM();

      void init(uint32_t pc, uint32_t cpsr);

      struct pipelinestage
      {
        bool ispipelinefill = true;
        uint32_t armvalue;

        #ifdef BEEARM_ENABLE_THUMB
        uint16_t thumbvalue;
        #endif // BEEARM_ENABLE_THUMB
      };

      void executenextinstr();
      void executearminstr(uint32_t instr);
      #ifdef BEEARM_ENABLE_THUMB
      void executethumbinstr(uint16_t instr);
      #endif // BEEARM_ENABLE_THUMB

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
	uint32_t r8fiq = 0;

	uint32_t r9 = 0;
	uint32_t r9fiq = 0;

	uint32_t r10 = 0;
	uint32_t r10fiq = 0;

	uint32_t r11 = 0;
	uint32_t r11fiq = 0;

	uint32_t r12 = 0;
	uint32_t r12fiq = 0;

	uint32_t r13 = 0;
	uint32_t r13fiq = 0;
	uint32_t r13svc = 0;
	uint32_t r13abt = 0;
	uint32_t r13irq = 0;
	uint32_t r13und = 0;

	uint32_t r14 = 0;
	uint32_t r14fiq = 0;
	uint32_t r14svc = 0;
	uint32_t r14abt = 0;
	uint32_t r14irq = 0;
	uint32_t r14und = 0;

        uint32_t r15 = 0;
        uint32_t cpsr = 0;

	enum armmode : int
	{
	    USR = 0,
	    FIQ = 1,
	    IRQ = 2,
	    SVC = 3,
	    ABT = 4,
	    UND = 5,
	    SYS = 6,
	};

	armmode currentmode;

        uint32_t getreg(int reg)
        {
          uint32_t temp = 0;

          switch (reg)
          {
            case 0: temp = r0; break;
            case 1: temp = r1; break;
            case 2: temp = r2; break;
            case 3: temp = r3; break;
            case 4: temp = r4; break;
            case 5: temp = r5; break;
            case 6: temp = r6; break;
            case 7: temp = r7; break;
	    case 8:
	    {
		if (currentmode == armmode::FIQ)
		{
		    temp = r8fiq;
		}
		else
		{
		    temp = r8;
		}
	    }
	    break;
	    case 9:
	    {
		if (currentmode == armmode::FIQ)
		{
		    temp = r9fiq;
		}
		else
		{
		    temp = r9;
		}
	    }
	    break;
	    case 10:
	    {
		if (currentmode == armmode::FIQ)
		{
		    temp = r10fiq;
		}
		else
		{
		    temp = r10;
		}
	    }
	    break;
	    case 11:
	    {
		if (currentmode == armmode::FIQ)
		{
		    temp = r11fiq;
		}
		else
		{
		    temp = r11;
		}
	    }
	    break;
	    case 12:
	    {
		if (currentmode == armmode::FIQ)
		{
		    temp = r12fiq;
		}
		else
		{
		    temp = r12;
		}
	    }
	    break;
	    case 13:
	    {
		switch (currentmode)
		{
		    case armmode::FIQ: temp = r13fiq; break;
		    case armmode::SVC: temp = r13svc; break;
		    case armmode::ABT: temp = r13abt; break;
		    case armmode::IRQ: temp = r13irq; break;
		    case armmode::UND: temp = r13und; break;
		    default: temp = r13; break;
		}
	    }
	    break;
	    case 14:
	    {
		switch (currentmode)
		{
		    case armmode::FIQ: temp = r14fiq; break;
		    case armmode::SVC: temp = r14svc; break;
		    case armmode::ABT: temp = r14abt; break;
		    case armmode::IRQ: temp = r14irq; break;
		    case armmode::UND: temp = r14und; break;
		    default: temp = r14; break;
		}
	    }
	    break;
            case 15: temp = r15; break;
            default: cout << "Unrecognized register of " << dec << (int)(reg) << endl; exit(1); break;
          }

          return temp;
        }

        void setreg(int reg, uint32_t val)
        {
          switch (reg)
          {
            case 0: r0 = val; break;
            case 1: r1 = val; break;
            case 2: r2 = val; break;
            case 3: r3 = val; break;
            case 4: r4 = val; break;
            case 5: r5 = val; break;
            case 6: r6 = val; break;
            case 7: r7 = val; break;
	    case 8:
	    {
		if (currentmode == armmode::FIQ)
		{
		    r8fiq = val;
		}
		else
		{
		    r8 = val;
		}
	    }
	    break;
	    case 9:
	    {
		if (currentmode == armmode::FIQ)
		{
		    r9fiq = val;
		}
		else
		{
		    r9 = val;
		}
	    }
	    break;
	    case 10:
	    {
		if (currentmode == armmode::FIQ)
		{
		    r10fiq = val;
		}
		else
		{
		    r10 = val;
		}
	    }
	    break;
	    case 11:
	    {
		if (currentmode == armmode::FIQ)
		{
		    r11fiq = val;
		}
		else
		{
		    r11 = val;
		}
	    }
	    break;
	    case 12:
	    {
		if (currentmode == armmode::FIQ)
		{
		    r12fiq = val;
		}
		else
		{
		    r12 = val;
		}
	    }
	    break;
	    case 13:
	    {
		switch (currentmode)
		{
		    case armmode::FIQ: r13fiq = val; break;
		    case armmode::SVC: r13svc = val; break;
		    case armmode::ABT: r13abt = val; break;
		    case armmode::IRQ: r13irq = val; break;
		    case armmode::UND: r13und = val; break;
		    default: r13 = val; break;
		}
	    }
	    break;
	    case 14:
	    {
		switch (currentmode)
		{
		    case armmode::FIQ: r14fiq = val; break;
		    case armmode::SVC: r14svc = val; break;
		    case armmode::ABT: r14abt = val; break;
		    case armmode::IRQ: r14irq = val; break;
		    case armmode::UND: r14und = val; break;
		    default: r14 = val; break;
		}
	    }
	    break;
            case 15: r15 = val; break;
            default: cout << "Unrecognized register of " << dec << (int)(reg) << endl; exit(1); break;
          }
        }

	void setmode(uint32_t val)
	{
	    int mode = (val & 0x1F);

	    switch (mode)
	    {
		case 0x10: currentmode = armmode::USR; break;
		case 0x11: currentmode = armmode::FIQ; break;
		case 0x12: currentmode = armmode::IRQ; break;
		case 0x13: currentmode = armmode::SVC; break;
		case 0x17: currentmode = armmode::ABT; break;
		case 0x1B: currentmode = armmode::UND; break;
		case 0x1F: currentmode = armmode::SYS; break;
		default: cout << "Unrecognized mode of " << hex << (int)(mode) << endl; exit(1); break;
	    }
	}

        uint32_t getcpsr()
        {
          return cpsr;
        }

        void setcpsr(uint32_t val)
        {
          cpsr = val;
	  setmode(val);
        }
      };

      BeeARMInterface *inter = NULL;

      void setinterface(BeeARMInterface *cb)
      {
        inter = cb;
      }

      armregisters armreg;

      string instmode = "";
      string armmode = "ARM";
      string thumbmode = "THUMB";

      pipelinestage currentarminstr;
      array<pipelinestage, 2> nextarminstr;

      #ifdef BEEARM_ENABLE_THUMB
      pipelinestage currentthumbinstr;
      array<pipelinestage, 2> nextthumbinstr;
      #endif // BEEARM_ENABLE_THUMB

      int clockcycles = 0;

      void flushpipeline()
      {
	currentarminstr.ispipelinefill = true;
	currentarminstr.armvalue = 0;

	for (int i = 0; i < 2; i++)
	{
	    nextarminstr[i].ispipelinefill = true;
	    nextarminstr[i].armvalue = 0;
	}
	#ifdef BEEARM_ENABLE_THUMB
	currentthumbinstr.ispipelinefill = true;
	currentthumbinstr.thumbvalue = 0;

	for (int i = 0; i < 2; i++)
	{
	    nextthumbinstr[i].ispipelinefill = true;
            nextthumbinstr[i].thumbvalue = 0;
	}
	#endif // BEEARM_ENABLE_THUMB
      }

      uint32_t getreg(int reg)
      {
        return armreg.getreg(reg);
      }

      void setreg(int reg, uint32_t val)
      {
        armreg.setreg(reg, val);
      }

      uint32_t getcpsr()
      {
        return armreg.getcpsr();
      }

      void setcpsr(uint32_t val)
      {
        armreg.setcpsr(val);
      }

	void printregs()
	{
	    for (int i = 0; i < 16; i++)
	    {
		cout << "R" << dec << (int)(i) << ": " << hex << (int)(getreg(i)) << endl;
	    }

	    cout << "CPSR: " << hex << (int)(getcpsr()) << endl;
	}

      void setthumbmode(bool cond)
      {
        setcpsr(BitChange(getcpsr(), 5, cond));

        instmode = (cond) ? thumbmode : armmode;
        flushpipeline();
      }

	void setnz(bool n, bool z)
	{
	    setcpsr(BitChange(getcpsr(), 31, n));
	    setcpsr(BitChange(getcpsr(), 30, z));
	}

	void setnzc(bool n, bool z, bool c)
	{
	    setcpsr(BitChange(getcpsr(), 31, n));
	    setcpsr(BitChange(getcpsr(), 30, z));
	    setcpsr(BitChange(getcpsr(), 29, c));
	}

	bool getn()
	{
	    return TestBit(getcpsr(), 31);
	}

	bool getz()
	{
	    return TestBit(getcpsr(), 30);
	}

	bool getc()
	{
	    return TestBit(getcpsr(), 29);
	}

	inline bool getcond(int instr)
	{
	    bool temp = false;

	    switch (instr)
	    {
		case 0x0: temp = getz(); break;
		case 0x1: temp = !getz(); break;
		case 0x2: temp = getc(); break;
		case 0x3: temp = !getc(); break;
		case 0x4: temp = getn(); break;
		case 0x5: temp = !getn(); break;
		case 0x8: temp = (getc() && !getz()); break;
		case 0x9: temp = (!getc() && getz()); break;
		case 0xE: temp = true; break;
		case 0xF: temp = true; break; // Reserved conditon
		default: cout << "Unrecognized condition of " << hex << (int)(instr) << endl; exit(1); break;
	    }

	    return temp;
	}

      uint8_t readByte(uint32_t addr)
      {
        if (inter != NULL)
        {
          return inter->readByte(addr);
        }
        else
        {
          return 0xFF;
        }
      }

      void writeByte(uint32_t addr, uint8_t val)
      {
	if (inter != NULL)
	{
	  inter->writeByte(addr, val);
	}
      }

      uint16_t readWord(uint32_t addr)
      {
        if (inter != NULL)
        {
          return inter->readWord(addr);
        }
        else
        {
          return 0xFFFF;
        }
      }

      void writeWord(uint32_t addr, uint16_t val)
      {
	if (inter != NULL)
	{
	  inter->writeWord(addr, val);
	}
      }

      uint32_t readLong(uint32_t addr)
      {
        if (inter != NULL)
        {
          return inter->readLong(addr);
        }
        else
        {
          return 0xFFFFFFFF;
        }
      }

      void writeLong(uint32_t addr, uint32_t val)
      {
	if (inter != NULL)
	{
	  inter->writeLong(addr, val);
	}
      }

      void clock(uint32_t val, int flags)
      {
	if (inter != NULL)
	{
	    int cycles = inter->clockcycle(val, flags);

	    clockcycles += cycles;

	    for (; cycles != 0; cycles--)
	    {
		inter->update();
	    }
	}
      }

      void clock()
      {
	if (inter != NULL)
	{
	    clockcycles += 1;
	    inter->update();
	}	
      }
  };
};

#endif // BEEARM_H
