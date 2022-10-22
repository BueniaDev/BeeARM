#define thumbinstruction(mask, val, func) {mask, val, bind(&BeeARM7::thumb_##func, this, _1)}

void thumb_unknown(uint16_t instr)
{
    cout << "Unrecognized THUMB instruction of " << hex << int(instr) << endl;
    debugoutput();
    exit(1);
}

vector<beeTHUMBmapping> thumbfuncmappings =
{
    thumbinstruction(0xE000, 0x0000, shift_imm),
    thumbinstruction(0xF800, 0x1800, add_subtract),
    thumbinstruction(0xE000, 0x2000, immed),
    thumbinstruction(0xFC00, 0x4000, alu),
    thumbinstruction(0xFC00, 0x4400, hi_reg),
    thumbinstruction(0xF800, 0x4800, load_pc_rel),
    thumbinstruction(0xF200, 0x5000, load_store_reg),
    thumbinstruction(0xF200, 0x5200, load_store_half_sb),
    thumbinstruction(0xE000, 0x6000, load_store_imm),
    thumbinstruction(0xF000, 0x8000, load_store_half),
    thumbinstruction(0xF000, 0x9000, load_store_sp),
    thumbinstruction(0xF000, 0xA000, get_rel_addr),
    thumbinstruction(0xFF00, 0xB000, add_sp),
    thumbinstruction(0xF600, 0xB400, push_pop),
    thumbinstruction(0xF000, 0xC000, load_store_multiple),
    thumbinstruction(0xF000, 0xD000, cond_branch),
    thumbinstruction(0xFF00, 0xDF00, swi),
    thumbinstruction(0xF800, 0xE000, branch),
    thumbinstruction(0xF000, 0xF000, long_branch)
};