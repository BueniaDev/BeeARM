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

TEST_CASE("Test to ensure everything's working", "[test]")
{
  arm.setinterface(&inter);
  arm.init(0, 0);
  arm.executenextinstr();
  arm.executenextinstr();
  arm.executenextinstr();
}