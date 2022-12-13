#include "Lexer.hpp"

auto Lexer::getTokens() -> std::vector<XMLBlock>&
{
    assert(readFile() == true);
    decodeTokens();
    return xmlBlocks;
}

bool Lexer::readFile()
{
    std::sregex_iterator end;

    reader.open(filename.data());
    if(!reader.is_open())
        return false;
    std::stringstream buffer;
    buffer << reader.rdbuf();
    fileContent = buffer.str();
    reader.close();

    return true;
}

void Lexer::decodeTokens()
{
    std::regex regex(R"((<[^>]+>)|(\[.*\])|((\d+([#](in|out):)\d+)|((-?\d+\.\d+)|(-?\d+)))|(((\+)+(-)*)|((-)+(\+)*))|(\w+))");
    std::sregex_iterator regexIterator(fileContent.begin(), fileContent.end(), regex);
    std::sregex_iterator end;

    assert(regexIterator != end);

    for (; regexIterator != end; regexIterator++)
    {
        std::string token = regexIterator->str();

        assert(!token.empty());

        if (token.front() == '<' && token.size() >= 3 &&
                token[token.size() - 2] == '/' && token.back() == '>')
            decodeEmptyTag(token);
        else if (token.front() == '<' && token.size() >= 4 &&
                token[1] == '?' && token[token.size() - 2] == '?' && token.back() == '>')
            /* DON'T SUPPORT THE INFORMATION TAG */
            xmlBlocks.push_back({Expression::INFO_TAG, XMLTagType::INFO});
        else if (token.front() == '<' && token.size() >= 3 &&
                token[1] == '/' && token.back() == '>')
            decodeEndTag(token);
        else if (token.front() == '<' && token.size() >= 3 &&
                token.back() == '>')
            decodeStartTag(token);
        else if (token.front() == '[' && token.back() == ']')
            decodeArray(token);
        else if (isNumber(token))
            decodeNumber(token);
        else if (isStringExtended(token))
            decodeString(token);
        else if (isLineString(token))
            decodeString(token);
        else
            DEBUG_OUTPUT("decodeTokens", "cannot resolve symbol " + token)
    }
}

void Lexer::decodeEmptyTag(std::string& emptyTag) { /* CODE FOR AN EMPTY TAG */ }

void Lexer::decodeStartTag(std::string& startTag)
{
    std::regex regex(R"((?!")(\w+[\s+\w+]*)(?=")|(\w+))");
    std::sregex_iterator attributeIterator(startTag.begin(), startTag.end(), regex);
    std::sregex_iterator end;
    XMLBlock xmlBlock {Expression::START_TAG};

    assert(attributeIterator != end);

    std::string&& attribute = attributeIterator->str();

    if (attribute == "System") {
        xmlBlock.tagType = XMLTagType::SYSTEM;
    } else if (attribute == "Block") {
        xmlBlock.tagType = XMLTagType::BLOCK;

        ++attributeIterator;
        assert(attributeIterator != end);

        for (; attributeIterator != end; ++attributeIterator)
        {
            attribute = attributeIterator->str();
            if (attribute == "BlockType") {
                attribute = (++attributeIterator)->str();
                if (attribute == "Inport")
                    xmlBlock.extension = XMLTagExtension::IN_PORT;
                else if (attribute == "Outport")
                    xmlBlock.extension = XMLTagExtension::OUT_PORT;
                else if (attribute == "Sum")
                    xmlBlock.extension = XMLTagExtension::SUM;
                else if (attribute == "UnitDelay")
                    xmlBlock.extension = XMLTagExtension::UNIT_DELAY;
                else if (attribute == "Gain")
                    xmlBlock.extension = XMLTagExtension::BT_GAIN;
                else
                    assert(false);
            } else if (attribute == "Name") {
                attribute = (++attributeIterator)->str();
                xmlBlock.string = attribute;
            } else if (attribute == "SID") {
                ++attributeIterator; 
                assert(attributeIterator != end);
                assert(isNumber(attributeIterator->str()));
                xmlBlock.numbers.push_back(std::stod(attributeIterator->str()));
            } else 
                assert(false);
        }
    } else if (attribute == "P") {
        xmlBlock.tagType = XMLTagType::P;

        ++attributeIterator; 
        assert(attributeIterator != end);

        attribute = attributeIterator->str();
        if (attribute == "Name") {
            attribute = (++attributeIterator)->str();

            if (attribute == "Position")
                xmlBlock.extension = XMLTagExtension::POSITION;
            else if (attribute == "PortNumber")
                xmlBlock.extension = XMLTagExtension::PORT_NUMBER;
            else if (attribute == "Port")
                xmlBlock.extension = XMLTagExtension::PORT;
            else if (attribute == "Ports")
                xmlBlock.extension = XMLTagExtension::PORTS;
            else if (attribute == "Name")
                xmlBlock.extension = XMLTagExtension::NAME;
            else if (attribute == "IconShape")
                xmlBlock.extension = XMLTagExtension::ICON_SHAPE;
            else if (attribute == "Inputs")
                xmlBlock.extension = XMLTagExtension::INPUTS;
            else if (attribute == "Gain")
                xmlBlock.extension = XMLTagExtension::P_GAIN;
            else if (attribute == "SampleTime")
                xmlBlock.extension = XMLTagExtension::SAMPLE_TIME;
            else if (attribute == "Src")
                xmlBlock.extension = XMLTagExtension::SRC;
            else if (attribute == "Dst")
                xmlBlock.extension = XMLTagExtension::DST;
            else if (attribute == "Points")
                xmlBlock.extension = XMLTagExtension::POINTS;
            else
                assert(false);
        } else {
            assert(false);
        }
    } else if (attribute == "Port") {
        xmlBlock.tagType = XMLTagType::PORT;
    } else if (attribute == "Line") {
        xmlBlock.tagType = XMLTagType::LINE;
    } else if (attribute == "Branch") {
        xmlBlock.tagType = XMLTagType::BRANCH;
    } else
        assert(false);

    xmlBlocks.push_back(xmlBlock);
}

void Lexer::decodeEndTag(std::string& endTag) 
{
    std::regex regex(R"(\w*[^=\"\s<>/]+)");
    std::sregex_iterator attributeIterator(endTag.begin(), endTag.end(), regex);
    std::sregex_iterator end;
    XMLBlock xmlBlock {Expression::END_TAG};

    assert(attributeIterator != end);

    std::string&& attribute = attributeIterator->str();

    if (attribute == "System")
        xmlBlock.tagType = XMLTagType::SYSTEM;
     else if (attribute == "Block")
        xmlBlock.tagType = XMLTagType::BLOCK;
     else if (attribute == "P")
        xmlBlock.tagType = XMLTagType::P;
     else if (attribute == "Port")
        xmlBlock.tagType = XMLTagType::PORT;
     else if (attribute == "Line")
        xmlBlock.tagType = XMLTagType::LINE;
     else if (attribute == "Branch")
        xmlBlock.tagType = XMLTagType::BRANCH;
     else
        assert(false);

    xmlBlocks.push_back(xmlBlock);
}

void Lexer::decodeArray(std::string& array)
{
    std::regex regex(R"(-?\d+[.\d+]*)");
    std::sregex_iterator elementIterator(array.begin(), array.end(), regex);
    std::sregex_iterator end;
    std::vector<double> elements;
    XMLBlock xmlBlock {Expression::ELEMENT, XMLTagType::NO_TYPE, XMLElementType::ARRAY};

    assert(elementIterator != end);

    elements.reserve(4);

    for (; elementIterator != end; ++elementIterator)
        xmlBlock.numbers.push_back(std::stod(elementIterator->str()));

    xmlBlocks.push_back(xmlBlock);
}

void Lexer::decodeNumber(std::string& number)
{
    xmlBlocks.push_back(XMLBlock{Expression::ELEMENT, XMLTagType::NO_TYPE,
                        XMLElementType::NUMBER, XMLTagExtension::NO_TYPE, {std::stod(number)}, ""});
}

void Lexer::decodeString(std::string& str)
{
    xmlBlocks.push_back(XMLBlock{Expression::ELEMENT, XMLTagType::NO_TYPE,
                        XMLElementType::STRING, XMLTagExtension::NO_TYPE, {}, str});
}