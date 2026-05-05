#include <exception>
#include <frontend/parser.hpp>
#include <runtime/evaluator.hpp>
#include <common/errors.hpp>
#include <common/error_handler.hpp>
#include <cstddef>
#include <iostream>
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

        try
        {
            ProgramParser parser(code);
            Program ast = parser.build_ast();

            Evaluator evaluator;

            for (const auto& stmt : ast.m_statements)
            {
                evaluator.evaluate(*stmt);
            }
        }
        catch (const std::exception& error)
        {
            dsh::handlers::handle_error(error);
        }
    }

    return 0;
}
