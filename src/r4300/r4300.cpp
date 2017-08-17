#include <cstring>
#include <sstream>
#include <r4300.h>
#include <memory.h>

namespace ultra64
{
    void r4300::beql(r4300 *cpu, opcode_t *op)
    {
        int32_t branch_offset = ((int16_t)op->offset) << 2;
        uint32_t branch = cpu->PC + branch_offset;
        
        if(cpu->GPR[op->rs] == cpu->GPR[op->rt]) cpu->PC = branch;
        else cpu->PC += 4; 
    }

    void r4300::bnel(r4300 *cpu, opcode_t *op)
    {
        int32_t branch_offset = ((int16_t)op->offset) << 2;
        uint64_t branch = cpu->PC + branch_offset + 4;

        if(cpu->GPR[op->rs] != cpu->GPR[op->rt]) 
        {
            cpu->PC += 4;
            cpu->step();
            cpu->PC = branch;
        }
        else cpu->PC += 8;
    }

    void r4300::bne(r4300 *cpu, opcode_t *op)
    {
        if(cpu->GPR[op->rs] != cpu->GPR[op->rt]) 
        {
            opcode_t *delay_op = cpu->get_instruction(cpu->PC + 4);

            try
            {
                cpu->opcode[delay_op->opcode](cpu, delay_op); 
            }
            catch(std::string e)
            {
                throw e;
            }
            cpu->CP0[9] += 2;

            cpu->PC += (((int16_t)op->offset) << 2) + 4; 
        }
        else cpu->PC += 4;
    }

    void r4300::andi(r4300 *cpu, opcode_t *op)
    {

        cpu->GPR[op->rt] = cpu->GPR[op->rs] & (uint64_t)(op->offset);
        cpu->PC += 4;
    }

    void r4300::addiu(r4300 *cpu, opcode_t *op)
    {
        cpu->GPR[op->rt] = cpu->GPR[op->rs] + (int16_t)(op->offset);
        cpu->PC += 4;
    }

    void r4300::lui(r4300 *cpu, opcode_t *op)
    {
        cpu->GPR[op->rt] = (uint64_t)(op->offset) * 0x10000;
        cpu->PC += 4;
    }

    void r4300::lw(r4300 *cpu, opcode_t *op)
    {
        try
        {
            cpu->GPR[op->rt] = mmu->read_word(cpu->GPR[op->base] + op->offset);
        }
        catch(std::string e) 
        {
            throw e;
        }
        cpu->PC += 4;
    }

    void r4300::sw(r4300 *cpu, opcode_t *op)
    {
        try {
            mmu->write_word(cpu->GPR[op->base] + (int16_t)(op->offset), (cpu->GPR[op->rt] & 0xFFFFFFFF));
        } catch(std::string e) { throw e; }
        cpu->PC += 4;
    }

    void r4300::ori(r4300 *cpu, opcode_t *op)
    {
        cpu->GPR[op->rt] = cpu->GPR[op->rs] | (uint64_t)(op->offset);
        cpu->PC += 4;
    }

    void r4300::not_implemented(r4300 *cpu, opcode_t *op)
    {
        char message[1024];
        sprintf(message, "0x%.8X: NOT IMPLEMENTED %s\n", cpu->get_PC(), cpu->get_instruction(cpu->get_PC())->to_string().c_str());
        throw std::string(message);
    }

    void r4300::cp0_not_implemented(r4300 *cpu, opcode_t *op)
    {
        char message[1024];
        sprintf(message, "0x%.8X: Instruction 0x%X not implemented.\n", cpu->get_PC(), cpu->get_instruction(cpu->get_PC())->opcode);
        throw std::string(message);
    }

    void r4300::special_not_implemented(r4300 *cpu, opcode_t *op)
    {
        char message[1024];
        sprintf(message, "0x%.8X: Instruction 0x%X not implemented.\n", cpu->get_PC(), cpu->get_instruction(cpu->get_PC())->opcode);
        throw std::string(message);
    }

    void r4300::cp0(r4300 *cpu, opcode_t *op)
    {
        cpu->opcode_cp0[op->function](cpu, op);
    }

    uint32_t r4300::get_PC()
    {
        return PC;
    }

    uint64_t r4300::get_GPR(uint8_t reg)
    {
        return GPR[reg];
    }

    uint64_t r4300::get_CP0(uint8_t reg)
    {
        return CP0[reg];
    }

    r4300::r4300()
    {
        PC = 0x1FC00000;

        uint32_t count = 0;
        while(count < 0x40)
        {
            opcode[count] = not_implemented;
            opcode_cp0[count] = cp0_not_implemented;
            count++;
        }

        opcode[BEQL] = beql;
        opcode[BNEL] = bnel;
        opcode[BNE] = bne;
        opcode[LW] = lw;
        opcode[SW] = sw;
        opcode[ANDI] = andi;
        opcode[ADDIU] = addiu;
        opcode[ORI] = ori;
        opcode[LUI] = lui;
        opcode[_CP0] = cp0;
        opcode[_SPECIAL] = special;

        opcode_cp0[0] = move_cp0;

        opcode_special[JR] = jr;
    }

    r4300::~r4300()
    {
    }

    void r4300::step()
    {
        opcode_t *op = get_instruction(PC);

        try
        {
            opcode[op->opcode](this, op);
        } 
        catch(std::string e)
        {
            throw e;
        }
        CP0[9] += 2;
    }

    opcode_t *r4300::get_instruction(uint32_t addr)
    {
        uint32_t instruction = mmu->read_word(addr);
        if(op_cache[instruction] == nullptr) op_cache[instruction] = new opcode_t(instruction);

        return op_cache[instruction];
    }

    opcode_t::opcode_t(uint32_t instruction)
    {
        this->instruction = instruction;
        decode_instruction();
    }

    void opcode_t::decode_instruction()
    {
        opcode = instruction >> 26;
        rs = base = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        rd = (instruction >> 11) & 0x1F;
        sa = (instruction >> 6) & 0x1F;
        function = instruction & 0x3F;
        offset = instruction & 0xFFFF;
    }

    std::string opcode_t::to_string()
    {
        char temp[1024];

        switch(opcode)
        {
            case _SPECIAL:
                return special_to_string();
                break;
            case _REGIMM:
                return regimm_to_string();
                break;
            case BEQ:
                sprintf(temp, "BEQ %d, %d, 0x%.4X", rs, rt, offset);
                break;
            case BNE:
                sprintf(temp, "BNE %d, %d, 0x%.4X", rs, rt, offset);
                break;
            case ADDI:
                sprintf(temp, "ADDI %d, %d, 0x%.4X", rt, rs, offset);
                break;
            case ADDIU:
                sprintf(temp, "ADDIU %d, %d, 0x%.4X", rt, rs, offset);
                break;
            case ANDI:
                sprintf(temp, "ANDI %d, %d, 0x%.4X", rt, rs, offset);
                break;
            case ORI:
                sprintf(temp, "ORI %d, %d, 0x%.4X", rt, rs, offset);
                break;
            case LUI:
                sprintf(temp, "LUI %d, 0x%.4X", rt, offset);
                break;
            case _CP0:
                return cp0_to_string();
                break;
            case BEQL:
                sprintf(temp, "BEQL %d, %d, 0x%.4X", rs, rt, offset);
                break;
            case BNEL:
                sprintf(temp, "BNEL %d, %d, 0x%.4X", rs, rt, offset);
                break;
            case DADDI:
                sprintf(temp, "DADDI %d, %d, 0x%.4X", rs, rt, offset);
                break;
            case LB:
                sprintf(temp, "LB %d, 0x%.4X(%d)", rt, offset, base);
                break;
            case LW:
                sprintf(temp, "LW %d, 0x%.4X(%d)", rt, offset, base);
                break;
            case SW:
                sprintf(temp, "SW %d, 0x%.4X(%d)", rt, offset, base);
                break;
            default:
                sprintf(temp, "Unknown opcode 0x%.2X", opcode);
                break;
        }

        return std::string(temp);
    }

    std::string opcode_t::special_to_string()
    {
        char temp[1024];

        switch(function)
        {
            case SLL:
                sprintf(temp, "SLL %d, %d, %d", rd, rt, sa);
                break;
            case SRL:
                sprintf(temp, "SRL %d, %d, %d", rd, rt, sa);
                break;
            case SLLV:
                sprintf(temp, "SLLV %d, %d, %d", rd, rt, rs);
                break;
            case SRLV:
                sprintf(temp, "SRLV %d, %d, %d", rd, rt, rs);
                break;
            case JR:
                sprintf(temp, "JR %d", rs);
                break;
            case MFHI:
                sprintf(temp, "MFHI %d", rd);
                break;
            case MFLO:
                sprintf(temp, "MFLO %d", rd);
                break;
            case MULTU:
                sprintf(temp, "MULTU %d, %d", rs, rt);
                break;
            case ADDU:
                sprintf(temp, "ADDU %d, %d, %d", rd, rs, rt);
                break;
            case SUBU:
                sprintf(temp, "SUBU %d, %d, %d", rd, rs, rt);
                break;
            case AND:
                sprintf(temp, "AND %d, %d, %d", rd, rs, rt);
                break;
            case OR:
                sprintf(temp, "OR %d, %d, %d", rd, rs, rt);
                break;
            case XOR:
                sprintf(temp, "XOR %d, %d, %d", rd, rs, rt);
                break;
            case SLTU:
                sprintf(temp, "SLTU %d, %d, %d", rd, rs, rt);
                break;
            case DSLL32:
                sprintf(temp, "DSLL32 %d, %d, %d", rd, rt, sa);
                break;
            default:
                sprintf(temp, "Unknown special opcode 0x%.2X", function);
                break;
        }
        
        return std::string(temp);
    }

    std::string opcode_t::regimm_to_string()
    {
        char temp[1024];

        switch(rt)
        {
            case BLTZ:
                sprintf(temp, "BLTZ %d, 0x%.4X", rs, offset);
                break;
            case BGEZAL:
                sprintf(temp, "BGEZAL %d, 0x%.4X", rs, offset);
                break;
            default:
                sprintf(temp, "Unknown regimm opcode 0x%.2X", rt);
                break;
        }

        return std::string(temp);
    }

    std::string opcode_t::cp0_to_string()
    {
        char temp[1024];

        switch(rs)
        {
            case MFC0:
                sprintf(temp, "MFC0 %d, %d", rt, rd);
                break;
            case MTC0:
                sprintf(temp, "MTC0 %d, %d", rt, rd);
                break;
            default:
                sprintf(temp, "Unknown cp0 opcode 0x%.2X", rs);
                break;
        }

        return std::string(temp);
    }
}
