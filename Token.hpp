#pragma once
#include <string>
#include <iostream>
class Token {
public:
    enum Type {
        ADD = '+', SUB = '-', MUL = '*', DIV = '/', MOD = '%', 
        AND, OR, 
        EQU, NEQ, LT = '<', GT= '>', LEQ, GEQ,
        REM, LET, INPUT, EXIT, 
        GOTO, IF, THEN, FOR, END, 
        NUM, ID,
        LParentheses, RParentheses,
        LBrackets, RBrackets,
        INT,
        LINE_END,
        FILE_END,
        ERROR,
        ASSIGN,
        COMMA,
        SEMI_COLON
    };
    Token(Type type, int value, std::string name) :
        type(type), value(value), name(name) {}
    bool good() {
        return type != ERROR && type != FILE_END;
    }
    bool is_lineend() {
        return type == LINE_END;
    }
    void print(std::ostream &os) {
        bool nextLine = false;
        os << '[';
        switch (type) {
            case REM:   os << "REM"; break;
            case LET:   os << "LET"; break;
            case INPUT: os << "INPUT"; break;
            case EXIT:  os << "EXIT"; break;
            case GOTO:  os << "GOTO"; break;
            case IF:    os << "IF"; break;
            case THEN:  os << "THEN"; break;
            case FOR:   os << "FOR"; break;
            case END:   os << "END"; break;
            case NUM:   os << "NUM, " << value; break;
            case ID:    os << "ID, " << name; break;
            case DIV:   os << "DIV"; break;
            case MOD:   os << "MOD"; break;
            case ADD:   os << "ADD"; break;
            case SUB:   os << "SUB"; break;
            case MUL:   os << "MUL"; break;
            case LParentheses:  os << "LParentheses"; break;
            case RParentheses:  os << "RParentheses"; break;
            case LBrackets: os << "LBrackets"; break;
            case RBrackets: os << "RBrackets"; break;
            case EQU:   os << "EQU"; break;
            case NEQ:   os << "NEQ"; break;
            case LT:    os << "LT"; break;
            case GT:    os << "GL"; break;
            case LEQ:   os << "LEQ"; break;
            case GEQ:   os << "GEQ"; break;
            case AND:   os << "AND"; break;
            case OR:    os << "OR"; break;
            case INT:   os << "INT"; break;
            case LINE_END:  os << "LINE_END, " << value; nextLine = true; break;
            case FILE_END:  os << "FILE_END"; break;
            case ERROR: os << "ERROR"; break;
            case ASSIGN:    os << "ASSIGN"; break;
            case COMMA: os << "COMMA"; break;
            case SEMI_COLON:    os << "SEMI_COLON"; break;
        }
        os << ']';
        if (nextLine)
            os << '\n';
    }
    Type getType() { return type; }
    int getValue() { return value; }
    std::string getName() { return name; }
private:
    Type type;
    int value;
    std::string name;
};