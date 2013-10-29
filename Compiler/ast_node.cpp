// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "administrator.h"
#include "ast_node.h"

ASTNode::~ASTNode() {
  delete next_node_;
}

ASTNode *ASTNode::next_node() const {
  return next_node_;
}

void ASTNode::set_next_node(ASTNode *next_node) {
  next_node_ = next_node;
}

ASTNode::ASTNode() {
  next_node_ = NULL;
}