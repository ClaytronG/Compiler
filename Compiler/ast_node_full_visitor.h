// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_ASTNODEFULLVISITOR_H_
#define COMPILER_ASTNODEFULLVISITOR_H_

#include "ast_node.h"
#include "ast_node_visitor.h"
#include "semantic_analyzer.h"

class ASTNodeFullVisitor : public ASTNodeVisitor {
public:
  typedef SemanticAnalyzer::SymbolTable SymbolTable;
  // 
  ASTNodeFullVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator);

  void Visit(const AssignmentNode &node);
  void Visit(const ASTNode &node);
  void Visit(const BinaryNode &node);
  void Visit(const BranchNode &node);
  void Visit(const CallNode &node);
  void Visit(const CaseNode &node);
  void Visit(const CompoundNode &node);
  void Visit(const ContinueNode &node);
  void Visit(const DeclarationNode &node);
  void Visit(const ExitNode &node);
  void Visit(const ExpressionNode &node);
  void Visit(const FunctionDeclarationNode &node);
  void Visit(const IfNode &node);
  void Visit(const LiteralNode &node);
  void Visit(const LoopNode &node);
  void Visit(const NullNode &node);
  void Visit(const ParameterNode &node);
  void Visit(const ProgramNode &node);
  void Visit(const ReturnNode &node);
  void Visit(const StatementNode &node);
  void Visit(const UnaryNode &node);
  void Visit(const VariableDeclarationNode &node);
  void Visit(const VariableNode &node);

private:
  typedef SemanticAnalyzer::SymbolTable::IdentificationTableEntry IdentificationTableEntry;

  void PopStack();

  int depth_;

  SymbolTable *symbol_table_;

  bool compound_variable_;

  std::string filename_;

  Administrator *administrator_;

  bool error_free_;
};

#endif // COMPILER_ASTNODEFULLVISITOR_H_