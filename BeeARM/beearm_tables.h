#ifndef BEEARM_TABLES
#define BEEARM_TABLES

#include <array>
#include <functional>
#include <iostream>
#include <unordered_map>
#include "beearm.h"
using namespace beearm;

namespace beearm
{
  void unrecognizedarminstr(BeeARM *arm)
  {
    cout << "Unrecognized opcode at " << hex << (int)(arm->currentarminstr.armvalue) << endl;
    exit(1);
  }

  #ifdef BEEARM_ENABLE_THUMB
  void unrecognizedthumbinstr(BeeARM *arm)
  {
    cout << "Unrecognized opcode at " << hex << (int)(arm->currentthumbinstr.thumbvalue) << endl;
    exit(1);
  }
  #endif // BEEARM_ENABLE_THUMB

  using armfunc = function<void(BeeARM*)>;

  array<uint32_t, 20> armresulttable = 
  {
    0x00000000, 0x00000010, 0x02000000, 0x03200000,
    0x01000000, 0x012fff10, 0x00000090, 0x00800090,
    0x01000090, 0x00000090, 0x00400090, 0x04000000,
    0x06000000, 0x06000010, 0x08000000, 0x0A000000,
    0x0C000000, 0x0E000000, 0x0E000010, 0x0F000000,
  };

  array<uint32_t, 20> armmasktable = 
  {
    0x0E000010, 0x0E000090, 0x0E000000, 0x0FB00000,
    0x0F900FF0, 0x0FFFFFD0, 0x0FC000F0, 0x0F8000F0,
    0x0FB00FF0, 0x0E400F90, 0x0E400090, 0x0E000000,
    0x0E000010, 0x0E000010, 0x0E000000, 0x0E000000,
    0x0E000000, 0x0F000000, 0x0F000010, 0x0F000000,
  };

  unordered_map<uint32_t, armfunc> armfunctable = 
  {
    { 0x00000000, unrecognizedarminstr },
    { 0x00000010, unrecognizedarminstr },
    { 0x02000000, unrecognizedarminstr },
    { 0x03200000, unrecognizedarminstr },
    { 0x01000000, unrecognizedarminstr },
    { 0x012fff10, unrecognizedarminstr },
    { 0x00000090, unrecognizedarminstr },
    { 0x00800090, unrecognizedarminstr },
    { 0x01000090, unrecognizedarminstr },
    { 0x00000090, unrecognizedarminstr },
    { 0x00400090, unrecognizedarminstr },
    { 0x04000000, unrecognizedarminstr },
    { 0x06000000, unrecognizedarminstr },
    { 0x06000010, unrecognizedarminstr },
    { 0x08000000, unrecognizedarminstr },
    { 0x0A000000, unrecognizedarminstr },
    { 0x0C000000, unrecognizedarminstr },
    { 0x0E000000, unrecognizedarminstr },
    { 0x0E000010, unrecognizedarminstr },
    { 0x0F000000, unrecognizedarminstr },
  };

  #ifdef BEEARM_ENABLE_THUMB

  array<uint16_t, 19> thumbresulttable = 
  {
    0x0000, 0x1800, 0x2000, 0x4000,
    0x4400, 0x4800, 0x5000, 0x5200,
    0x6000, 0x8000, 0x9000, 0xA000,
    0xB000, 0xB400, 0xBE00, 0xD000,
    0xDF00, 0xE000, 0xF000,
  };

  array<uint16_t, 19> thumbmasktable = 
  {
    0xE000, 0xF800, 0xE000, 0xFC00,
    0xFC00, 0xF800, 0xF200, 0xF200,
    0xE000, 0xF000, 0xF000, 0xF000,
    0xFF00, 0xF600, 0xF000, 0xF000,
    0xFF00, 0xF800, 0xF000,
  };

  unordered_map<uint16_t, armfunc> thumbfunctable = 
  {
    { 0x0000, unrecognizedthumbinstr },
    { 0x1800, unrecognizedthumbinstr },
    { 0x2000, unrecognizedthumbinstr },
    { 0x4000, unrecognizedthumbinstr },
    { 0x4400, unrecognizedthumbinstr },
    { 0x4800, unrecognizedthumbinstr },
    { 0x5000, unrecognizedthumbinstr },
    { 0x5200, unrecognizedthumbinstr },
    { 0x6000, unrecognizedthumbinstr },
    { 0x8000, unrecognizedthumbinstr },
    { 0x9000, unrecognizedthumbinstr },
    { 0xA000, unrecognizedthumbinstr },
    { 0xB000, unrecognizedthumbinstr },
    { 0xB400, unrecognizedthumbinstr },
    { 0xBE00, unrecognizedthumbinstr },
    { 0xD000, unrecognizedthumbinstr },
    { 0xDF00, unrecognizedthumbinstr },
    { 0xE000, unrecognizedthumbinstr },
    { 0xF000, unrecognizedthumbinstr },
  };

  #endif
};

#endif // BEEARM_TABLES