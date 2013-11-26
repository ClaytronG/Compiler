// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

AssignmentNode::AssignmentNode() {
  array_assignment_ = false;
  array_expression_ = NULL;
  identifier_ = -1; // BAD
  value_ = NULL;
}

AssignmentNode::AssignmentNode(const int identifier, 
                               ExpressionNode *expression) {
  array_assignment_ = false;
  array_expression_ = NULL;
  identifier_ = identifier;
  value_ = expression;
}

AssignmentNode::AssignmentNode(const int identifier,
                               ExpressionNode *array_expression,
                               ExpressionNode *expression,
                               const bool array_assignment) {
  array_assignment_ = array_assignment;
  array_expression_ = array_expression;
  identifier_ = identifier;
  value_ = expression;
}

AssignmentNode::~AssignmentNode() {
  delete value_;
}

void AssignmentNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

ExpressionNode *AssignmentNode::array_expression() const {
  return array_expression_;
}

void AssignmentNode::set_array_assignment(const bool array_assignment) {
  array_assignment_ = array_assignment;
}

void AssignmentNode::set_array_expression(ExpressionNode *array_expression) {
    array_expression_ = array_expression;
}

void AssignmentNode::set_identifier(const int identifier) {
  identifier_ = identifier;
}

void AssignmentNode::set_identifier(const std::string &string_identifier) {
  Administrator::spelling_table[identifier_] = string_identifier;
}

void AssignmentNode::set_value(ExpressionNode *value) {
  value_ = value;
}

std::string AssignmentNode::StringIdentifier() const {
  return Administrator::spelling_table[identifier_];
}

ExpressionNode *AssignmentNode::value() const {
  return value_;
}