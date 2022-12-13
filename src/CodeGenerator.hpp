#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <fstream>
#include <string_view>
#include <array>
#include <algorithm>

#include "utils.hpp"

class CodeGenerator
{
public:
    CodeGenerator() = default;

    void run(std::vector<Block>& blocks, std::vector<ArithmeticExpression>& arithmetics, std::vector<Line>& lines);

private:
    bool openFile();
    void writeHeaders();
    void writeBlocks(std::vector<Block>&);
    void writeInit(std::vector<Block>&);
    void writeArithmetics(std::vector<Block>&, std::vector<ArithmeticExpression>&);
    void writePorts(std::vector<Block>&, std::vector<Line>& lines);

    Block& getBlock(std::vector<Block>&, unsigned);

    std::array<std::string_view, 1> localHeaders = {"nwocg_run.h"};
    std::array<std::string_view, 1> libraryHeaders = {"math.h"};

    std::string_view filename = "output.c";
    std::ofstream writer;
};

#endif