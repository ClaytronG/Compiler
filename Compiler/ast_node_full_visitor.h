// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_ASTNODEFULLVISITOR_H_
#define COMPILER_ASTNODEFULLVISITOR_H_

#include <set>

#include "ast_node.h"
#include "ast_node_visitor.h"
#include "semantic_analyzer.h"

class ASTNodeFullVisitor : public ASTNodeVisitor {
public:
  typedef SemanticAnalyzer::SymbolTable SymbolTable;
  // 
  ASTNodeFullVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator);

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
  typedef SemanticAnalyzer::SymbolTable::IdentificationTableEntry IdentificationTableEntry;

  void AddNode(ASTNode *node);

  void PopStack();

  int depth_;

  SymbolTable *symbol_table_;

  bool compound_variable_;

  std::string filename_;

  Administrator *administrator_;

  bool error_free_;

  bool in_loop_;

  bool nonvoid_function_;

  bool return_statement_;

  bool exit_statement_;

  Token::TokenName function_return_type_;

  bool default_case_;

  std::set<int> case_numbers_;

  bool variable_array_expression_;

  bool void_function_valid;
};

#endif // COMPILER_ASTNODEFULLVISITOR_H_