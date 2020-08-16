#pragma once

#include <string>
#include <memory>
#include <iostream>

class Argv {
public:
    enum Type {
        ARGV,
        VAR_ARGV, NUM_ARGV,
        TEMP_ARGV, ARRAY_ARGV,
        REG_ARGV
    };
    Argv() {}
    virtual bool isConst() = 0;
    virtual void print(std::ostream &os) = 0;
    virtual Type getType() { return ARGV; }
};

class VarArgv : public Argv {
public:
    std::string name;
    VarArgv(std::string name) : name(name) {}
    bool isConst() { return false; }
    virtual void print(std::ostream &os) {
        os << "[" << name << "]";
    }
    virtual Type getType() { return VAR_ARGV; }
};

class NumArgv : public Argv {
public:
    int value;
    NumArgv(int value) : value(value) {}
    bool isConst() { return true; }
    virtual void print(std::ostream &os) {
        os << " " << value << " ";
    }
    virtual Type getType() { return NUM_ARGV; }
};



class TempArgv : public Argv {
public:
    int tempID;
    static int tempCount;
    TempArgv() { tempID = ++tempCount; }
    bool isConst() { return false; }
    virtual void print(std::ostream &os) {
        os << "{" << tempID << "}";
    }
    virtual Type getType() { return TEMP_ARGV; }
};

int TempArgv::tempCount = 0;

class RegArgv : public Argv {
public:
    std::string regName;
    bool isConst() { return false; }
    int regID() {
        
    }
    RegArgv(std::string name) : regName(name) {}
    virtual void print(std::ostream &os) {
        os << "{" << regName << "}";
    }
    virtual Type getType() { return REG_ARGV; }
};

class ArrayArgv : public Argv {
public:
    std::shared_ptr<VarArgv> addr;
    std::shared_ptr<TempArgv> offset;
    ArrayArgv(
        std::shared_ptr<VarArgv> addr,
        std::shared_ptr<TempArgv> offset
    ) : addr(addr), offset(offset) {}
    bool isConst() { return false; }
    virtual void print(std::ostream &os) {
        addr->print(os);
        offset->print(os);
    }
    virtual Type getType() { return ARRAY_ARGV; }
};

class Quaternion {
public:
    enum Operator {
        NOP = -1,
        ADD = '+', SUB = '-', MUL = '*', DIV = '/', MOD = '%', 
        AND, OR, 
        EQU, NEQ, LT = '<', GT= '>', LEQ, GEQ,
        LOAD, STORE, LET,
        GOTO, BEQ, BNE,
        INPUT, EXIT, JAL, 
        LINENO
    };
private:
           
public:
    int lines, tt;
    std::shared_ptr<Argv> argv1, argv2, result;
    Operator op;  
    Quaternion(
        std::shared_ptr<Argv> argv1,
        std::shared_ptr<Argv> argv2,
        std::shared_ptr<Argv> result,
        Operator op
    ) : argv1(argv1), argv2(argv2), result(result), op(op) {}
    virtual void print(std::ostream &os) {
        switch (op) {
            case NOP: os << "---\t"; break;
            case ADD: os << "ADD\t"; break;
            case SUB: os << "SUB\t"; break;
            case MUL: os << "MUL\t"; break;
            case DIV: os << "DIV\t"; break;
            case MOD: os << "MOD\t"; break;
            case AND: os << "AND\t"; break;
            case OR:  os << "OR\t";  break;
            case EQU: os << "EQU\t"; break;
            case NEQ: os << "NEQ\t"; break;
            case LT:  os << "LT\t";  break;
            case GT:  os << "GT\t";  break;
            case LEQ: os << "LEQ\t"; break;
            case GEQ: os << "GEQ\t"; break;
            case LOAD: os << "LOAD\t"; break;
            case STORE: os << "STORE\t"; break;
            case LET: os << "LET\t";  break;
            case GOTO: os << "GOTO\t"; break;
            case BEQ: os << "BEQ\t"; break;
            case BNE: os << "BNE\t"; break;
            case INPUT: os << "INPUT\t"; break;
            case EXIT: os << "EXIT\t"; break;
            case JAL: os << "JAL\t"; break;
            case LINENO: 
                argv1->print(os); 
                os << std::endl; 
                return;
        }
        if (argv1 != nullptr)
            argv1->print(os);
        else os << "---";
        os << "\t";
        if (argv2 != nullptr)
            argv2->print(os);
        else os << "---";
        os << "\t";
        if (result != nullptr)
            result->print(os);
        else os << "---";
        os << "\t";
        os << std::endl;
    }
};
