#include "CodeGenerator.hpp"

void CodeGenerator::run(std::vector<Block>& blocks, std::vector<ArithmeticExpression>& arithmetics, std::vector<Line>& lines)
{
    if (!openFile())
        return;

    writeHeaders();
    writeBlocks(blocks);
    writeInit(blocks);
    writeArithmetics(blocks, arithmetics);
    writePorts(blocks, lines);
}

bool CodeGenerator::openFile()
{
    writer.open(filename.data());
    return writer.is_open();
}

void CodeGenerator::writeHeaders()
{
    for (auto &&i : localHeaders)
        writer << "#include \"" << i << '\"' << '\n';

    writer << '\n';

    for (auto &&i : libraryHeaders)
        writer << "#include <" << i << ">\n";

    writer << '\n';
}

void CodeGenerator::writeBlocks(std::vector<Block>& blocks)
{
    writer << "static struct\n{\n";

    for (auto &&i : blocks)
        writer << "\tdouble " << i.name << ";\n";
    writer << "} nwocg;\n\n";
}

void CodeGenerator::writeInit(std::vector<Block>& blocks)
{
    writer << "void nwocg_generated_init()\n{\n";

    for (auto &&i : blocks)
        if (i.extension == XMLTagExtension::UNIT_DELAY)
            writer << "\tnwocg." << i.name << " = 0;\n";
    writer << "}\n\n";
}

void CodeGenerator::writeArithmetics(std::vector<Block>& blocks, std::vector<ArithmeticExpression>& arithmetics)
{
    writer << "void nwocg_generated_step()\n{\n";

    for (auto &&a : arithmetics)
    {
        writer << "\tnwocg." << getBlock(blocks, a.arithmeticsFor).name << " = ";

        if (a.operation == "=") {
            writer << "nwocg." << getBlock(blocks, a.operand1).name;
        } else if (a.operation.size() == 1) {
            if (a.operands == ArithmeticExpression::Operands::BOTH_SID)
                writer << "nwocg." << getBlock(blocks, a.operand1).name << ' ' << a.operation << "nwocg." << ' ' << getBlock(blocks, a.operand2).name;
            else if (a.operands == ArithmeticExpression::Operands::FIRST_SID)
                writer << "nwocg." << getBlock(blocks, a.operand1).name << ' ' << a.operation << ' ' << a.operand2d;
        } else if (a.operation.size() == 2) {
            if (a.operation.front() == '-')
                writer << '-';
            writer << "nwocg." << getBlock(blocks, a.operand1).name << ' ' << a.operation[1] << " nwocg." << getBlock(blocks, a.operand2).name;
        }

        writer << ";\n";
    }

    writer << "}\n\n";
}

void CodeGenerator::writePorts(std::vector<Block>& blocks, std::vector<Line>& lines)
{
    writer << "static const nwocg_ExtPort\n\text_ports[] =\n{\n";

    for (auto &&b : blocks)
        if (b.extension == XMLTagExtension::OUT_PORT)
            writer << "\t{\"" << b.name << "\", &nwocg." << getBlock(blocks, b.sources.front()).name << ", 0 },\n";
        else if (b.extension == XMLTagExtension::IN_PORT)
            writer << "\t{\"" << b.name << "\", &nwocg." << b.name << ", 1 },\n";

    writer << "\t{ 0, 0, 0 },\n};\n\n";
    writer << "const nwocg_ExtPort * const\n\tnwocg_generated_ext_ports = ext_ports;\n\nconst size_t\n\tnwocg_generated_ext_ports_size = sizeof(ext_ports);\n";
}

Block& CodeGenerator::getBlock(std::vector<Block>& block, unsigned SID)
{
    return *std::find_if(block.begin(), block.end(), [&SID](Block& block) { return block.SID == SID; });
}