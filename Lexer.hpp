#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Token.hpp"
class Lexer {
public:
    Lexer(std::istream &is) : is(is) {
        id = 0;
        lineno = 1;
        init_keywords();
        code.push_back("");
    }   
    Token getToken() {
        int value = 0;
        std::string name = "";
        int type = lexan(value, name);
        return Token((Token::Type)type, value, name);
    }
    void print(std::ostream &os) {
        for (auto token : tokens)
            token.print(os);
    }
    std::vector<Token> &getTokens() {
        
        tokens.clear();
        /*
        for (Token cur = getToken(); cur.good(); cur = getToken()) 
            tokens.push_back(cur);
            */
        Token cur = getToken();
        while (cur.good()) {
            tokens.push_back(cur);
            cur = getToken();
        }
        if (tokens.size() && !tokens[tokens.size() - 1].is_lineend())
            tokens.push_back(Token(Token::LINE_END, lineno, ""));
        return tokens;
    }
    std::vector<std::string>code;
    std::vector<std::string> getCode() {
        return code;
    }
    void printCode(std::ostream &os) {
        for (auto line : code)
            os << line;
        os << std::endl;
    }
private:
    std::vector<Token>tokens;
    std::istream &is;
    std::map<std::string, int>symtable;
    uint id, lineno;
    
    int lexan(int &tokenval, std::string &lexbuf) {
        bool REM_line = false;
        for (char t = is.get(); is.good(); is.get(t)) {
            code.back() += t;
            if (t == '\n') {
                tokenval = lineno++;
                code.push_back("");
                return Token::LINE_END;
            }
            else if (REM_line) 
                ;
            else if (t == ' ' || t == '\t' || t == '\r')
                ;
            else if (isdigit(t)) {
                tokenval = t - '0';
                t = is.get();
                while (isdigit(t)) {
                    code.back() += t;
                    tokenval = tokenval * 10 + t - '0';
                    t = is.get();
                }
                is.unget();
                return Token::Type::NUM;
            } else if (isalpha(t)) {
                lexbuf = "";
                lexbuf += t;
                t = is.get();
                while (isalpha(t)) {
                    code.back() += t;
                    lexbuf += t;
                    t = is.get();
                }
                is.unget();
                if (symtable.find(lexbuf) == symtable.end()) 
                    return Token::ID;

                int tmp = symtable[lexbuf];
                lexbuf = "";
                if (Token::Type(tmp) != Token::REM)
                    return Token::Type(tmp);
                else REM_line = true;
            } else {
                if (t == '<' || t == '>' || t == '!' || t == '&' || t == '|' || t == '=') {
                    lexbuf = "";
                    lexbuf += t;
                    auto tt = is.get();
                    lexbuf += tt;
                    if (symtable.find(lexbuf) != symtable.end()) {
                        code.back() += tt;
                        return symtable[lexbuf];
                    } else 
                        is.unget();
                }
                std::string lexbuf = "";
                lexbuf += t;
                if (symtable.find(lexbuf) != symtable.end())
                    return symtable[lexbuf];
                else {
                    // ERROR!
                    // std::cerr << "ERROR" << ' ' << t << std::endl;
                    return Token::ERROR;
                } 
            }
        }
        return Token::FILE_END;
    }
    void init_keywords() {
        symtable["REM"] = Token::REM;
        symtable["LET"] = Token::LET;
        symtable["INPUT"] = Token::INPUT;
        symtable["EXIT"] = Token::EXIT;
        symtable["EXIT"] = Token::EXIT;
        symtable["GOTO"] = Token::GOTO;
        symtable["IF"] = Token::IF;
        symtable["THEN"] = Token::THEN;
        symtable["FOR"] = Token::FOR;
        symtable["END"] = Token::END;
        symtable["/"] = Token::DIV;
        symtable["%"] = Token::MOD;
        symtable["+"] = Token::ADD;
        symtable["-"] = Token::SUB;
        symtable["*"] = Token::MUL;
        symtable["("] = Token::LParentheses;
        symtable[")"] = Token::RParentheses;
        symtable["["] = Token::LBrackets;
        symtable["]"] = Token::RBrackets;
        symtable["=="] = Token::EQU;
        symtable["!="] = Token::NEQ;
        symtable["<"] = Token::LT;
        symtable[">"] = Token::GT;
        symtable["<="] = Token::LEQ;
        symtable[">="] = Token::GEQ;
        symtable["&&"] = Token::AND;
        symtable["||"] = Token::OR;
        symtable["INT"] = Token::INT;
        symtable["="] = Token::ASSIGN;
        symtable[","] = Token::COMMA;
        symtable[";"] = Token::SEMI_COLON;
    }
};

