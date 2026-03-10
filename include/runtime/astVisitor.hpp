#pragma once

class PipeExpression;
class CommandExpression;

class ASTVisitor {
public:
  virtual ~ASTVisitor() = default;

  ASTVisitor() = default;
  ASTVisitor(const ASTVisitor &) = default;
  ASTVisitor &operator=(const ASTVisitor &) = default;
  ASTVisitor(ASTVisitor &&) = default;
  ASTVisitor &operator=(ASTVisitor &&) = default;

  virtual void visit(const PipeExpression &) = 0;
  virtual void visit(const CommandExpression &) = 0;
};
