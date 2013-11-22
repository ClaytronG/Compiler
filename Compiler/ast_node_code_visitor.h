#ifndef COMPILER_ASTNODECODEVISITOR_H_
#define COMPILER_ASTNODECODEVISITOR_H_

#include <string>

#include "ast_node.h"
#include "ast_node_visitor.h"

class ASTNodeCodeVisitor : public ASTNodeVisitor {
public:
  // 
  ASTNodeCodeVisitor(std::string &output);

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
  std::string CreateQuad(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);
  //
  std::string *output_;

  bool global_scope_;

  bool function_compound_;
};

#endif // COMPILER_ASTNODECODEVISITOR_H_