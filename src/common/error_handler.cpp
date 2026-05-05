#include <common/error_handler.hpp>
#include <common/errors.hpp>
#include <exception>
#include <iostream>

namespace dsh::handlers {

void handle_error(const std::exception& error)
{
    const auto* shell_error = dynamic_cast<const ShellError*>(&error);

    if (shell_error != nullptr)
    {
        std::cerr << "[" << shell_error->type() << "]: " << " " << shell_error->what() << "\n";
    }
    else
    {
        std::cerr << "[UNEXPECTED ERROR]: " << " " << error.what() << "\n";
    }
}
} // namespace dsh::handlers
