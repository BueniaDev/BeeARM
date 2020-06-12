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

      if (!currentarminstr.ispipelinefill && getcond(cond))
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

      if (!currentthumbinstr.ispipelinefill)
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
      int hash = (((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF));
      armfunctable[hash](this);
  }

  void BeeARM::executethumbinstr(uint16_t instr)
  {
      #ifdef BEEARM_ENABLE_THUMB
      int hash = (instr >> 6);
      thumbfunctable[hash](this);
      #endif // BEEARM_ENABLE_THUMB
  }
};
