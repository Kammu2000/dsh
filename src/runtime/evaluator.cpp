#include "runtime/evaluator.hpp"
#include "frontend/parser.hpp"
#include <array>
#include <cstdlib>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

constexpr mode_t DEFAULT_FILE_MODE = 0644;

void Evaluator::visit(const PipeExpression& expr)
{
    evaluate_pipe(expr);
}

void Evaluator::visit(const CommandExpression& expr)
{
    evaluate_command(expr);
}

void Evaluator::evaluate(const Expression& expr)
{
    expr.accept(*this);
}

void Evaluator::evaluate_pipe(const PipeExpression& expr)
{
    std::array<int, 2> file_desc{};
    pipe(file_desc.data());

    // left process
    pid_t left = fork();

    if (left == 0)
    {
        dup2(file_desc[1], STDOUT_FILENO);
        close(file_desc[0]);
        close(file_desc[1]);
        evaluate(*expr.get_left());
        exit(0);
    }

    // right process
    pid_t right = fork();

    if (right == 0)
    {
        dup2(file_desc[0], STDIN_FILENO);
        close(file_desc[0]);
        close(file_desc[1]);
        evaluate(*expr.get_right());
        exit(0);
    }

    close(file_desc[0]);
    close(file_desc[1]);
    waitpid(left, nullptr, 0);
    waitpid(right, nullptr, 0);
}

void Evaluator::evaluate_command(const CommandExpression& cmd)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        for (const auto& redirect : cmd.get_redirects())
        {
            int file_desc = 0;

            if (redirect.m_type == RedirectType::REDIRECT_OUT)
            {
                // NOLINTNEXTLINE
                file_desc = open(redirect.m_file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                                 DEFAULT_FILE_MODE);
                dup2(file_desc, STDOUT_FILENO);
            }
            else if (redirect.m_type == RedirectType::REDIRECT_APPEND)
            {
                // NOLINTNEXTLINE
                file_desc = open(redirect.m_file_name.c_str(), O_WRONLY | O_CREAT | O_APPEND,
                                 DEFAULT_FILE_MODE);
                dup2(file_desc, STDOUT_FILENO);
            }
            else if (redirect.m_type == RedirectType::REDIRECT_IN)
            {
                // NOLINTNEXTLINE
                file_desc = open(redirect.m_file_name.c_str(), O_RDONLY);
                dup2(file_desc, STDIN_FILENO);
            }

            close(file_desc);
        }

        // we can not use .c_str() on cmd.get_command because that returns const char* while argv needs char*
        // so we need to own the memory and then pass reference to argv array by keeping a args_store
        std::vector<std::string> args_store;
        std::vector<char*> argv;

        args_store.push_back(cmd.get_command());
        argv.push_back(args_store.back().data());

        for (const auto& arg : cmd.get_args())
        {
            args_store.push_back(arg);
            argv.push_back(args_store.back().data());
        }

        execvp(cmd.get_command().c_str(), argv.data());

        perror("Execution failed");
        exit(1);
    }
    else
    {
        waitpid(pid, nullptr, 0);
    }
}
