// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_ASTNODEINITVISITOR_H_
#define COMPILER_ASTNODEINITVISITOR_H_

#include <string>

#include "ast_node.h"
#include "ast_node_visitor.h"
#include "semantic_analyzer.h"

class ASTNodeInitVisitor : public ASTNodeVisitor {
public:
  typedef SemanticAnalyzer::SymbolTable SymbolTable;

  //
  ASTNodeInitVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator);

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

  bool error_free() const {
    return error_free_;
  }

private:
  typedef SemanticAnalyzer::SymbolTable::IdentificationTableEntry IdentificationTableEntry;

  bool IsValidIntOperator(const Token::TokenName op) const;

  SymbolTable *symbol_table_;

  Administrator *administrator_;

  std::string filename_;

  bool error_free_;

  bool static_array_expression_;
};

#endif // COMPILER_ASTNODEINITVISITOR_H_