#include "Parser.hpp"

void Parser::run(std::vector<XMLBlock>& xmlBlocks)
{
    this->xmlBlocks = xmlBlocks;
    parsingIndex = 0;
    parseXMLBlocks();
    connectLinesWithBlocks();
    makeupPath();
}

void Parser::parseXMLBlocks()
{
    XMLBlock& xmlBlock = xmlBlocks[parsingIndex];

    assert(xmlBlock.expression == Expression::START_TAG ||
           xmlBlock.expression == Expression::INFO_TAG);

    switch (xmlBlock.tagType)
    {
    case XMLTagType::SYSTEM:
        parseTagSystem();
        break;
    case XMLTagType::INFO:
        /* DON'T SUPPORT THIS NOW */
        ++parsingIndex;
        parseXMLBlocks();
        break;
    default:
        assert(false);
    }

    assert(tags.empty());
}

void Parser::parseTagSystem()
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::SYSTEM);

    ++parsingIndex;
    assert(parsingIndex < xmlBlocks.size());

    tags.push(XMLTagType::SYSTEM);

    for (; parsingIndex < xmlBlocks.size(); ++parsingIndex)
    {
        switch (xmlBlocks[parsingIndex].tagType)
        {
        case XMLTagType::BLOCK:
            parseTagBlock();
            break;
        case XMLTagType::LINE:
            parseTagLine();
            break;
        case XMLTagType::SYSTEM:
            assert(xmlBlocks[parsingIndex].expression == Expression::END_TAG);
            break;
        default:
            assert(false);
            break;
        }
    }

    tags.pop();
}

void Parser::parseTagBlock()
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::BLOCK);

    tags.push(XMLTagType::BLOCK);

    Block block { XMLTagType::BLOCK, xmlBlocks[parsingIndex].extension, 
                  xmlBlocks[parsingIndex].string, static_cast<unsigned>(xmlBlocks[parsingIndex].numbers.front()) };
    XMLBlock& xmlBlock = xmlBlocks[parsingIndex];

    if (xmlBlock.expression == Expression::START_TAG) {
        ++parsingIndex;
        collectTagBlockInfo(block);
    } else if (xmlBlock.expression != Expression::END_TAG) {
        assert(false);
    }

    block.name.erase(std::remove_if(block.name.begin(), block.name.end(),
             [](unsigned char x){return std::isspace(x);}), block.name.end());

    blocks.push_back(block);
    tags.pop();
}

void Parser::parseTagLine()
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::LINE);

    tags.push(XMLTagType::LINE);

    Line line;
    XMLBlock& xmlBlock = xmlBlocks[parsingIndex];

    if (xmlBlock.expression == Expression::START_TAG) {
        ++parsingIndex;
        collectTagLineInfo(line);
    } else if (xmlBlock.expression != Expression::END_TAG) {
        assert(false);
    }

    lines.push_back(line);

    tags.pop();
}

void Parser::collectTagBlockInfo(Block& block)
{
    assert(parsingIndex < xmlBlocks.size());

    for (; parsingIndex < xmlBlocks.size(); ++parsingIndex)
    {
        if (xmlBlocks[parsingIndex].tagType == XMLTagType::P)
            parseTagP(block);
        else if (xmlBlocks[parsingIndex].tagType == XMLTagType::PORT)
            parseTagPort(block);
        else if (xmlBlocks[parsingIndex].tagType == XMLTagType::BLOCK &&
                 xmlBlocks[parsingIndex].expression == Expression::END_TAG)
            return;
        else
            assert(false);
    }
}

void Parser::collectTagLineInfo(Line& line)
{
    assert(parsingIndex < xmlBlocks.size());

    for (; parsingIndex < xmlBlocks.size(); ++parsingIndex)
    {
        if (xmlBlocks[parsingIndex].tagType == XMLTagType::P)
            parseTagP(line);
        else if (xmlBlocks[parsingIndex].tagType == XMLTagType::BRANCH)
            parseTagBranch(line);
        else if (xmlBlocks[parsingIndex].tagType == XMLTagType::LINE &&
                 xmlBlocks[parsingIndex].expression == Expression::END_TAG)
            return;
        else
            assert(false);
    }
}

void Parser::parseTagP(Block& block)
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::P);

    if (xmlBlocks[parsingIndex].expression == Expression::START_TAG) {
        tags.push(XMLTagType::P);

        if (xmlBlocks[parsingIndex].extension == XMLTagExtension::POSITION) {
            auto position = getNextBlockArray();
            assert(position.size() == 4);
            block.position[0] = position[0];
            block.position[1] = position[1];
            block.position[2] = position[2];
            block.position[3] = position[3];
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::PORT) {
            block.portId = getNextBlockNumber();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::PORT_NUMBER) {
            /* DON'T HANDLE THIS NOW */
            getNextBlockNumber();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::NAME) {
            /* DON'T HANDLE THIS NOW */
            getNextBlockString();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::PORTS) {
            std::vector<double> ports = getNextBlockArray();
            assert(ports.size() == 2);
            block.numberOfInputPorts = ports.front();
            block.numberOfOutputPorts = ports.back();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::ICON_SHAPE) {
            getNextBlockString();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::INPUTS) {
            block.inputs = getNextBlockString();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::P_GAIN) {
            block.gain = getNextBlockNumber();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::SAMPLE_TIME) {
            block.sampleTime = getNextBlockNumber();
        } else {
            assert(false);
        }
    } else if (xmlBlocks[parsingIndex].expression == Expression::END_TAG) {
        tags.pop();
    } else {
        assert(false);
    }
}

void Parser::parseTagP(Line& line)
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::P);

    if (xmlBlocks[parsingIndex].expression == Expression::START_TAG) {
        tags.push(XMLTagType::P);

        if (xmlBlocks[parsingIndex].extension == XMLTagExtension::NAME) {
            line.name = getNextBlockString();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::POINTS) {
            /* DON'T SUPPORT THIS NOW */
            getNextBlockArray();
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::SRC) {
            line.source = parseConnection(line);
        } else if (xmlBlocks[parsingIndex].extension == XMLTagExtension::DST) {
            line.destinations.push_back(parseConnection(line));
        } else {
            assert(false);
        }
    } else if (xmlBlocks[parsingIndex].expression == Expression::END_TAG) {
        tags.pop();
    } else {
        assert(false);
    }
}

void Parser::parseTagPort(Block& block)
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::PORT);

    if (xmlBlocks[parsingIndex].expression == Expression::START_TAG) {
        tags.push(XMLTagType::PORT);
    } else if (xmlBlocks[parsingIndex].expression == Expression::END_TAG) {
        tags.pop();
    } else {
        assert(false);
    }
}

void Parser::parseTagBranch(Line& line)
{
    assert(xmlBlocks[parsingIndex].tagType == XMLTagType::BRANCH);

    if (xmlBlocks[parsingIndex].expression == Expression::START_TAG) {
        tags.push(XMLTagType::BRANCH);

        for (++parsingIndex; parsingIndex < xmlBlocks.size(); ++parsingIndex)
        {
            if (xmlBlocks[parsingIndex].tagType == XMLTagType::P)
                parseTagP(line);
            else if (xmlBlocks[parsingIndex].tagType == XMLTagType::BRANCH &&
                     xmlBlocks[parsingIndex].expression == Expression::END_TAG) { 
                tags.pop();
                break;
            } else if (xmlBlocks[parsingIndex].tagType == XMLTagType::LINE &&
                     xmlBlocks[parsingIndex].expression == Expression::END_TAG) {
                break;
            }
            else {
                assert(false);
            }
        }
    } else if (xmlBlocks[parsingIndex].expression == Expression::END_TAG) {
        tags.pop();
    } else {
        assert(false);
    }
}

Connection Parser::parseConnection(Line& lines)
{
    std::regex regex(R"((\d+)|(in|out))");
    std::string element = getNextBlockString();
    std::sregex_iterator iterator(element.begin(), element.end(), regex);
    std::sregex_iterator end;
    Connection connection;

    assert(iterator != end);
    connection.SID = std::stoul(iterator->str());
    /* This one is either `in` or `out`. Could be used for checking.
       Could be used for checking */
    ++iterator; assert(iterator != end);
    ++iterator; assert(iterator != end);
    connection.port = std::stoul(iterator->str());

    return connection;
}

double Parser::getNextBlockNumber()
{
    ++parsingIndex;

    assert(parsingIndex < xmlBlocks.size());
    assert(xmlBlocks[parsingIndex].expression == Expression::ELEMENT);
    assert(xmlBlocks[parsingIndex].elementType == XMLElementType::NUMBER);

    return xmlBlocks[parsingIndex].numbers.front();
}

std::vector<double> Parser::getNextBlockArray()
{
    ++parsingIndex;

    assert(parsingIndex < xmlBlocks.size());
    assert(xmlBlocks[parsingIndex].expression == Expression::ELEMENT);
    assert(xmlBlocks[parsingIndex].elementType == XMLElementType::ARRAY);

    return xmlBlocks[parsingIndex].numbers;
}

std::string Parser::getNextBlockString()
{
    ++parsingIndex;

    assert(parsingIndex < xmlBlocks.size());
    assert(xmlBlocks[parsingIndex].expression == Expression::ELEMENT);
    assert(xmlBlocks[parsingIndex].elementType == XMLElementType::STRING);

    return xmlBlocks[parsingIndex].string;
}

void Parser::makeupPath() 
{
    for (Block& block : blocks)
        if (block.extension == XMLTagExtension::OUT_PORT)
            checkoutStack.push_back(block.SID);

    traverseCheckoutStack();
    unloadArithmetics();
}

void Parser::traverseCheckoutStack()
{
    while (!checkoutStack.empty())
    {
        Block& block = getBlock(checkoutStack.back());

        if (block.extension == XMLTagExtension::SUM) {
            std::vector<unsigned> operands;
            ArithmeticExpression ae;
            addToCheckoutStack(block, [&operands](Block& block) { operands.push_back(block.SID); });
            if (operands.empty())
                continue;
            ae.operand1 = operands.front();
            ae.operand2 = operands.back();
            ae.operands = ArithmeticExpression::Operands::BOTH_SID;
            ae.operation = block.inputs.empty() ? "++" : block.inputs;
            ae.arithmeticsFor = block.SID;
            arithmeticExpressions.push_back(ae);
        } else if (block.extension == XMLTagExtension::BT_GAIN) {
            int SID = -1;
            ArithmeticExpression ae;
            addToCheckoutStack(block, [&SID](Block& block) { SID = block.SID; });
            if (SID == -1)
                continue;
            ae.operand1 = SID;
            ae.operand2d = block.gain;
            ae.operands = ArithmeticExpression::Operands::FIRST_SID;
            ae.operation = '*';
            ae.arithmeticsFor = block.SID;
            arithmeticExpressions.push_back(ae);
        } else if (block.extension == XMLTagExtension::UNIT_DELAY) {
            int SID = -1;
            ArithmeticExpression ae;
            addToCheckoutStack(block, [&SID](Block& block) { SID = block.SID; });
            if (SID == -1)
                continue;
            ae.operand1 = SID;
            ae.operand2d = 0;
            ae.operands = ArithmeticExpression::Operands::FIRST_SID;
            ae.operation = '=';
            ae.arithmeticsFor = block.SID;
            arithmeticExpressions.push_back(ae);
            block.solved = true;
        } else if (block.extension == XMLTagExtension::IN_PORT) {
            block.solved = true;
        } else if (block.extension == XMLTagExtension::OUT_PORT) {
            addToCheckoutStack(block, [](Block& block) {});
        } else {}

        block.solved = isBlockSolved(block);
        if (block.solved) {
            if (block.extension != XMLTagExtension::IN_PORT &&
                block.extension != XMLTagExtension::OUT_PORT &&
                block.extension != XMLTagExtension::UNIT_DELAY) {
                unloadArithmetics();
            }
            checkoutStack.pop_back();
            continue;
        }
    }
}

void Parser::addToCheckoutStack(Block& block, std::function<void(Block& block)> predicate)
{
    for (auto &&source : block.sources) {
        Block& sourceBlock = getBlock(source);
        if (checkedBlocks.find(sourceBlock) == checkedBlocks.end()) {
            checkedBlocks.insert(sourceBlock);
            checkoutStack.push_back(sourceBlock.SID);
        }
        predicate(sourceBlock);
    }
}

void Parser::unloadArithmetics()
{
    if (!arithmeticExpressions.empty()) {
        arithmetics.push_back(arithmeticExpressions.back());
        arithmeticExpressions.pop_back();
    }
}

/* Block is solved if all blocks pointing to the block are solved */
bool Parser::isBlockSolved(Block& blockDest)
{
    if (blockDest.solved)
        return true;

    bool hasSourceBlocks = blockDest.sources.size() > 0 ? true : false;

    for (auto &&source : blockDest.sources) {
        Block& sourceBlock = getBlock(source);
        if (!sourceBlock.solved)
            return false;
    }

    /* IN_PORTs don't have connections,
    so here they won't be considered to be solved */
    return hasSourceBlocks;
}

void Parser::connectLinesWithBlocks()
{
    for (auto &&line : lines)
        for (auto &&dest : line.destinations)
            getBlock(dest.SID).sources.push_back(line.source.SID);
}

Block& Parser::getBlock(unsigned SID)
{
    return *std::find_if(blocks.begin(), blocks.end(), [&SID](Block& block) { return block.SID == SID; });
}

auto Parser::getArithmetics() -> std::vector<ArithmeticExpression>&
{
    return arithmetics;
}

auto Parser::getBlocks() -> std::vector<Block>&
{
    return blocks;
}

auto Parser::getLines() -> std::vector<Line>&
{
    return lines;
}