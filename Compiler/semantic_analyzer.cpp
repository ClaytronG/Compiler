// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_init_visitor.h"
#include "semantic_analyzer.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode *root) : root_(root), symbol_table_() {
  error_free_ = true;
}

void SemanticAnalyzer::InitTraversal() {
  root_->Accept(new ASTNodeInitVisitor(&symbol_table_));
}

void SemanticAnalyzer::FullTraversal() {

}