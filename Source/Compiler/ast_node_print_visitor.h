// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#ifndef COMPILER_ASTNODEPRINTVISITOR_H_
#define COMPILER_ASTNODEPRINTVISITOR_H_

#include "ast_node.h"
#include "ast_node_visitor.h"
#include "messenger.h"

class ASTNodePrintVisitor : public ASTNodeVisitor {
public:
  // 
  ASTNodePrintVisitor(Messenger *messenger);

  void Visit(AssignmentNode &node);
  void Visit(ASTNode &node);
  void Visit(BinaryNode &node);
  void Visit(BranchNode &node);
  void Visit(CallNode &node);
  void Visit(CaseNode &node);
  void Visit(CompoundNode &node);
  void Visit(ContinueNode &node);
  void Visit(DeclarationNode &node);
  void Visit(ExitNode &node);
  void Visit(ExpressionNode &node);
  void Visit(FunctionDeclarationNode &node);
  void Visit(IfNode &node);
  void Visit(LiteralNode &node);
  void Visit(LoopNode &node);
  void Visit(NullNode &node);
  void Visit(ParameterNode &node);
  void Visit(ProgramNode &node);
  void Visit(ReturnNode &node);
  void Visit(StatementNode &node);
  void Visit(UnaryNode &node);
  void Visit(VariableDeclarationNode &node);
  void Visit(VariableNode &node);

private:
  //
  int depth_;

  //
  Messenger *messenger_;
};

#endif // COMPILER_ASTNODEPRINTVISITOR_H_