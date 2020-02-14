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

    void writeByte(uint32_t addr, uint8_t val)
    {
	memory[addr] = val;
    }

    uint16_t readWord(uint32_t addr)
    {
      return ((readByte(addr + 1) << 8) | (readByte(addr)));
    }

    void writeWord(uint32_t addr, uint16_t val)
    {
	writeByte(addr, (val & 0xFF));
	writeByte((addr + 1), (val >> 8));
    }

    uint32_t readLong(uint32_t addr)
    {
      return ((readWord(addr + 2) << 16) | (readWord(addr)));
    }

    void writeLong(uint32_t addr, uint32_t val)
    {
	writeWord(addr, (val & 0xFFFF));
	writeWord((addr + 2), (val >> 16));
    }

    int clockcycle(uint32_t val, int flags)
    {
	return 1;
    }

    void update()
    {
	return;
    }

    void softwareinterrupt(uint8_t val)
    {
	return;
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

void init()
{
    arm.setinterface(&inter);
}

void thumbinit()
{
  arm.init(0, 0x30);
}

void thumbtest(uint16_t instr, uint32_t val)
{
    inter.writeWord(0, instr);

    for (int j = 0; j < 3; j++)
    {
	arm.executenextinstr();
    }

    REQUIRE(arm.getreg(0) == val);

    inter.writeWord(0, 0);
    thumbinit();
}

TEST_CASE("THUMB.1-Move shifted register", "[thumb1]")
{
  init();

  SECTION("Logical Shift Left")
  {
    thumbinit();
    arm.setreg(1, 5);

    for (int i = 0; i < 32; i++)
    {
	uint16_t instr = 0x0008;
	instr |= (i << 6);
	thumbtest(instr, (5 << i));
	arm.setreg(1, 5);
    }
  }

  SECTION("Logical Shift Right")
  {
    thumbinit();
    arm.setreg(1, 0x80000000);

    uint16_t instr = 0x0808;

    thumbtest(instr, 0);

    for (int i = 1; i < 32; i++)
    {
	uint16_t instr = 0x0808;
	instr |= (i << 6);
	thumbtest(instr, (0x80000000 >> i));
	arm.setreg(1, 5);
	arm.setreg(1, 0x80000000);
    }
  }
}
