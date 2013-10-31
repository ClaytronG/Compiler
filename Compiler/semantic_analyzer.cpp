// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"
#include "ast_node_init_visitor.h"
#include "semantic_analyzer.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode *root) : root_(root), symbol_table_() {
  error_free_ = true;
}

void SemanticAnalyzer::InitTraversal() {
  root_->Accept(new ASTNodeInitVisitor(&symbol_table_));
  // Make sure void main is the last declaration

  symbol_table_.PrintTables();
}

void SemanticAnalyzer::FullTraversal() {
  root_->Accept(new ASTNodeFullVisitor(&symbol_table_));
}