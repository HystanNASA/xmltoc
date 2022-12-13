#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <cassert>
#include <string_view>
#include <stack>
#include <string>
#include <regex>
#include <fstream>
#include <sstream>

#include "utils.hpp"

#define DEBUG_OUTPUT(method, msg) { std::cout << method << ": " << msg << std::endl; }

class Lexer
{
public:
    Lexer(std::string_view filename) : filename(filename) {}
    ~Lexer() = default;

    auto getTokens() -> std::vector<XMLBlock>&;

private:
    bool readFile();

    void decodeTokens();
    void decodeEmptyTag(std::string&);
    void decodeEndTag(std::string&);
    void decodeStartTag(std::string&);
    void decodeArray(std::string&);
    void decodeNumber(std::string& number);
    void decodeString(std::string& number);

    std::ifstream reader;
    std::string_view filename;
    std::string fileContent;

    std::vector<XMLBlock> xmlBlocks;
};

#endif