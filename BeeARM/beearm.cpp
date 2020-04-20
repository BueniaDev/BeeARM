#include "beearm.h"
#include "beearm_tables.h"
using namespace beearm;

namespace beearm
{
  BeeARMInterface::BeeARMInterface()
  {

  }

  BeeARMInterface::~BeeARMInterface()
  {

  }

  BeeARM::BeeARM()
  {

  }

  BeeARM::~BeeARM()
  {

  }

  void BeeARM::init(uint32_t pc, uint32_t cpsr)
  {
    setreg(15, pc);
    setcpsr(cpsr);
    setthumbmode(TestBit(cpsr, 5));
    flushpipeline();
  }

  void BeeARM::executenextinstr()
  {
    if (instmode == armmode)
    {
      currentarminstr = nextarminstr[0];
      nextarminstr[0] = nextarminstr[1];
      nextarminstr[1].ispipelinefill = false;
      nextarminstr[1].armvalue = readLong(armreg.r15);
      armreg.r15 += 4;

      int cond = (currentarminstr.armvalue >> 28);

      if (currentarminstr.ispipelinefill)
      {
        // cout << "Filling pipeline..." << endl;
      }
      else if (getcond(cond))
      {
	isflushed = false;
        // cout << "Exectuing ARM instruction..." << endl;
        executearminstr(currentarminstr.armvalue);
      }
    }
    else
    {
      #ifdef BEEARM_ENABLE_THUMB
      currentthumbinstr = nextthumbinstr[0];
      nextthumbinstr[0] = nextthumbinstr[1];
      nextthumbinstr[1].ispipelinefill = false;
      nextthumbinstr[1].thumbvalue = readWord(armreg.r15);
      armreg.r15 += 2;

      if (currentthumbinstr.ispipelinefill)
      {
        // cout << "Filling pipeline..." << endl;
      }
      else
      {
	isflushed = false;
        // cout << "Exectuing THUMB instruction..." << endl;
        executethumbinstr(currentthumbinstr.thumbvalue);
      }
      #endif // BEEARM_ENABLED_THUMB
    }
  }

  void BeeARM::executearminstr(uint32_t instr)
  {
      if (((instr >> 8) & 0xFFFFF) == 0x12FFF)
      {
	 arm3(this);
      }
      else if (((instr >> 25) & 0x7) == 0x5)
      {
	 arm4(this);
      }
      else if ((instr & 0xD900000) == 0x1000000)
      {
	 if ((instr & 0x80) && (instr & 0x10) && ((instr & 0x2000000) == 0))
	 {
	     if (((instr >> 5) & 0x3) == 0)
	     {
		 arm12(this);
	     }
	     else
	     {
		 arm10(this);
	     }
	 }
	 else
	 {
	     arm6(this);
	 }
      }
      else if (((instr >> 26) & 0x3) == 0)
      {
	 if ((instr & 0x80) && ((instr & 0x10) == 0))
	 {
	     if (instr & 0x2000000)
	     {
		 arm5(this);
	     }
	     else if ((instr & 0x100000) && (((instr >> 23) & 0x3) == 0x2))
	     {
		 arm5(this);
	     }
	     else if (((instr >> 23) & 0x3) != 0x2)
	     {
		 arm5(this);
	     }
	     else
	     {
		 arm7(this);
	     }
	 }
	 else if ((instr & 0x80) && (instr & 0x10))
	 {
	     if (((instr >> 4) & 0xF) == 0x9)
	     {
		 if (instr & 0x2000000)
		 {
		     arm5(this);
		 }
		 else if (((instr >> 23) & 0x3) == 0x2)
		 {
		     arm12(this);
		 }
		 else
		 {
		     arm7(this);
		 }
	     }
	     else if (instr & 0x2000000)
	     {
		 arm5(this);
	     }
	     else
	     {
		 arm10(this);
	     }
	 }
	 else
	 {
	     arm5(this);
	 }
      }
      else if (((instr >> 26) & 0x3) == 0x1)
      {
	 arm9(this);
      }
      else if (((instr >> 25) & 0x7) == 0x4)
      {
	 arm11(this);
      }
      else if (((instr >> 24) & 0xF) == 0xF)
      {
	 arm13(this);
      }
      else if (((instr >> 24) & 0xF) == 0xE)
      {
	 if (instr & 0x10)
	 {
	    arm16(this);
	 }
	 else
	 {
	    arm14(this);
	 }
      }
      else if (((instr >> 25) & 0x7) == 0x6)
      {
	 arm15(this);
      }
  }

  void BeeARM::executethumbinstr(uint16_t instr)
  {
    #ifdef BEEARM_ENABLE_THUMB
    
    if (((instr >> 13) == 0) && (((instr >> 11) & 0x7) != 0x3))
    {
	thumb1(this);
    }
    else if (((instr >> 11) & 0x1F) == 0x3)
    {
	thumb2(this);
    }
    else if ((instr >> 13) == 0x1)
    {
	thumb3(this);
    }
    else if (((instr >> 10) & 0x3F) == 0x10)
    {
	thumb4(this);
    }
    else if (((instr >> 10) & 0x3F) == 0x11)
    {
	thumb5(this);
    }
    else if ((instr >> 11) == 0x9)
    {
	thumb6(this);
    }
    else if ((instr >> 12) == 0x5)
    {
	if (instr & 0x200)
	{
	    thumb8(this);
	}
	else
	{
	    thumb7(this);
	}
    }
    else if (((instr >> 13) & 0x7) == 0x3)
    {
	thumb9(this);
    }
    else if ((instr >> 12) == 0x8)
    {
	thumb10(this);
    }
    else if ((instr >> 12) == 0x9)
    {
	thumb11(this);
    }
    else if ((instr >> 12) == 0xA)
    {
	thumb12(this);
    }
    else if ((instr >> 8) == 0xB0)
    {
	thumb13(this);
    }
    else if ((instr >> 12) == 0xB)
    {
	thumb14(this);
    }
    else if ((instr >> 12) == 0xC)
    {
	thumb15(this);
    }
    else if ((instr >> 12) == 0xD)
    {
	if (((instr >> 8) & 0xF) == 0xF)
	{
	    thumb17(this);
	}
	else
	{
	    thumb16(this);
	}
    }
    else if ((instr >> 11) == 0x1C)
    {
	thumb18(this);
    }
    else if ((instr >> 11) >= 0x1E)
    {
	thumb19(this);
    }
    

    #endif // BEEARM_ENABLE_THUMB
  }
};
