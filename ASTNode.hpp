#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <map>
#include "Quaternion.hpp"
#include "Error.hpp"

const std::string TAB = "\t";

enum ASTNodeType {
    PROGRAM,
    EXPR, STMT, 
    VARIABLE_EXPR, NUMBER_EXPR, IDENTIFIER_EXPR, ARRAY_EXPR, ASSIGN_EXPR, BINARY_EXPR, INT_EXPR,
    GOTO_STMT, IF_STMT, FOR_STMT, END_FOR_STMT, INPUT_STMT, LET_STMT, EXIT_STMT
};

void addNumber(int n, std::map<std::string, std::vector<int> > &varName_arrDim) {
    std::vector<int> num;
    num.push_back(n);
    varName_arrDim["= " + std::to_string(n)] = num;
}

class AbstractSyntaxTreeNode {
    int lineno;
public:
    AbstractSyntaxTreeNode() {}
    virtual ~AbstractSyntaxTreeNode() {}
    virtual void print(std::ostream &os, int deep) {}
    virtual ASTNodeType getNodeType() = 0;
    virtual std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) = 0;
    virtual void setLineNo(int line) { lineno = line; }
    virtual int getLineNo() { return lineno; }
};

class ExprAST : public AbstractSyntaxTreeNode {
public:
    virtual ~ExprAST() {}
    virtual void print(std::ostream &os, int deep) {}
    virtual ASTNodeType getNodeType() { return EXPR; }
    virtual std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { return nullptr; }
};

class StmtAST : public AbstractSyntaxTreeNode {
public:
    virtual ~StmtAST() {}
    virtual void print(std::ostream &os, int deep) {}
    virtual ASTNodeType getNodeType() { return STMT; }
    virtual std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { return nullptr; }
};
// a | a[b]
class VariableExprAST : public ExprAST {
public:
    virtual ~VariableExprAST() {}
    virtual void print(std::ostream &os, int deep) {}
    virtual ASTNodeType getNodeType() { return VARIABLE_EXPR; }
    virtual std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { return nullptr; }
};

class Program : public AbstractSyntaxTreeNode {
    std::vector<std::shared_ptr<StmtAST> > stmts;
public:
    Program() {}
    Program(std::vector<std::shared_ptr<StmtAST> > &sts) {
        for (int i = 0; i < sts.size(); i++)
            stmts.push_back(std::move(sts[i]));
    }
    virtual ASTNodeType getNodeType() { return PROGRAM; }
    void addStatement(std::shared_ptr<StmtAST> stmt) {
        stmts.push_back(std::move(stmt));
    }
    std::vector<std::shared_ptr<StmtAST> >& getStmts() {
        return stmts;
    }
    virtual void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- Program: " << std::endl;
        for (int i = 0; i < stmts.size(); i++)
            stmts[i]->print(os, deep + 1);
    }
    virtual std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { return nullptr; }
};

class NumberExprAST : public ExprAST {
public:
    int value;    
    NumberExprAST(int value) : value(value) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- Number: " << value << std::endl;
    }
    virtual ASTNodeType getNodeType() { return  NUMBER_EXPR; }
    std::shared_ptr<Argv> toQuaternions(
        std::vector<std::shared_ptr<Quaternion> > &quaternions, 
        std::map<std::string, std::vector<int> >&varName_arrDim) { 
        addNumber(value, varName_arrDim);
        return std::make_shared<NumArgv>(value); 
    }
};

class IntExprAST : public ExprAST {
public:
    std::vector<std::shared_ptr<ExprAST> > sizes;
    virtual void setLineNo(int line) {
        ExprAST::setLineNo(line);
        for (auto e : sizes)
            e->setLineNo(line);
    }
    IntExprAST(std::vector<std::shared_ptr<ExprAST> > &_sizes) {
        for (int i = 0; i < _sizes.size(); i++)
            sizes.push_back(std::move(_sizes[i]));
    }
    virtual ASTNodeType getNodeType() { return INT_EXPR; }
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- IntExpr: " << std::endl;
        for (int i = 0; i < sizes.size(); i++)
            sizes[i]->print(os, deep + 1);
    }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, 
            std::map<std::string, std::vector<int> >&varName_arrDim
    ) { 
        int D = sizes.size();
        auto reg_s0 = std::make_shared<RegArgv>("s0");
        auto result = std::make_shared<TempArgv>();
        quaternions.push_back(std::make_shared<Quaternion>(
            reg_s0, 
            std::make_shared<NumArgv>(0), 
            result, 
            Quaternion::ADD
        ));
        if (D < 1) {
            // ERROR
        } 
        auto argv1 = sizes[0]->toQuaternions(quaternions, varName_arrDim);
        auto offset = std::make_shared<TempArgv>();
        quaternions.push_back(std::make_shared<Quaternion>(
            argv1, 
            std::make_shared<NumArgv>(0), 
            offset,
            Quaternion::ADD
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            reg_s0, 
            offset, 
            std::make_shared<NumArgv>(0), 
            Quaternion::STORE
        ));
        for (int i = 1; i < D; i++) {
            auto argv2 = sizes[i]->toQuaternions(quaternions, varName_arrDim);
            
            quaternions.push_back(std::make_shared<Quaternion>(
                reg_s0, 
                argv2, 
                std::make_shared<NumArgv>(i * 4), 
                Quaternion::STORE
            ));
            quaternions.push_back(std::make_shared<Quaternion>(
                offset, argv2, offset, Quaternion::MUL
            ));
        }
        addNumber(D * 4, varName_arrDim);
        quaternions.push_back(std::make_shared<Quaternion>(
            reg_s0, std::make_shared<NumArgv>(D * 4), reg_s0, Quaternion::ADD
        ));
        addNumber(4, varName_arrDim);
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(4), offset, offset, Quaternion::MUL
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            reg_s0, offset, reg_s0, Quaternion::ADD
        ));
        return result;
    }
};
// a 
class IdentifierExprAST : public ExprAST {
public:
    std::string name;
    IdentifierExprAST(const std::string &name) : name(name) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- Identifier: " << name << std::endl;
    }
    virtual ASTNodeType getNodeType() { return IDENTIFIER_EXPR; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
                
                if (varName_arrDim.count(name) == 0)
                    logError(std::string("undefined variable '") + name + "'!", getLineNo());
                return std::make_shared<VarArgv>(name); 
            }
};

class ArrayExprAST : public ExprAST {
    std::string name;
    std::vector<std::shared_ptr<ExprAST> > indexs;
public:
    virtual void setLineNo(int line) {
        ExprAST::setLineNo(line);
        for (auto e : indexs)
            e->setLineNo(line);
    }
    ArrayExprAST(
        const std::string &name,
        std::vector<std::shared_ptr<ExprAST> > &idxs
    ) : name(name) {
        for (int i = 0; i < idxs.size(); i++) 
            indexs.push_back(std::move(idxs[i]));
    }
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- ArrayExpr: " << name << std::endl;
        for (int i = 0; i < indexs.size(); i++)
            indexs[i]->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return ARRAY_EXPR; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        if (varName_arrDim.count(name) == 0 
        || varName_arrDim[name][0] != indexs.size() 
        || varName_arrDim[name].size() != indexs.size() + 1)
            logError(std::string("undefined variable '" + name + "'!"), getLineNo());
        for (int i = 0; i < indexs.size(); i++) {
            if (varName_arrDim[name][i + 1] != -1) {
                if (indexs[i]->getNodeType() == NUMBER_EXPR) {
                    int tmp = std::dynamic_pointer_cast<NumberExprAST>(indexs[i])->value;
                    if (tmp <  0 || tmp >= varName_arrDim[name][i + 1])
                        logError(std::string("array '" + name + "' index exceed!"), getLineNo());
                }
            }
        }
        auto addr = std::make_shared<VarArgv>(name);
        int D = indexs.size();
        if (D < 1) {
            logError(std::string("undefined variable '" + name + "'!"), getLineNo());
        }
        auto offset = std::make_shared<TempArgv>();
        quaternions.push_back(std::make_shared<Quaternion>(
            indexs[0]->toQuaternions(quaternions, varName_arrDim),
            std::make_shared<NumArgv>(0),
            offset,       
            Quaternion::ADD
        ));
        
        for (int i = 1; i < D; i++) {
            auto size = std::make_shared<TempArgv>();
            addNumber(i * 4, varName_arrDim);
            quaternions.push_back(std::make_shared<Quaternion>(
                addr, 
                std::make_shared<NumArgv>(i),
                size,   
                Quaternion::LOAD
            ));
            quaternions.push_back(std::make_shared<Quaternion>(
                offset, 
                size,
                offset,       
                Quaternion::MUL
            ));
            quaternions.push_back(std::make_shared<Quaternion>(
                offset, 
                indexs[i]->toQuaternions(quaternions, varName_arrDim),
                offset,       
                Quaternion::ADD
            ));
        }
        auto temp = std::make_shared<TempArgv>();
        addNumber(D * 4, varName_arrDim);
        quaternions.push_back(std::make_shared<Quaternion>(
            offset,
            std::make_shared<NumArgv>(D),
            temp,       
            Quaternion::ADD
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            temp,
            std::make_shared<NumArgv>(4),
            temp,       
            Quaternion::MUL
        ));
        return std::make_shared<ArrayArgv>(addr, temp);
    }
};

class AssignExprAST : public ExprAST {
private:
    std::shared_ptr<ExprAST> Lvalue;
    std::shared_ptr<ExprAST> Rvalue;
public:
    virtual void setLineNo(int line) {
        ExprAST::setLineNo(line);
        Lvalue->setLineNo(line);
        Rvalue->setLineNo(line);
    }
    AssignExprAST(std::shared_ptr<ExprAST> lhs, std::shared_ptr<ExprAST> rhs) 
        : Lvalue(std::move(lhs)), Rvalue(std::move(rhs)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- AssignExprAST: " << "=" << std::endl;
        Lvalue->print(os, deep + 1);
        Rvalue->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return ASSIGN_EXPR; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        auto rst = Lvalue->toQuaternions(quaternions, varName_arrDim);
        quaternions.push_back(std::make_shared<Quaternion>(
            Rvalue->toQuaternions(quaternions, varName_arrDim),
            std::make_shared<NumArgv>(0), 
            rst,
            Quaternion::ADD
        ));
        return rst; 
    }
};

class BinaryExprAST : public ExprAST {
public:
    enum Operator {
        ADD = '+', SUB = '-', MUL = '*', DIV = '/', MOD = '%', 
        AND, OR, 
        EQU, NEQ, LT = '<', GT= '>', LEQ, GEQ
    };
    BinaryExprAST(
        Operator op,
        std::shared_ptr<ExprAST> lhs,
        std::shared_ptr<ExprAST> rhs
    ) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << "- Binary: ";
        switch (op) {
            case OR:    os << "||"; break;
            case AND:   os << "&&"; break;
            case LEQ:   os << "<="; break;
            case GEQ:   os << ">="; break;
            case EQU:   os << "=="; break;
            case NEQ:   os << "!="; break;
            default:    os << (char)op;
        }
        os << std::endl;
        
        lhs->print(os, deep + 1);
        rhs->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return BINARY_EXPR; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        auto rst = std::make_shared<TempArgv>();
        quaternions.push_back(std::make_shared<Quaternion>(
            lhs->toQuaternions(quaternions, varName_arrDim),
            rhs->toQuaternions(quaternions, varName_arrDim), 
            rst, 
            Quaternion::Operator(op)
        ));
        return rst; 
    }
    virtual void setLineNo(int line) {
        ExprAST::setLineNo(line);
        lhs->setLineNo(line);
        rhs->setLineNo(line);
    }
private:
    Operator op;
    std::shared_ptr<ExprAST> lhs, rhs;
};

class GotoStmtAST : public StmtAST {
private:
    std::shared_ptr<ExprAST> Target;
public:
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        Target->setLineNo(line);
    }
    GotoStmtAST(std::shared_ptr<ExprAST> target) 
        : Target(std::move(target)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- GotoStmt: " << std::endl;
        Target->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return GOTO_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            Target->toQuaternions(quaternions, varName_arrDim),
            std::make_shared<NumArgv>(0),
            nullptr,
            Quaternion::GOTO
        ));
        return nullptr; 
    }
};

class IfStmtAST : public StmtAST {
private:
    std::shared_ptr<ExprAST> Cond;
    std::shared_ptr<ExprAST> Target;
public:
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        Target->setLineNo(line);
        Cond->setLineNo(line);
    }
    IfStmtAST(std::shared_ptr<ExprAST> cond, std::shared_ptr<ExprAST> target) 
        : Cond(std::move(cond)), Target(std::move(target)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- IfStmt: " << std::endl;
        Cond->print(os, deep + 1);
        Target->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return IF_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            Cond->toQuaternions(quaternions, varName_arrDim),
            std::make_shared<NumArgv>(0),
            Target->toQuaternions(quaternions, varName_arrDim),
            Quaternion::BNE
        ));
        return nullptr; 
    }
};

class ForStmtAST : public StmtAST {
private:   
    std::shared_ptr<ExprAST> exp;
    std::shared_ptr<ExprAST> Target;
public:
    std::shared_ptr<ExprAST> stmt;    
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        Target->setLineNo(line);
        exp->setLineNo(line);
        stmt->setLineNo(line);
    }
    ForStmtAST(
        std::shared_ptr<ExprAST> stmt,
        std::shared_ptr<ExprAST> exp,
        std::shared_ptr<ExprAST> target
    ) : stmt(std::move(stmt)), exp(std::move(exp)), Target(std::move(target)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- ForStmt: " << std::endl;
        exp->print(os, deep + 1);
        stmt->print(os, deep + 1);
        Target->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return FOR_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        auto pos = quaternions.size();
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(0),
            std::make_shared<NumArgv>(0),
            nullptr,
            Quaternion::JAL
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        stmt->toQuaternions(quaternions, varName_arrDim);
        std::dynamic_pointer_cast<NumArgv>(quaternions[pos]->argv1)->value = quaternions.size() - pos;
        quaternions.push_back(std::make_shared<Quaternion>(
            nullptr,
            nullptr,
            nullptr,
            Quaternion::TAG
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            exp->toQuaternions(quaternions, varName_arrDim),
            std::make_shared<NumArgv>(0),
            Target->toQuaternions(quaternions, varName_arrDim),
            Quaternion::BEQ
        ));   
        return nullptr; 
    }
};

class EndForStmtAST : public StmtAST {
private:
    std::shared_ptr<ExprAST> Target;
public:
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        Target->setLineNo(line);
    }
    EndForStmtAST(
        std::shared_ptr<ExprAST> target
    ) : Target(std::move(target)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- EndForStmt: " << std::endl;
        Target->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return END_FOR_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            Target->toQuaternions(quaternions, varName_arrDim),
            std::make_shared<NumArgv>(0),
            nullptr,
            Quaternion::GOTO
        ));
        return nullptr; 
    }
};

class InputStmtAST : public StmtAST {
    std::vector<std::shared_ptr<ExprAST> > variables;
public:
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        for (auto e : variables)
            e->setLineNo(line);
    }
    InputStmtAST(std::vector<std::shared_ptr<ExprAST> > &vars) {
        for (int i = 0; i < vars.size(); i++)
            variables.push_back(std::move(vars[i]));
    }
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- InputStmt: " << std::endl;
        for (int i = 0; i < variables.size(); i++)
            variables[i]->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return INPUT_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        for (auto var : variables) if (var->getNodeType() == IDENTIFIER_EXPR) {
            int D = 0;
            auto name = std::dynamic_pointer_cast<IdentifierExprAST>(var)->name;
            if (varName_arrDim.count(name) == 0) {
                varName_arrDim[name].push_back(D);
            }
            else if (varName_arrDim[name][0] != D)
                logError(std::string("variable '" + name + "'type error"), getLineNo());
        }
        for (int i = 0; i < variables.size(); i++)
            quaternions.push_back(std::make_shared<Quaternion>(
                nullptr,
                nullptr,
                variables[i]->toQuaternions(quaternions, varName_arrDim),
                Quaternion::INPUT
            ));
        return nullptr; 
    }
};



class LetStmtAST : public StmtAST {
    std::shared_ptr<ExprAST> Lvalue;
    std::shared_ptr<ExprAST> Rvalue;
public:
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        Lvalue->setLineNo(line);
        Rvalue->setLineNo(line);
    }
    LetStmtAST(
        std::shared_ptr<ExprAST> Lvalue,
        std::shared_ptr<ExprAST> Rvalue
    ) : Lvalue(std::move(Lvalue)), Rvalue(std::move(Rvalue)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- LetStmt: " << std::endl;
        Lvalue->print(os, deep + 1);
        Rvalue->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return LET_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        if (Lvalue->getNodeType() == IDENTIFIER_EXPR) {
            int D = 0;
            if (Rvalue->getNodeType() == INT_EXPR) 
                D = std::dynamic_pointer_cast<IntExprAST>(Rvalue)->sizes.size();
            auto name = std::dynamic_pointer_cast<IdentifierExprAST>(Lvalue)->name;
            if (varName_arrDim.count(name) == 0) {
                varName_arrDim[name].push_back(D);
                if (D != 0) {
                    auto tmp = std::dynamic_pointer_cast<IntExprAST>(Rvalue)->sizes;
                    for (int i = 0; i < D; i++) {
                        if (tmp[i]->getNodeType() == NUMBER_EXPR) {
                            varName_arrDim[name].push_back(
                                std::dynamic_pointer_cast<NumberExprAST>(tmp[i])->value
                            );
                            if (varName_arrDim[name].back() < 0)
                                logError(std::string("array '" + name + "' size error!"), getLineNo());
                        }
                        else 
                            varName_arrDim[name].push_back(-1);
                    }
                }
            }
            else if (varName_arrDim[name][0] != D)
                logError(std::string("variable '" + name + "'type error!"), getLineNo());
        }
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            Rvalue->toQuaternions(quaternions, varName_arrDim),
            nullptr,
            Lvalue->toQuaternions(quaternions, varName_arrDim),
            Quaternion::LET
        ));
        
        return nullptr; 
    }
};



class ExitStmtAST : public StmtAST {
private:
    std::shared_ptr<ExprAST> value;
public:
    virtual void setLineNo(int line) {
        StmtAST::setLineNo(line);
        value->setLineNo(line);
    }
    ExitStmtAST(std::shared_ptr<ExprAST> value) 
        : value(std::move(value)) {}
    void print(std::ostream &os, int deep) {
        for (int i = 0; i < deep; i++)
            os << TAB;
        os << getLineNo() << ' ';
        os << "- ExitStmt: " << std::endl;
        value->print(os, deep + 1);
    }
    virtual ASTNodeType getNodeType() { return EXIT_STMT; }
    std::shared_ptr<Argv> toQuaternions(
            std::vector<std::shared_ptr<Quaternion> > &quaternions, std::map<std::string, std::vector<int> >&varName_arrDim) { 
        quaternions.push_back(std::make_shared<Quaternion>(
            std::make_shared<NumArgv>(getLineNo()),
            nullptr,
            nullptr,
            Quaternion::LINENO
        ));
        quaternions.push_back(std::make_shared<Quaternion>(
            value->toQuaternions(quaternions, varName_arrDim),
            nullptr,
            nullptr,
            Quaternion::EXIT
        ));
        return nullptr; 
    }
};
