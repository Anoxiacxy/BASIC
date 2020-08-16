#pragma once

#include <iostream>
#include <iomanip>

namespace sjtu {

enum InstructionOperation {
    NOP, LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, DIV, MUL, REM, INPUT
};
enum InstructionOpcode {
    OC_NOP     = 0b0000000,
    OC_LUI     = 0b0110111,
    OC_AUIPC   = 0b0010111,
    OC_JAL     = 0b1101111,
    OC_JALR    = 0b1100111,
    OC_BRANCH  = 0b1100011,
    OC_LOAD    = 0b0000011,
    OC_STORE   = 0b0100011,
    OC_OP_IMM  = 0b0010011,
    OC_OP      = 0b0110011,
    OC_INPUT   = 0b1111111
};

int pc = 0;

class Instruction {
public:
    uint32_t ins;
    InstructionOperation op;
    int rd, rs1, rs2;
    int imm;
    int target, delta;
    Instruction(uint32_t ins) : ins(ins) {}
    Instruction(InstructionOperation op, int rd, int rs1, int rs2, int imm, int target, int delta) : 
        op(op), rd(rd), rs1(rs1), rs2(rs2), imm(imm), target(target), delta(delta) {

    }
    Instruction * genInstruction() {
        switch (op) {
            case NOP: ins = 0; break;
            case LUI: ins = ((imm >> 12) << 12) | (rd << 7) | 0b0110111; break;
            case JAL: ins = ((imm & 0x7fe) << 20) | (((imm >> 20) & 1) << 31) | (((imm >> 11) & 1) << 20) | (((imm >> 12) & 0xff) << 12) | 0b1101111; break;
            case BEQ: ins = (((imm >> 11) & 1) << 7) | (((imm >> 12) & 1) << 31) | (((imm >> 5) & 0x3f) << 25) | ((imm & 0b11110) << 7); ins |= 0b1100011| (rs2 << 20) | (rs1 << 15); break;
            case BNE: ins = (((imm >> 11) & 1) << 7) | (((imm >> 12) & 1) << 31) | (((imm >> 5) & 0x3f) << 25) | ((imm & 0b11110) << 7); ins |= (0b001 << 12) | 0b1100011| (rs2 << 20) | (rs1 << 15); break;
            case BLT: ins = (((imm >> 11) & 1) << 7) | (((imm >> 12) & 1) << 31) | (((imm >> 5) & 0x3f) << 25) | ((imm & 0b11110) << 7); ins |= (0b100 << 12) | 0b1100011| (rs2 << 20) | (rs1 << 15); break;
            case BGE: ins = (((imm >> 11) & 1) << 7) | (((imm >> 12) & 1) << 31) | (((imm >> 5) & 0x3f) << 25) | ((imm & 0b11110) << 7); ins |= (0b101 << 12) | 0b1100011| (rs2 << 20) | (rs1 << 15); break;
            case LW: ins = (imm << 20) | (rs1 << 15) | (rd << 7) | (0b010 << 12) | 0b0000011; break;
            case SW: ins = ((imm & 0b11111) << 7) | (((imm >> 5) & 0x7f) << 25) | (rs2 << 20) | (rs1 << 15) | (1 << 13) | 0b0100011; break;
            case ADDI: ins = (imm << 20) | (rs1 << 15) | (rd << 7) | 0b0010011; break;
            case ADD: ins = (rs2 << 20) | (rs1 << 15) | (rd << 7) | 0b0110011; break;
            case SUB: ins = (0b0100000 << 25) | (rs2 << 20) | (rs1 << 15) | (rd << 7) | 0b0110011; break;
            case MUL: ins = (0b0000001 << 25) | (rs2 << 20) | (rs1 << 15) | (rd << 7) | 0b0110011; break;
            case DIV: ins = (0b0000001 << 25) | (rs2 << 20) | (rs1 << 15) | (0b100) << 12 | (rd << 7) | 0b0110011; break;
            case REM: ins = (0b0000001 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110) << 12 | (rd << 7) | 0b0110011; break;
            case OR: ins = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110) << 12 | (rd << 7) | 0b0110011; break;
            case AND: ins = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b111) << 12 | (rd << 7) | 0b0110011; break;
            case INPUT: ins = (rd << 7) | 0b1111111; break;
        }
        return this;
    }
    void printDump(std::ostream &os) {
        genInstruction();
        os << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(4) << std::setfill('0') << (pc++ * 4) << ' ';
        os << std::dec;
        switch (op) {
            case NOP: os << "NOP  "; break;
            case LUI: os << "LUI  " << '\t' << "rd_" << rd << '\t'
                << "rs1_" << 0 << '\t' << "rs2_" << 0 << '\t' << "imm_" << imm; break;
            case JAL: 
                os << "JAL  " << '\t' << "rd_" << 0 << '\t' << "rs1_" << 0 << '\t' << "rs2_" << 0 << '\t' << "imm_" << imm; break;
            case BEQ: 
                os << "BEQ  " << '\t' << "rd_" << 0 << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case BNE: os << "BNE  " << '\t' << "rd_" << 0 << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case BLT: os << "BLT  " << '\t' << "rd_" << 0 << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case BGE: os << "BGE  " << '\t' << "rd_" << 0 << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case LW: os << "LW   " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case SW: os << "SW   " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case ADDI: os << "ADDI " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case ADD: os << "ADD  " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case SUB: os << "SUB  " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case MUL: os << "MUL  " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case DIV: os << "DIV  " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case REM: os << "REM  " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case OR: os << "OR   " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case AND: os << "AND  " << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
            case INPUT: os << "INPUT" << '\t' << "rd_" << rd << '\t' << "rs1_" << rs1 << '\t' << "rs2_" << rs2 << '\t' << "imm_" << imm; break;
        }
        os << '\t' << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(8) << std::setfill('0') << (ins) << ' ';
        os << std::dec;
    }
    void print(std::ostream &os) {
        os << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(2) << std::setfill('0') << (ins & 255) << ' ';
        ins >>= 8;
        os << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(2) << std::setfill('0') << (ins & 255) << ' ';
        ins >>= 8;
        os << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(2) << std::setfill('0') << (ins & 255) << ' ';
        ins >>= 8;
        os << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(2) << std::setfill('0') << (ins & 255) << ' ';
    }
};

}

