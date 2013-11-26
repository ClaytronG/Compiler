// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

ProgramNode::ProgramNode() {
  declaration_node_ = NULL;
}

ProgramNode::ProgramNode(DeclarationNode *declaration_node) {
  declaration_node_ = declaration_node;
}

ProgramNode::~ProgramNode() {
  delete declaration_node_;
}

void ProgramNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

DeclarationNode *ProgramNode::declaration_node() const {
  return declaration_node_;
}

void ProgramNode::set_declaration_node(DeclarationNode *declaration_node) {
  declaration_node_ = declaration_node;
}