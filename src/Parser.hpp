#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <array>
#include <stack>
#include <regex>
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <cassert>

#include "utils.hpp"

class Parser
{
public:
    Parser() = default;

    void run(std::vector<XMLBlock>&);

    auto getArithmetics() -> std::vector<ArithmeticExpression>&;
    auto getBlocks() -> std::vector<Block>&;
    auto getLines() -> std::vector<Line>&;

private:
    void parseXMLBlocks();
    void makeupPath();

    void parseTagSystem();
    void parseTagBlock();
    void parseTagLine();
    void parseTagPort(Block&);
    void parseTagP(Block&);
    void parseTagP(Line&);
    void parseTagBranch(Line&);

    Connection parseConnection(Line&);

    void collectTagLineInfo(Line&);
    void collectTagBlockInfo(Block&);
    std::vector<double> getNextBlockArray();
    std::string getNextBlockString();
    double getNextBlockNumber();

    void traverseCheckoutStack();
    inline void addToCheckoutStack(Block&, std::function<void(Block&)>);
    inline void unloadArithmetics();
    bool isBlockSolved(Block& block);
    Block& getBlock(unsigned);
    void connectLinesWithBlocks();

    std::vector<Line> lines;
    std::vector<Block> blocks;
    std::vector<XMLBlock> xmlBlocks;
    std::stack<XMLTagType> tags;
    size_t parsingIndex = 0;

    std::set<Block> checkedBlocks;
    std::vector<unsigned> checkoutStack;
    std::vector<ArithmeticExpression> arithmetics;
    std::vector<ArithmeticExpression> arithmeticExpressions;
};

#endif