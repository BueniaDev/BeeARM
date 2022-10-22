#define arminstruction(mask, val, func) {mask, val, bind(&BeeARM::arm_##func, this, _1)}

void arm_unknown(uint32_t instr)
{
    cout << "Unrecognized ARM instruction of " << hex << int(instr) << endl;
    exit(1);
}

vector<beeARMmapping> funcmappings = 
{
    arminstruction(0x0E000000, 0x0A000000, unknown), // Branch / Branch with link
    arminstruction(0x0C000000, 0x00000000, unknown), // Data processing
    arminstruction(0x0C000000, 0x04000000, unknown), // Data transfer
    arminstruction(0x0E000000, 0x08000000, unknown), // Block transfer
    arminstruction(0x0F000000, 0x0F000000, unknown), // SWI
};