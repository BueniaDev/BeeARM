#include "../BeeARM/beearm.h"
#include <string>
#include <fstream>
using namespace beearm;
using namespace std;

class TestInterface : public BeeARMInterface
{
  public:
    TestInterface();
    ~TestInterface();

    array<uint8_t, 0x10000000> memory;

    void loadfile(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    file.seekg(0, ios::beg);
	    file.read((char*)&memory[0x8000000], size);
	    cout << "Success" << endl;
	    file.close();
	}
	else
	{
	    cout << "Error" << endl;
	    exit(1);
	}
    }

    uint8_t readByte(uint32_t addr)
    {
      addr = (addr % 0x10000000);
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
};

TestInterface::TestInterface()
{

}

TestInterface::~TestInterface()
{

}

TestInterface inter;
BeeARM arm;

void init(string filename)
{
    inter.loadfile(filename);
    arm.setinterface(&inter);
    arm.init(0x8000000, 0x5F);
    arm.armreg.r13 = 0x3007F00;
    arm.armreg.r13irq = 0x3007FA0;
}

int main()
{
    init("first.gba");

    for (int i = 0; i < 100000000; i++)
    {
	arm.executenextinstr();
	// arm.printregs();
    }

    cout << "Program execution finished." << endl;
    return 0;
}
