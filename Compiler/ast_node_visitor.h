// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#ifndef COMPILER_ASTNODEVISITOR_H_
#define COMPILER_ASTNODEVISITOR_H_

class AssignmentNode;
class BinaryNode;
class BranchNode;
class CallNode;
class CaseNode;
class CompoundNode;
class ContinueNode;
class DeclarationNode;
class ExitNode;
class ExpressionNode;
class FunctionDeclarationNode;
class IfNode;
class LiteralNode;
class LoopNode;
class NullNode;
class ParameterNode;
class ProgramNode;
class ReturnNode;
class StatementNode;
class UnaryNode;
class VariableDeclarationNode;
class VariableNode;

//#include "ast_node.h"

class ASTNodeVisitor {
public:
  virtual void Visit(const AssignmentNode &node) = 0;
  virtual void Visit(const BinaryNode &node) = 0;
  virtual void Visit(const BranchNode &node) = 0;
  virtual void Visit(const CallNode &node) = 0;
  virtual void Visit(const CaseNode &node) = 0;
  virtual void Visit(const CompoundNode &node) = 0;
  virtual void Visit(const ContinueNode &node) = 0;
  virtual void Visit(const DeclarationNode &node) = 0;
  virtual void Visit(const ExitNode &node) = 0;
  virtual void Visit(const ExpressionNode &node) = 0;
  virtual void Visit(const FunctionDeclarationNode &node) = 0;
  virtual void Visit(const IfNode &node) = 0;
  virtual void Visit(const LiteralNode &node) = 0;
  virtual void Visit(const LoopNode &node) = 0;
  virtual void Visit(const NullNode &node) = 0;
  virtual void Visit(const ParameterNode &node) = 0;
  virtual void Visit(const ProgramNode &node) = 0;
  virtual void Visit(const ReturnNode &node) = 0;
  virtual void Visit(const StatementNode &node) = 0;
  virtual void Visit(const UnaryNode &node) = 0;
  virtual void Visit(const VariableDeclarationNode &node) = 0;
  virtual void Visit(const VariableNode &node) = 0;
};

#endif // COMPILER_ASTNODEVISITOR_H_