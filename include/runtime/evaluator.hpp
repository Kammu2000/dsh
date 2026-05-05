#pragma once
#include "ast_visitor.hpp"
#include "frontend/parser.hpp"

class Evaluator : public ASTVisitor
{
  public:
    void visit(const PipeExpression& expr) override;
    void visit(const CommandExpression& expr) override;
    void evaluate(const Expression&);

  private:
    void evaluate_pipe(const PipeExpression&);
    static void evaluate_command(const CommandExpression&);
};
