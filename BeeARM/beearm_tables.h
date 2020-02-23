#ifndef BEEARM_TABLES
#define BEEARM_TABLES

#include <array>
#include <functional>
#include <iostream>
#include <unordered_map>
#include "beearm.h"
#include "beearm_interpreterthumb.h"
#include "beearm_interpreterarm.h"
using namespace beearm;

namespace beearm
{
  inline void unrecognizedarminstr(BeeARM *arm)
  {
    cout << "Unrecognized opcode at " << hex << (int)(arm->currentarminstr.armvalue) << endl;
    exit(1);
  }

  #ifdef BEEARM_ENABLE_THUMB
  inline void unrecognizedthumbinstr(BeeARM *arm)
  {
    cout << "Unrecognized opcode at " << hex << (int)(arm->currentthumbinstr.thumbvalue) << endl;
    exit(1);
  }
  #endif // BEEARM_ENABLE_THUMB

  inline void arm7t10(BeeARM *arm)
  {
    uint32_t instr = arm->currentarminstr.armvalue;

    if (((instr >> 4) & 0xF) == 9)
    {
	arm7(arm);
    }
    else
    {
	arm10(arm);
    }
  }

  using armfunc = function<void(BeeARM*)>;

  inline array<uint32_t, 19> armresulttable = 
  {
    0x03200000, 0x01000000, 0x012fff10, 0x00000000, 
    0x00000010, 0x02000000, 0x00000090, 0x01000090,
    0x00000090, 0x00400090, 0x04000000, 0x06000000,
    0x06000010, 0x08000000, 0x0A000000, 0x0C000000,
    0x0E000000, 0x0E000010, 0x0F000000,
  };

  inline array<uint32_t, 19> armmasktable = 
  {
    0x0FB00000, 0x0F900FF0, 0x0FFFFFD0, 0x0E000010,
    0x0E000090, 0x0E000000, 0x0FC000F0, 0x0FB00FF0, 
    0x0E400F90, 0x0E400090, 0x0E000000, 0x0E000010,
    0x0E000010, 0x0E000000, 0x0E000000, 0x0E000000, 
    0x0F000000, 0x0F000010, 0x0F000000,
  };

  inline unordered_map<uint32_t, armfunc> armfunctable = 
  {
    { 0x03200000, arm6 },
    { 0x01000000, arm6 },
    { 0x012fff10, arm3 },
    { 0x00000000, arm5 },
    { 0x00000010, arm5 },
    { 0x02000000, arm5 },
    { 0x00000090, arm7t10 },
    { 0x01000090, arm12 },
    { 0x00000090, arm7t10 },
    { 0x00400090, arm10 },
    { 0x04000000, arm9 },
    { 0x06000000, arm9 },
    { 0x06000010, arm17 },
    { 0x08000000, arm11 },
    { 0x0A000000, arm4 },
    { 0x0C000000, arm15 },
    { 0x0E000000, arm14 },
    { 0x0E000010, arm16 },
    { 0x0F000000, arm13 },
  };

  #ifdef BEEARM_ENABLE_THUMB

  inline array<uint16_t, 19> thumbresulttable = 
  {
    0x1800, 0x0000, 0x2000, 0x4000,
    0x4400, 0x4800, 0x5000, 0x5200,
    0x6000, 0x8000, 0x9000, 0xA000,
    0xB000, 0xB400, 0xC000, 0xDF00,
    0xD000, 0xE000, 0xF000,
  };

  inline array<uint16_t, 19> thumbmasktable = 
  {
    0xF800, 0xE000, 0xE000, 0xFC00,
    0xFC00, 0xF800, 0xF200, 0xF200,
    0xE000, 0xF000, 0xF000, 0xF000,
    0xFF00, 0xF600, 0xF000, 0xFF00,
    0xF000, 0xF800, 0xF000,
  };

  inline unordered_map<uint16_t, armfunc> thumbfunctable = 
  {
    { 0x1800, thumb2 },
    { 0x0000, thumb1 },
    { 0x2000, thumb3 },
    { 0x4000, thumb4 },
    { 0x4400, thumb5 },
    { 0x4800, thumb6 },
    { 0x5000, thumb7 },
    { 0x5200, thumb8 },
    { 0x6000, thumb9 },
    { 0x8000, thumb10 },
    { 0x9000, thumb11 },
    { 0xA000, thumb12 },
    { 0xB000, thumb13 },
    { 0xB400, thumb14 },
    { 0xC000, thumb15 },
    { 0xDF00, thumb17 },
    { 0xD000, thumb16 },
    { 0xE000, thumb18 },
    { 0xF000, thumb19 },
  };

  #endif
};

#endif // BEEARM_TABLES
