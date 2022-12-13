#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <array>

enum class Expression
{
    TAG_STRUCT = 0,

    START_TAG,
    END_TAG,
    EMPTY_TAG,
    INFO_TAG,

    TAG_SECTION,
    NAME,
    ARGS,
    ARG,

    ELEMENT,
    ARRAY,
    ARR_ELEMENTS,
    
    STR_STR,
    STR_NUM
};

enum class XMLTagType
{
    NO_TYPE = 0,
    INFO,
    SYSTEM,
    BLOCK,
    P,
    PORT,
    LINE,
    BRANCH
};

enum class XMLTagExtension
{
    /* Begining */
    NO_TYPE = 0,
    /* BlockType */
    IN_PORT,
    OUT_PORT,
    SUM,
    UNIT_DELAY,
    BT_GAIN,
    /* PName */
    POSITION,
    PORT_NUMBER,
    PORTS,
    PORT,
    NAME,
    ICON_SHAPE,
    INPUTS,
    P_GAIN,
    SAMPLE_TIME,
    SRC,
    DST,
    POINTS
};

enum class XMLElementType
{
    NO_TYPE = 0,
    ARRAY,
    STRING,
    NUMBER
};

struct XMLBlock
{
    Expression expression;
    XMLTagType tagType = XMLTagType::NO_TYPE;
    XMLElementType elementType = XMLElementType::NO_TYPE;
    XMLTagExtension extension = XMLTagExtension::NO_TYPE;
    std::vector<double> numbers; /* array/number/SID */
    std::string string; /* name/data */
};

enum class XMLBlockShape
{
    RECTANGULAR = 0,
    OVAL
};

[[maybe_unused]] 
static bool isNumber(const std::string& str)
{
    bool dot = false;
    for (unsigned i = 0; i < str.size(); ++i) {
        if (std::isdigit(str[i]) == 0) {
            if (str[i] == '.' && !dot) dot = true;
            else if (str[i] == '-' && i == 0) continue;
            else return false;
        }
    }
    return true;
}

struct Block
{
private:
    struct Port 
    {
        std::string name;
        unsigned portNumber = 0; 

        bool operator==(const Port& port) const 
        {
            return (port.name == name) && (port.portNumber == portNumber);
        }
    };
public:
    XMLTagType blockType = XMLTagType::NO_TYPE;
    XMLTagExtension extension = XMLTagExtension::NO_TYPE;
    std::string name;
    unsigned SID = 0;
    unsigned portId = 1;
    unsigned numberOfInputPorts = 0;
    unsigned numberOfOutputPorts = 0;
    std::array<int, 4> position;
    std::vector<Port> ports;
    std::vector<unsigned> pointingSIDs;
    std::string inputs;
    XMLBlockShape shape = XMLBlockShape::RECTANGULAR;
    double gain = 1;
    double sampleTime = -1;
    bool solved = false;
    std::vector<unsigned> sources;

    bool operator==(const Block& block) const
    {
        return (block.blockType == blockType) && (block.extension == extension) &&
               (block.name == name) && (block.SID == SID) && (block.portId == portId) &&
               (block.numberOfInputPorts == numberOfInputPorts) && (block.numberOfOutputPorts == numberOfOutputPorts) &&
               (block.position == position) && (block.ports == ports) && (block.inputs == inputs) &&
               (block.shape == shape) && (block.gain == gain) && (block.sampleTime == sampleTime) &&
               (block.solved == solved);
    }

    bool operator<(const Block& block) const
    {
        return block.SID < SID;
    }

    bool operator>(const Block& block) const
    {
        return block.SID > SID;
    }
};

struct Connection
{
    unsigned SID = 0;
    unsigned port = 0;
};

struct Line
{
public:
    std::string name;
    Connection source;
    std::vector<Connection> destinations;
};

struct ArithmeticExpression
{
    unsigned operand1 = 0;
    unsigned operand2 = 0;
    double operand2d = 0;
    std::string operation;
    unsigned arithmeticsFor;

    enum class Operands
    { FIRST_SID = 0, BOTH_SID } operands;
};



[[maybe_unused]]
static bool isString(const std::string& str)
{
    for (char const &c : str) {
        if (std::isalpha(c) == 0)
            return false;
    }
    return true;
}

[[maybe_unused]]
static bool isStringExtended(const std::string& str)
{
    for (char const &c : str) {
        if (std::isalpha(c) == 0 && c != '-' && c != '+')
            return false;
    }
    return true;
}

[[maybe_unused]]
static bool isLineString(const std::string& str)
{
    for (char const &c : str) {
        if (std::isalpha(c) == 0 && std::isdigit(c) == 0 && c != '#' && c != ':')
            return false;
    }
    return true;
}

#endif