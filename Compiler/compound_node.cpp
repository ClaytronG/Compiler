// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

CompoundNode::CompoundNode() {
  local_variables_ = NULL;
  statements_ = NULL;
}

CompoundNode::CompoundNode(StatementNode *statement) {
  local_variables_ = NULL;
  statements_ = statement;
}

CompoundNode::CompoundNode(VariableDeclarationNode *variable, 
                           StatementNode *statement) {
  local_variables_ = variable;
  statements_ = statement;
}

CompoundNode::~CompoundNode() {
  delete local_variables_;
  delete statements_;
}

void CompoundNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this); 
}

VariableDeclarationNode *CompoundNode::local_variables() const {
  return local_variables_;
}

void CompoundNode::set_local_variable(VariableDeclarationNode *variable) {
  local_variables_ = variable;
}

void CompoundNode::set_statement(StatementNode *statement) {
  statements_ = statement;
}

StatementNode *CompoundNode::statements() const {
  return statements_;
}