// Author: Clayton Green

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
  // Creates a string representation of a quadruple
  std::string CreateQuad(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);

  //
  std::string GetVariable(const VariableNode *node);

  //
  std::string GetLiteral(const LiteralNode *node);

  //
  void PushArguments(const CallNode &node);
  
  //
  std::string *output_;

  // 
  VariableNode last_variable_;

  // 
  LiteralNode last_literal_;

  // Check to see if we are in a function's compound statement.
  // i.e. do not need ECS and LCS quadruples
  bool function_compound_;

  // 
  int current_num_parameters_;

  //
  int current_temp_variable_;

  //
  int current_continue_jump_;

  //
  int current_exit_jump_;

  //
  int current_label_;

  //
  int current_branch_;
};

#endif // COMPILER_ASTNODECODEVISITOR_H_