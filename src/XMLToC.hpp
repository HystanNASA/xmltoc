#ifndef XMLTOC_HPP
#define XMLTOC_HPP

#include <string_view>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"

class XMLToC
{
public:
    XMLToC(std::string_view filename) : filename(filename) {}

    void run()
    {
        Lexer lexer(filename);
        Parser parser;
        CodeGenerator codeGenerator;
        
        auto& tokens = lexer.getTokens();
        parser.run(tokens);

        auto& blocks = parser.getBlocks();
        auto& arithmetics = parser.getArithmetics();
        auto& lines = parser.getLines();
        codeGenerator.run(blocks, arithmetics, lines);
    }

private:
    std::string_view filename;
};

#endif