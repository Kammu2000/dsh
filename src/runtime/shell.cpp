#include <cstddef>
#include <frontend/parser.hpp>
#include <iostream>
#include <runtime/evaluator.hpp>
#include <string>

int main()
{
    while (true)
    {
        std::cout << "$ ";
        std::string code;

        // exit signal detection
        if (!std::getline(std::cin, code))
        {
            break; // ctrl + d
        }

        ProgramParser parser(code);
        Program ast = parser.build_ast();

        Evaluator evaluator;

        for (const auto& stmt : ast.m_statements)
        {
            evaluator.evaluate(*stmt);
        }
    }

    return 0;
}
