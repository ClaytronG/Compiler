// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

CallNode::CallNode() {
  arguments_ = NULL;
  identifier_ = -1; // BAD
  declaration_ = NULL;
}

CallNode::CallNode(const int identifier) {
  arguments_ = NULL;
  declaration_ = NULL;
  identifier_ = identifier;
}

CallNode::CallNode(const int identifier, ExpressionNode *argument) {
  arguments_ = argument;
  identifier_ = identifier;
  declaration_ = NULL;
}

CallNode::~CallNode() {
  delete arguments_;
}

void CallNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

ExpressionNode *CallNode::arguments() const {
  return arguments_;
}

void CallNode::set_argurments(ExpressionNode *argument) {
  arguments_ = argument;
}

void CallNode::set_identifier(const int identifier) {
  identifier_ = identifier;
}

void CallNode::set_identifier(const std::string &string_identifier) {
  Administrator::spelling_table[identifier_] = string_identifier;
}

std::string CallNode::StringIdentifier() const {
  return Administrator::spelling_table[identifier_];
}

FunctionDeclarationNode *CallNode::declaration() const {
  return declaration_;
}

void CallNode::set_declaration(FunctionDeclarationNode *declaration) {
  declaration_ = declaration;
}