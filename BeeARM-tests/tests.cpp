#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../BeeARM/beearm.h"
using namespace beearm;

class TestInterface : public BeeARMInterface
{
  public:
    TestInterface();
    ~TestInterface();

    array<uint8_t, 0x1000000> memory;

    uint8_t readByte(uint32_t addr)
    {
      return memory[addr];
    }

    uint16_t readWord(uint32_t addr)
    {
      return ((readByte(addr + 1) << 8) | (readByte(addr)));
    }

    uint32_t readLong(uint32_t addr)
    {
      return ((readWord(addr + 2) << 16) | (readWord(addr)));
    }
};

TestInterface::TestInterface()
{

}

TestInterface::~TestInterface()
{

}

TestInterface inter;
BeeARM arm;

void thumbinit()
{
  arm.setinterface(&inter);
  arm.init(0, 0x30);
}

TEST_CASE("THUMB.1-Move shifted register", "[thumb1]")
{
  thumbinit();
  arm.setreg(1, 5);

  for (int i = 0; i < 32; i++)
  {

    uint16_t instr = 0x0008;
    instr |= (i << 6);
    cout << hex << (int)(instr) << endl;
    inter.memory[0] = (instr & 0xFF);
    inter.memory[1] = (instr >> 8);

    for (int j = 0; j < 3; j++)
    {
      arm.executenextinstr();
    }

    REQUIRE(arm.getreg(0) == ((5 << i)));

    inter.memory[0] = 0;
    inter.memory[1] = 0;
  }
}