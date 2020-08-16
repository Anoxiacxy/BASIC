#pragma once
#include <memory>
#include <map>
#include <stack>
#include "ASTNode.hpp"
#include "Lexer.hpp"
#include "Error.hpp"
class Parser {
private:
    std::vector<Token> tokens;
    Token::Type type;
    int value;
    std::string name;
    int index, total, indexOfEnd;
    int lineno;
    std::stack<int>indexOfFor;
    std::map<Token::Type, int>BinopPrecedence;
    
    bool getNextToken() {
        if (index >= total) return false;
        value = tokens[index].getValue();
        name = tokens[index].getName();
        type = tokens[index].getType();
        index++; return true;
    }
    int getTokPrecedence() {
        if (BinopPrecedence.count(type))
            return 16 - BinopPrecedence[type];
        else 
            return -1;
    }
    int getNextEndFor() {
        int curIndex = index + 1;
        int k = 1;
        while (curIndex < total) {
            if (tokens[curIndex].getType() == Token::FOR)
                k++;
            else if (tokens[curIndex].getType() == Token::END) {
                if (k == 1)
                    return tokens[curIndex - 1].getValue();
                else {
                    k--;
                    curIndex++;
                }
            }
            curIndex++;
        }
        return 0;
    }
    int getLastFor() {
        if (indexOfFor.size())
            return indexOfFor.top();
        else 
            return 0;
    }

public:
    Parser(std::vector<Token> &tokens) : tokens(tokens) {
        index = 0; 
        total = tokens.size();
        indexOfEnd = 0;

        BinopPrecedence[Token::COMMA] = 15;
        BinopPrecedence[Token::OR] = 12;
        BinopPrecedence[Token::AND] = 11;
        
        BinopPrecedence[Token::NEQ] = 7;
        BinopPrecedence[Token::EQU] = 7;

        BinopPrecedence[(Token::Type)'<'] = 6;
        BinopPrecedence[(Token::Type)'>'] = 6;
        BinopPrecedence[Token::LEQ] = 6;
        BinopPrecedence[Token::GEQ] = 6;

        BinopPrecedence[(Token::Type)'+'] = 4;
        BinopPrecedence[(Token::Type)'-'] = 4;

        BinopPrecedence[(Token::Type)'*'] = 3;
        BinopPrecedence[(Token::Type)'/'] = 3;
        BinopPrecedence[(Token::Type)'%'] = 3;
        
    }
    // Program := (Statement)* 
    std::shared_ptr<Program> parseProgram() {
        std::shared_ptr<Program> program(new Program());
        while (true) {
            if (!getNextToken()) break; 
            auto stmt = parseStatement();
            if (stmt != nullptr) {
                stmt->setLineNo(lineno);
                program->addStatement(std::move(stmt));
            }
            if (type != Token::LINE_END) {
                while (type != Token::LINE_END) 
                    getNextToken();
                logError("Statement did not end properly!", lineno);
            }
        }
        return std::move(program);
    }
    // Statement := GotoStmtAST
    //           := IfStmtAST
    //           := ForStmtAST
    //           := EndForStmtAST
    //           := InputStmtAST
    //           := LetStmtAST
    //           := ExitStmtAST
    std::shared_ptr<StmtAST> parseStatement() {
        lineno = value;
        getNextToken();
        if (type == Token::LINE_END) return nullptr;
        std::shared_ptr<StmtAST> stmt;
        // GOTO
        stmt = std::move(parseGotoStmt());
        if (stmt != nullptr) return std::move(stmt);
        // IF
        stmt = std::move(parseIfStmt());
        if (stmt != nullptr) return std::move(stmt);
        // FOR
        stmt = std::move(parseForStmt());
        if (stmt != nullptr) return std::move(stmt);
        // END FOR
        stmt = std::move(parseEndForStmt());
        if (stmt != nullptr) return std::move(stmt);
        // Input
        stmt = std::move(parseInputStmt());
        if (stmt != nullptr) return std::move(stmt);
        // Let
        stmt = std::move(parseLetStmt());
        if (stmt != nullptr) return std::move(stmt);
        // Exit
        stmt = std::move(parseExitStmt());
        if (stmt != nullptr) return std::move(stmt);
        // ASSIGN
        // stmt = std::move(parseAssignStmt());
        // if (stmt != nullptr) return std::move(stmt);
        return logError("invalid statement!", lineno), nullptr;
    }
    // GotoStmtAST := GOTO {Number}
    std::shared_ptr<StmtAST> parseGotoStmt() {
        if (type != Token::GOTO) return nullptr;
        getNextToken();
        auto target = parseNumberExpr();
        if (!target) return logError("missing a number of line for GOTO!", lineno), nullptr;
        return std::make_shared<GotoStmtAST>(std::move(target));
    }
    // IfStmtAST := IF {Expr} THEN {Number}
    std::shared_ptr<StmtAST> parseIfStmt() {
        if (type != Token::IF) return nullptr;
        getNextToken();
        auto cond = parseExpression();
        if (!cond) return logError("missing IF condition!", lineno),nullptr;
        if (type != Token::THEN) return logError("missing keyword 'THEN'!", lineno), nullptr;
        getNextToken();
        auto target = parseNumberExpr();
        if (!target) return logError("missing IF target!", lineno), nullptr;;
        return std::make_shared<IfStmtAST>(std::move(cond), std::move(target));
    }
    // ForStmtAST := For {AssignExpr}; {Expr}
    std::shared_ptr<StmtAST> parseForStmt() {
        if (type != Token::FOR) return nullptr;
        getNextToken();
        auto stmt = parseAssignExpr();
        if (!stmt) 
            return logError("missing FOR loop assignment statement", lineno), nullptr;
        if (type != Token::SEMI_COLON) return logError("missing ';' in FOR loop", lineno), nullptr;
        getNextToken();
        auto exp = parseExpression();
        if (!exp) return logError("missing FOR condition", lineno), nullptr;
        int endForNo = getNextEndFor();
        if (!endForNo) return logError("missing keyword END for this FOR statement", lineno), nullptr;
        std::shared_ptr<ExprAST> target(new NumberExprAST(endForNo + 1));
        indexOfFor.push(lineno);
        return std::make_shared<ForStmtAST>(std::move(stmt), std::move(exp), std::move(target));
    }
    // EndForStmtAST := END FOR
    std::shared_ptr<StmtAST> parseEndForStmt() {
        if (type != Token::END) return nullptr;
        getNextToken();
        if (type != Token::FOR) return logError("missing END label", lineno), nullptr;
        getNextToken();
        int forNo = getLastFor();
        if (!forNo) return logError("missing FOR statement to END", lineno), nullptr;
        indexOfFor.pop();
        std::shared_ptr<ExprAST> target(new NumberExprAST(forNo));
        return std::make_shared<EndForStmtAST>(std::move(target));
    }
    // InputStmtAST := INPUT 
    std::shared_ptr<StmtAST> parseInputStmt() {
        if (type != Token::INPUT) return nullptr;
        std::vector<std::shared_ptr<ExprAST> > vars;
        do {
            getNextToken();
            auto var = parseVariableExpr();
            if (!var) return logError("there must be at least one parameter for INPUT statement", lineno), nullptr;
            vars.push_back(std::move(var));
        } while (type == Token::COMMA);
        return std::make_shared<InputStmtAST>(vars);
    }

    std::shared_ptr<StmtAST> parseLetStmt() {
        if (type != Token::LET) return nullptr;
        getNextToken();
        auto Lvalue = parsePrimary();
        if (Lvalue == nullptr) return logError("missing a primary expression", lineno), nullptr;
        if (type != Token::ASSIGN) return logError("expected '='!", lineno), nullptr;
        getNextToken();
        auto Rvalue = parseExpression();
        if (Rvalue == nullptr) return logError("missing a expression", lineno), nullptr;
        return std::make_shared<LetStmtAST>(Lvalue, Rvalue);
    }
    std::shared_ptr<StmtAST> parseExitStmt() {
        if (type != Token::EXIT) return nullptr;
        getNextToken();
        auto val = parseExpression();
        if (!val) return logError("missing a value for EXIT!", lineno), nullptr;
        return std::make_shared<ExitStmtAST>(std::move(val));
    }
    std::shared_ptr<ExprAST> parseAssignExpr() {
        auto var = parsePrimary();
        if (!var) return nullptr;
        if (type != Token::ASSIGN) return nullptr;
        getNextToken();
        auto val = parseExpression();
        if (!val) return nullptr;
        return std::make_shared<AssignExprAST>(std::move(var), std::move(val));
    }

    
    // number
    std::shared_ptr<ExprAST> parseNumberExpr() {
        auto result = std::make_shared<NumberExprAST>(value);
        getNextToken();
        return std::move(result);
    }
    // '(' expression ')'
    std::shared_ptr<ExprAST> parseParenExpr() {

        getNextToken();
        auto result = parseExpression();
        if (!result) return nullptr;
        if (type != Token::RParentheses)
            return nullptr;
        getNextToken();
        return result;
    }

    // identifier
    std::shared_ptr<ExprAST> parseVariableExpr() {
        std::string varname = name;
        getNextToken();
        if (type == Token::LBrackets) {
            std::vector<std::shared_ptr<ExprAST> > idxs;
            do {
                getNextToken();
                auto idx = parseExpression();
                if (!idx) return nullptr;
                idxs.push_back(std::move(idx));
                if (type != Token::RBrackets) 
                    return nullptr;
                getNextToken();
            } while (type == Token::LBrackets);
            return std::make_shared<ArrayExprAST>(varname, idxs);
        } else 
            return std::make_shared<IdentifierExprAST>(varname);
    }
    std::shared_ptr<ExprAST> parseIntExpr() {
        if (type != Token::INT) return nullptr;
        getNextToken();
        std::vector<std::shared_ptr<ExprAST> > sizes;
        while (type == Token::LBrackets) {
            getNextToken();
            auto siz = parseExpression();
            if (!siz) return nullptr;
            sizes.push_back(std::move(siz));
            if (type != Token::RBrackets) 
                return nullptr;
            getNextToken();
        }
        return std::make_shared<IntExprAST>(sizes);
    }
    // primary
    //   ::= identifierexpr
    //   ::= numberexpr
    //   ::= parenexpr
    std::shared_ptr<ExprAST> parsePrimary() {
        switch (type) {
        default:
            return logError("unknown token when expecting an expression"), nullptr;
        case Token::INT:
            return parseIntExpr();
        case Token::ID:
            return parseVariableExpr();
        case Token::NUM:
            return parseNumberExpr();
        case Token::LParentheses: // '('
            return parseParenExpr();
        case Token::SUB:
            getNextToken();
            value = -value;
            return parseNumberExpr();
        case Token::ADD:
            return parseNumberExpr();
        }
    }

    std::shared_ptr<ExprAST> parseExpression() {
        auto lhs = parsePrimary();
        if (!lhs) return nullptr;
        return parseBinOpRHS(0, std::move(lhs));
    }
    
    std::shared_ptr<ExprAST> parseBinOpRHS(
        int ExprPrec, 
        std::shared_ptr<ExprAST> lhs) {
        while (true) {
            int curPrec = getTokPrecedence();
            if (curPrec < ExprPrec) 
                return lhs;
            Token::Type binOp = type;
            getNextToken();

            auto rhs = parsePrimary();
            if (!rhs) 
                return nullptr;
            
            int nextPrec = getTokPrecedence();
            if (curPrec < nextPrec) {
                rhs = parseBinOpRHS(curPrec + 1, std::move(rhs));
                if (!rhs)
                    return nullptr;
            }
            lhs = std::make_shared<BinaryExprAST>((BinaryExprAST::Operator)binOp, std::move(lhs), std::move(rhs));
        }
    }
};