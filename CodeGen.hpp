#pragma once

#include <memory>
#include <cstring>
#include <iomanip>
#include "ASTNode.hpp"
#include "CFGNode.hpp"
#include "Instruction.hpp"
using namespace sjtu;

#include <map>
#include <vector>
#include <deque>
typedef std::deque<std::pair<int, Instruction> > Riscv;

class Register {
    bool reg[32];
    std::deque<int>LRU;
public:
    Register() {
        memset(reg, true, sizeof reg);
        reg[0] = false; // zero
        reg[1] = false; // ra
        reg[2] = false; // sp
        reg[3] = false; // gp
        reg[4] = false; // tp
        reg[8] = false; // s0
        reg[9] = false; // s1
        reg[10] = false; // a0
    }

    int newRegister() {
        print(std::cerr);
        for (int i = 0; i < 32; i++) 
            if (reg[i] == true) {
                reg[i] = false;
                LRU.push_back(i);
                return i;
            }
        int x = LRU.front();
        LRU.pop_front();
        LRU.push_back(x);
        return x;
    }
    void releaseRegister(int x) {
        reg[x] = true;
        for (auto it = LRU.begin(); it != LRU.end(); it++)
            if (*it == x) {
                LRU.erase(it);
                break;
            }
    }
    void usingRegister(int x) {
        releaseRegister(x);
        reg[x] = false;
        LRU.push_back(x);
    }
    int getID(std::string str) {
        if (str == "zero") return 0;
        if (str == "ra") return 1; 
        if (str == "sp") return 2;
        if (str == "gp") return 3;
        if (str == "tp") return 4;
        if (str == "s0") return 8;
        if (str == "s1") return 9;
        if (str == "a0") return 10;
    }
    void print(std::ostream &os) {
        for (int i = 0; i < LRU.size(); i++)
            os << "[" << LRU[i] << "] ";
        os << std::endl;
    }
};

class CodeGen {
private:
    std::shared_ptr<Program> AST;    
    std::map<int, std::shared_ptr<StmtAST> > lineNo_stmt;
    std::vector<std::shared_ptr<ControlFlowGraphNode> >nodes;
    std::map<std::string, std::vector<int> >varName_arrDim;
    Register regs;
public:
    CodeGen(std::shared_ptr<Program> &AST) : AST(std::move(AST)) {}
    
    void initNodes() {
        auto curNode = std::make_shared<ControlFlowGraphNode>();
        for (auto stmt : AST->getStmts()) {
            lineNo_stmt[stmt->getLineNo()] = stmt;
            curNode->appendStmt(stmt);
            switch (stmt->getNodeType()) {
                case IF_STMT:
                case GOTO_STMT:
                case FOR_STMT:
                case END_FOR_STMT:
                    nodes.push_back(curNode);
                    curNode = std::make_shared<ControlFlowGraphNode>();
                    break;
                default:
                    nodes.push_back(curNode);
                    curNode = std::make_shared<ControlFlowGraphNode>();
                    break;
            }
        }
        nodes.push_back(curNode);
    }
    void print(std::ostream &os) {
        for (int i = 0; i < nodes.size(); i++) {
            for (auto quat : nodes[i]->quaternions) 
                quat->print(os);
            os << "----------------------" << std::endl;
        }
        for (auto var : varName_arrDim) {
            os << var.first << ' ';
            os << var.second[0] << " | ";
            for (int i = 1; i < var.second.size(); i++)
                os << var.second[i] << ' ';
            os << std::endl;
        }
    }

    void toQuaternions() {
        for (int i = 0; i < nodes.size(); i++) {
            std::vector<std::shared_ptr<Quaternion> > quaternions;
            for (auto stmt : nodes[i]->stmts) {
                stmt->toQuaternions(quaternions, varName_arrDim);
            }
            nodes[i]->quaternions = quaternions;
        }
    }
    std::map<std::string, int> varName_addr;
    std::vector<int> number;
    int varAddr = 0, numAddr = 0;
    void genVariable() {
        for (auto var : varName_arrDim) 
            if (var.first[0] != '=')
                varName_addr[var.first] = varAddr++;
            else {
                varName_addr[var.first] = numAddr++;
                int num = 0, f = 1;
                for (int i = 2; i < var.first.size(); i++) {
                    if (var.first[i] == '-') f *= -1;
                    else if (isdigit(var.first[i])) 
                        num = num * 10 + var.first[i] - '0';
                    else break;
                }
                num *= f;
                number.push_back(num);
            }
    }

    std::map<std::string, int> varName_regs;
    int tempVarAddr = 0;
    int maxTempVarAddr = 0;
    std::stack<int>oldTempVarAddr;

    void storeVariable(std::string name, Riscv &code, int &addr) {
        if (name[0] == '=') {
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::SW,   // operation
                0, regs.getID("gp"), varName_regs[name], // rd, rs1, rs2
                varName_addr[name] * 4,                  // imm
                0, 0                        // target, delta
            )));
        }
        else if (!isdigit(name[0])) {
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::SW,   // operation
                0, regs.getID("sp"), varName_regs[name], // rd, rs1, rs2
                varName_addr[name] * 4,                  // imm
                0, 0                        // target, delta
            )));
        } else {
            if (oldTempVarAddr.size())
                varName_addr[name] = oldTempVarAddr.top(), oldTempVarAddr.pop();
            else
                varName_addr[name] = tempVarAddr++;
                
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::SW,   // operation
                0, regs.getID("s1"), varName_regs[name], // rd, rs1, rs2
                varName_addr[name] * 4,                  // imm
                0, 0                        // target, delta
            )));
        }
    }

    void loadVariable(std::string name, Riscv &code, int &addr) {
        if (varName_regs.count(name)) {
            regs.usingRegister(varName_regs[name]);
        } else if (name[0] == '=') {
            int regID = regs.newRegister();
            for (auto it = varName_regs.begin(); it != varName_regs.end(); it++) 
                if (it->second == regID) {
                    storeVariable(it->first, code, addr);
                    varName_regs.erase(it);
                    break;
                }
            varName_regs[name] = regID;
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::LW,   // operation
                varName_regs[name], regs.getID("gp"), 0, // rd, rs1, rs2
                varName_addr[name] * 4,                  // imm
                0, 0                        // target, delta
            )));            

        }else if (!isdigit(name[0])) {
            int regID = regs.newRegister();
            for (auto it = varName_regs.begin(); it != varName_regs.end(); it++) 
                if (it->second == regID) {
                    storeVariable(it->first, code, addr);
                    varName_regs.erase(it);
                    break;
                }
            varName_regs[name] = regID;
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::LW,   // operation
                varName_regs[name], regs.getID("sp"), 0, // rd, rs1, rs2
                varName_addr[name] * 4,                  // imm
                0, 0                        // target, delta
            )));
        } else {
            int regID = regs.newRegister();
            for (auto it = varName_regs.begin(); it != varName_regs.end(); it++) 
                if (it->second == regID) {
                    storeVariable(it->first, code, addr);
                    varName_regs.erase(it);
                    break;
                }
            varName_regs[name] = regID;
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::LW,   // operation
                varName_regs[name], regs.getID("s1"), 0, // rd, rs1, rs2
                varName_addr[name] * 4,                  // imm
                0, 0                        // target, delta
            )));
            oldTempVarAddr.push(varName_addr[name]);
            varName_addr.erase(name);
        }
    }

    void clearTempVariables(std::shared_ptr<ControlFlowGraphNode> node, Riscv &code, int &addr) {
        maxTempVarAddr = std::max(maxTempVarAddr, tempVarAddr);
        while (oldTempVarAddr.size()) oldTempVarAddr.pop();
        tempVarAddr = 0;
        for (auto var : varName_regs) 
            if (isdigit(var.first[0])) {
                regs.releaseRegister(var.second);
                varName_regs.erase(var.first);
            } else if (var.first[0] == '=') {
                regs.releaseRegister(var.second);
                varName_regs.erase(var.first);
            } else {
                
            }
        //varName_regs.clear();
    }
    std::vector<std::pair<std::string, int> > arrayArgv;
    void clearArrayVariables(Riscv &code, int &addr) {
        for (auto p : arrayArgv) {
            auto str = p.first;
            auto offset = p.second;
            code.push_back(std::make_pair(addr++, Instruction(
                InstructionOperation::SW,   // operation
                0, offset, varName_regs[str], // rd, rs1, rs2
                0, 0, 0                        // target, delta
            )));
            regs.releaseRegister(varName_regs[str]);
            varName_regs.erase(str);
        }
        arrayArgv.clear();
    }
    
    std::pair<bool, int> loadArgv(std::shared_ptr<Argv> argv, 
        Riscv &code, int &addr) {
        switch (argv->getType()) {
            case Argv::VAR_ARGV: {
                std::string name = std::dynamic_pointer_cast<VarArgv>(argv)->name; 
                loadVariable(name, code, addr);
                return std::make_pair(false, varName_regs[name]);
            }
            case Argv::NUM_ARGV: {
                int val = std::dynamic_pointer_cast<NumArgv>(argv)->value;
                if (val == 0) 
                    return std::make_pair(false, regs.getID("zero"));
                std::string str = "= " + std::to_string(val);
                if (varName_addr.count(str)) {
                    loadVariable(str, code, addr);
                    return std::make_pair(false, varName_regs[str]);
                }
                else 
                    return std::make_pair(true, val);
            }   
            case Argv::TEMP_ARGV: {
                int id = std::dynamic_pointer_cast<TempArgv>(argv)->tempID;
                std::string str = std::to_string(id);
                if (varName_regs.count(str))
                    return std::make_pair(false, varName_regs[str]);
                else if (varName_addr.count(str)) {
                    loadVariable(str, code, addr);
                    return std::make_pair(false, varName_regs[str]);
                } else {
                    int regID = regs.newRegister();
                    std::cerr << "new regID = " << regID << std::endl;
                    for (auto it = varName_regs.begin(); it != varName_regs.end(); it++) 
                        if (it->second == regID) {
                            storeVariable(it->first, code, addr);
                            varName_regs.erase(it);
                            break;
                        }
                    varName_regs[str] = regID;
                    return std::make_pair(false, varName_regs[str]);
                }
            }
            case Argv::ARRAY_ARGV: {
                auto tmp = std::dynamic_pointer_cast<ArrayArgv>(argv);
                auto pos = loadArgv(tmp->addr, code, addr).second;
                auto offset = loadArgv(tmp->offset, code, addr).second;
                code.push_back(std::make_pair(addr++, Instruction(
                    InstructionOperation::ADD,   // operation
                    offset, 
                    pos, 
                    offset, // rd, rs1, rs2
                    0, 0, 0               // imm, target, delta
                )));
                std::string str = pos + "[" + std::to_string(offset) + "]";
                varName_regs[str] = regs.newRegister();
                code.push_back(std::make_pair(addr++, Instruction(
                    InstructionOperation::LW,   // operation
                    varName_regs[str], offset, 0, // rd, rs1, rs2
                    0, 0, 0                        // target, delta
                )));
                arrayArgv.emplace_back(str, offset);
                return std::make_pair(false, varName_regs[str]);
            }
            case Argv::REG_ARGV:
                auto str = std::dynamic_pointer_cast<RegArgv>(argv)->regName;
                return std::make_pair(false, regs.getID(str));
        }
        
    }

    std::map<int, int>lineNo_addr;
    Riscv code;
    int toInstructions() {
        int addr = 0;
        for (auto node : nodes) {
            for (auto quat : node->quaternions) {
                int sAddr = addr;
                //std::cerr << __LINE__ << ' ' << addr << std::endl;
                arrayArgv.clear();
                std::pair<bool, int> a1, a2, rs;
                if (quat->op != Quaternion::LINENO) {
                    if (quat->argv1 != nullptr) 
                        a1 = loadArgv(quat->argv1, code, addr);
                    if (quat->argv2 != nullptr) 
                        a2 = loadArgv(quat->argv2, code, addr);
                    if (quat->result != nullptr) 
                        rs = loadArgv(quat->result, code, addr);
                }
                
                quat->tt = addr - sAddr;
                //std::cerr << __LINE__ << ' ' << addr << std::endl;
                switch (quat->op) {
                    case Quaternion::NOP: break;
                    case Quaternion::ADD: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADD,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::SUB: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::SUB,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::MUL: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::MUL,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::DIV: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::DIV,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::MOD: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::REM,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::AND: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::AND,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::OR: { 
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::OR,   // operation
                            rs.second,
                            a1.second,  
                            a2.second,
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::EQU: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            regs.getID("zero"),
                            0, 
                            0, 0, 0                        // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BNE,   // operation
                            0,
                            a1.second,  
                            a2.second,
                            2 * 4, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            rs.second, 
                            0, 
                            1, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::NEQ: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            regs.getID("zero"),
                            0, 
                            0, 0, 0                        // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BEQ,   // operation
                            0,
                            a1.second,  
                            a2.second,
                            2 * 4, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            rs.second, 
                            0, 
                            1, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::LT: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            regs.getID("zero"),
                            0, 
                            0, 0, 0                        // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BGE,   // operation
                            0,
                            a1.second,  
                            a2.second,
                            2 * 4, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            rs.second, 
                            0, 
                            1, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::GT: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            regs.getID("zero"),
                            0, 
                            0, 0, 0                        // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BGE,   // operation
                            0,
                            a2.second,  
                            a1.second,
                            2 * 4, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            rs.second, 
                            0, 
                            1, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::LEQ: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            regs.getID("zero"),
                            0, 
                            0, 0, 0                        // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BLT,   // operation
                            0,
                            a2.second,  
                            a1.second,
                            2 * 4, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            rs.second, 
                            0, 
                            1, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::GEQ: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            regs.getID("zero"),
                            0, 
                            0, 0, 0                        // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BLT,   // operation
                            0,
                            a1.second,  
                            a2.second,
                            2 * 4, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            rs.second,
                            rs.second, 
                            0, 
                            1, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::LOAD: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::LW,   // operation
                            rs.second, a1.second, 0, // rd, rs1, rs2
                            std::dynamic_pointer_cast<NumArgv>(quat->argv2)->value * 4,                  // imm
                            0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::STORE: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::SW,   // operation
                            0, a1.second, a2.second, // rd, rs1, rs2
                            std::dynamic_pointer_cast<NumArgv>(quat->result)->value * 4,                    // imm
                            0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::LET: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADD,   // operation
                            rs.second, a1.second, regs.getID("zero"), // rd, rs1, rs2
                            0, 0, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::GOTO: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::JAL,   // operation
                            regs.getID("ra"), 0, 0, // rd, rs1, rs2
                            0, std::dynamic_pointer_cast<NumArgv>(quat->argv1)->value, 0                        // target, delta
                        )));
                    } break;
                    case Quaternion::BEQ: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BEQ,   // operation
                            0,
                            a1.second,  
                            a2.second,
                            0, std::dynamic_pointer_cast<NumArgv>(quat->result)->value, 0          // target, delta
                        )));
                    } break;
                    case Quaternion::BNE: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::BNE,   // operation
                            0,
                            a1.second,  
                            a2.second,
                            0, std::dynamic_pointer_cast<NumArgv>(quat->result)->value, 0          // target, delta
                        )));
                    } break;
                    case Quaternion::INPUT: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::INPUT,   // operation
                            rs.second,
                            0,  
                            0,
                            0, 0, 0          // target, delta
                        )));
                    } break;
                    case Quaternion::EXIT: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            regs.getID("a0"),
                            a1.second,  
                            0,
                            0, 0, 0          // target, delta
                        )));
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::ADDI,   // operation
                            regs.getID("a0"),
                            regs.getID("zero"),  
                            0,
                            255, 0, 0          // target, delta
                        )));
                    } break;
                    case Quaternion::JAL: {
                        code.push_back(std::make_pair(addr++, Instruction(
                            InstructionOperation::JAL,   // operation
                            regs.getID("ra"), 0, 0, // rd, rs1, rs2
                            0, 0, std::dynamic_pointer_cast<NumArgv>(quat->argv1)->value                   // target, delta
                        )));
                    } break;
                    case Quaternion::LINENO:
                        lineNo_addr[
                            std::dynamic_pointer_cast<NumArgv>(quat->argv1)->value
                        ] = addr;
                        break;
                }
                clearArrayVariables(code, addr);
                int eAddr = addr;
                //std::cerr << __LINE__ << ' ' << addr << std::endl;
                quat->lines = eAddr - sAddr;
            }
            clearTempVariables(node, code, addr);
        }
        lineNo_addr[0x7fffffff] = addr;
        code.push_back(std::make_pair(addr++, Instruction(
            InstructionOperation::ADDI,   // operation
            regs.getID("a0"),
            regs.getID("zero"),
            0,
            0, 0, 0          // target, delta
        )));
        code.push_back(std::make_pair(addr++, Instruction(
            InstructionOperation::ADDI,   // operation
            regs.getID("a0"),
            regs.getID("zero"),  
            0,
            255, 0, 0          // target, delta
        )));
        int nowAddr = 0;
        for (auto node : nodes) 
            for (int i = 0; i < node->quaternions.size(); i++) {
                auto quat = node->quaternions[i]; 
                nowAddr += quat->tt;
                switch (quat->op) {
                    case Quaternion::GOTO:
                        code[nowAddr].second.imm = (lineNo_addr.lower_bound(code[nowAddr].second.target)->second - nowAddr) * 4;
                        break;
                    case Quaternion::BEQ:
                        if (code[nowAddr].second.imm == 0) {
                            code[nowAddr].second.imm = (lineNo_addr.lower_bound(code[nowAddr].second.target)->second - nowAddr) * 4;
                        }
                        break;
                    case Quaternion::BNE:
                        if (code[nowAddr].second.imm == 0) {
                            code[nowAddr].second.imm = (lineNo_addr.lower_bound(code[nowAddr].second.target)->second - nowAddr) * 4;
                        } break;
                    case Quaternion::JAL:
                        code[nowAddr].second.imm = 0;
                        for (int j = 0; j < code[nowAddr].second.delta; j++)
                            code[nowAddr].second.imm += node->quaternions[i + j]->lines;
                        code[nowAddr].second.imm -= quat->tt;
                        code[nowAddr].second.imm *= 4;
                        break;
                }
                nowAddr -= quat->tt;
                nowAddr += quat->lines;
            }
        //addr; // pc
        //maxTempVarAddr; // s1
        //varAddr;    // sp
        //numAddr;    // gp
        // array       // s0
        return addr;
    }
    
    void gen() {
        initNodes();
        toQuaternions();    
        genVariable();
        int addr = toInstructions();
        
        code.push_front(std::make_pair(addr++, Instruction(
            InstructionOperation::ADDI,
            regs.getID("s0"), regs.getID("s1"), 0, 
            (maxTempVarAddr + 0x10) * 4, 0, 0
        )));

        code.push_front(std::make_pair(addr++, Instruction(
            InstructionOperation::ADDI,
            regs.getID("s1"), regs.getID("sp"), 0,
            (varAddr + 0x10) * 4, 0, 0
        )));

        code.push_front(std::make_pair(addr++, Instruction(
            InstructionOperation::LUI,
            regs.getID("sp"), 0, 0,
            0x20000, 0, 0
        )));
        
        code.push_front(std::make_pair(addr++, Instruction(
            InstructionOperation::LUI,
            regs.getID("gp"), 0, 0,
            0x10000, 0, 0
        )));
        
        numAddr = 0x10000;
    }

    void printDump(std::ostream &os) {
        for (auto inst : code) {
            inst.second.genInstruction()->printDump(os);
            os << std::endl;
        }
    }

    void printCode(std::ostream &os) {
        os << "@" << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(8) << std::setfill('0') << 0 << std::endl;
        int instCount = 0;
        for (auto inst : code) {
            instCount++;
            inst.second.genInstruction()->print(os);
            if (instCount % 4 == 0)
                os << std::endl;
        }
        os << std::endl;
        if (!number.size()) return;
        instCount = 0;
        os << "@" << std::setiosflags(std::ios::uppercase) << std::hex << std::setw(8) << std::setfill('0') << numAddr << std::endl;
        for (auto num : number) {
            instCount++;
            Instruction(num).print(os);
            if (instCount % 4 == 0)
                os << std::endl;
        }
        os << std::endl;
    }
};