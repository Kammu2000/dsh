#pragma once
#include "astVisitor.hpp"
#include "frontend/parser.hpp"

class Evaluator : public ASTVisitor {
public:
  void visit(const PipeExpression &expr) override;
  void visit(const CommandExpression &expr) override;
  void evaluate(const Expression &);

private:
  void evaluatePipe(const PipeExpression &);
  static void evaluateCommand(const CommandExpression &);
};
