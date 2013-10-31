// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

VariableNode::VariableNode() {
  array_expression_  = NULL;
  array_variable_ = false;
  identifier_ = -1; // BAD
}

VariableNode::VariableNode(const int identifier) {
  array_expression_ = NULL;
  array_variable_ = false;
  identifier_ = identifier;
}

VariableNode::VariableNode(const int identifier, 
                           ExpressionNode *array_expression) {
  array_expression_ = array_expression;
  array_variable_ = true;
  identifier_ = identifier;
}

VariableNode::~VariableNode() {
  delete array_expression_;
}

void VariableNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}

ExpressionNode *VariableNode::array_expression() const {
  return array_expression_;
}

void VariableNode::set_array_expression(ExpressionNode *array_expression) {
  array_expression_ = array_expression;
}

void VariableNode::set_array_variable(const bool array_variable) {
  array_variable_ = array_variable;
}

void VariableNode::set_identifier(const int identifier) {
  identifier_ = identifier;
}

void VariableNode::set_identifier(const std::string &string_identifier) {
  Administrator::spelling_table[identifier_] = string_identifier;
}

std::string VariableNode::StringIdentifier() const {
  return Administrator::spelling_table[identifier_];
}

DeclarationNode *VariableNode::declaration_pointer() const {
  return declaration_pointer_;
}

void VariableNode::set_declaration_pointer(DeclarationNode *declaration_pointer) {
  declaration_pointer_ = declaration_pointer;
}