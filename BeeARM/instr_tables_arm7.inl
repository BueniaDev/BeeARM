#define arm7instruction(mask, val, func) {mask, val, bind(&BeeARM7::arm7_##func, this, _1)}

void arm7_unknown(uint32_t instr)
{
    throw BeeARM7UnknownInstruction(getVersion(), instr);
    return;
}

vector<beeARM7mapping> arm7funcmappings = 
{
    arm7instruction(0x0FFF0FF0, 0x016F0F10, unknown), // ARMv5 and up
    arm7instruction(0x0F000FF0, 0x01000050, unknown), // ARMv5 and up
    arm7instruction(0x0E000010, 0x00000000, data_proc),
    arm7instruction(0x0E000090, 0x00000010, data_proc),
    arm7instruction(0x0E000000, 0x02000000, data_proc),
    arm7instruction(0x0FB00000, 0x03200000, mrs_msr),

    arm7instruction(0x0F900FF0, 0x01000000, mrs_msr),
    arm7instruction(0x0FF000D0, 0x01200010, bx),
    arm7instruction(0x0E400F90, 0x00000090, half_transfer_reg),
    arm7instruction(0x0E400090, 0x00400090, half_transfer_imm),

    arm7instruction(0x0FB00FF0, 0x01000090, swap),
    arm7instruction(0x0E000000, 0x04000000, single_transfer),
    arm7instruction(0x0FC000F0, 0x00000090, mul_mla),
    arm7instruction(0x0F8000F0, 0x00800090, mul_mla_long),

    arm7instruction(0x0E000010, 0x06000000, single_transfer),
    arm7instruction(0x0E000010, 0x06000010, unknown), // Undefined instruction
    arm7instruction(0x0550F000, 0x0550F000, unknown), // PLD (ARMv5TE and up)
    arm7instruction(0x0E000000, 0x08000000, block_transfer),
    arm7instruction(0x0E000000, 0x0A000000, branch),

    arm7instruction(0x0E000000, 0x0C000000, unknown), // Coprocessor data transfer
    arm7instruction(0x0F000000, 0x0E000000, unknown), // Coprocessor data operations
    arm7instruction(0x0F000010, 0x0E000010, coproc_reg_transfer),
    arm7instruction(0x0F000000, 0x0F000000, swi),
    arm7instruction(0x0FF000F0, 0x01200070, unknown) // BKPT (ARMv5 and up)
};