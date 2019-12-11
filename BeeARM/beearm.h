#ifndef BEEARM_H
#define BEEARM_H

#include <cstdint>
#include <string>
#include <array>
#include <iostream>
using namespace std;

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
      virtual uint16_t readWord(uint32_t addr) = 0;
      virtual uint32_t readLong(uint32_t addr) = 0;
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
        uint32_t r15 = 0;
        uint32_t cpsr = 0;

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
            case 15: r15 = val; break;
            default: cout << "Unrecognized register of " << dec << (int)(reg) << endl; exit(1); break;
          }
        }

        uint32_t getcpsr()
        {
          return cpsr;
        }

        void setcpsr(uint32_t val)
        {
          cpsr = val;
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

      void flushpipeline()
      {
        if (instmode == armmode)
        {
          currentarminstr.ispipelinefill = true;
          currentarminstr.armvalue = 0;

          for (int i = 0; i < 2; i++)
          {
            nextarminstr[i].ispipelinefill = true;
            nextarminstr[i].armvalue = 0;
          }
        }
        else
        {
          #ifdef BEEARM_ENABLE_THUMB
          currentthumbinstr.ispipelinefill = true;
          currentthumbinstr.thumbvalue = 0;

          for (int i = 0; i < 2; i++)
          {
            nextarminstr[i].ispipelinefill = true;
            nextarminstr[i].thumbvalue = 0;
          }
          #endif // BEEARM_ENABLE_THUMB
        }
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

      void setthumbmode(bool cond)
      {
        setcpsr(BitChange(getcpsr(), 5, cond));

        instmode = (cond) ? thumbmode : armmode;
        flushpipeline();
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
  };
};

#endif // BEEARM_H