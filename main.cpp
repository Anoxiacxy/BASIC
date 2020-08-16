#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeGen.hpp"
#include "Error.hpp"
#include <vector>
#include <iostream>

int main() {
    //freopen("testcases/array_test/array_1.txt", "r", stdin);
    freopen(".data", "w", stdout);
    Lexer lexer(std::cin);
    std::vector<Token> tokens = lexer.getTokens();
    auto code = lexer.getCode();
    setCode(code);
    lexer.printCode(std::cerr);
    lexer.print(std::cerr);
    
    Parser parser(tokens);
    auto AST = parser.parseProgram();
    AST->print(std::cerr, 0);
    
    CodeGen codeGen(AST);
    codeGen.gen();
    codeGen.print(std::cerr);
    codeGen.printDump(std::cerr);
    codeGen.printCode(std::cout);
    //riscv.print(std::cout);
    int rtn = printError(std::cerr);
    std::cerr << rtn << std::endl;
    return rtn;
}