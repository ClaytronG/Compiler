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
  virtual void Visit(AssignmentNode &node) = 0;
  virtual void Visit(BinaryNode &node) = 0;
  virtual void Visit(BranchNode &node) = 0;
  virtual void Visit(CallNode &node) = 0;
  virtual void Visit(CaseNode &node) = 0;
  virtual void Visit(CompoundNode &node) = 0;
  virtual void Visit(ContinueNode &node) = 0;
  virtual void Visit(DeclarationNode &node) = 0;
  virtual void Visit(ExitNode &node) = 0;
  virtual void Visit(ExpressionNode &node) = 0;
  virtual void Visit(FunctionDeclarationNode &node) = 0;
  virtual void Visit(IfNode &node) = 0;
  virtual void Visit(LiteralNode &node) = 0;
  virtual void Visit(LoopNode &node) = 0;
  virtual void Visit(NullNode &node) = 0;
  virtual void Visit(ParameterNode &node) = 0;
  virtual void Visit(ProgramNode &node) = 0;
  virtual void Visit(ReturnNode &node) = 0;
  virtual void Visit(StatementNode &node) = 0;
  virtual void Visit(UnaryNode &node) = 0;
  virtual void Visit(VariableDeclarationNode &node) = 0;
  virtual void Visit(VariableNode &node) = 0;
};

#endif // COMPILER_ASTNODEVISITOR_H_