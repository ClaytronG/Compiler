// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

ParameterNode::ParameterNode() {
  array_parameter_ = false;
  identifier_ = -1; // This is BAD. User has to change later.
  next_parameter_ = NULL;
  reference_parameter_ = false;
  type_ = Token::VOID;
}

ParameterNode::ParameterNode(const int identifier,
                             const Token::TokenName &type) {
  array_parameter_ = false;
  identifier_ = identifier;
  next_parameter_ = NULL;
  reference_parameter_ = false;
  type_ = type;
}

ParameterNode::ParameterNode(const int identifier, 
                             const Token::TokenName &type,
                             ParameterNode *next_parameter) {
  array_parameter_ = false;
  identifier_ = identifier;
  next_parameter_ = next_parameter;
  reference_parameter_ = false;
  type_ = type;
}

ParameterNode::~ParameterNode() {
  delete next_parameter_;
}

void ParameterNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

ParameterNode *ParameterNode::next_parameter() const {
  return next_parameter_;
}

void ParameterNode::set_array_parameter(const bool array_parameter) {
  array_parameter_ = array_parameter;
  if (array_parameter) {
    reference_parameter_ = false;
  }
}

void ParameterNode::set_identifier(const int index) {
  identifier_ = index;
}

void ParameterNode::set_identifier(const std::string &string_identifier) {
  Administrator::spelling_table[identifier_] = string_identifier;
}

void ParameterNode::set_next_parameter(ParameterNode *next_parameter) {
  next_parameter_ = next_parameter;
}

void ParameterNode::set_reference_parameter(const bool reference_parameter) {
  reference_parameter_ = reference_parameter;
  if (reference_parameter) {
    array_parameter_ = false;
  }
}

void ParameterNode::set_type(const Token::TokenName &type) {
  type_ = type;
}

std::string ParameterNode::StringIdentifier() const {
  return Administrator::spelling_table[identifier_];
}

Token::TokenName ParameterNode::type() const {
  return type_;
}