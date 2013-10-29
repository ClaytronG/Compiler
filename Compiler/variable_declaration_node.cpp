// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

VariableDeclarationNode::VariableDeclarationNode() {
  array_expression_ = NULL;
  array_variable_ = false;
  next_variable_declaration_ = NULL;
}

VariableDeclarationNode::VariableDeclarationNode(
    VariableDeclarationNode *next_variable_declaration) {
  next_variable_declaration_ = next_variable_declaration;
}

VariableDeclarationNode::VariableDeclarationNode(const int identifier, 
                                                 const Token::TokenName &type)
  : DeclarationNode(identifier, type) {
  array_expression_ = NULL;
  array_variable_ = false;
  next_variable_declaration_ = NULL;
}

VariableDeclarationNode::VariableDeclarationNode(
    const int identifier,
    const Token::TokenName &type,
    VariableDeclarationNode *next_variable_declaration) 
  : DeclarationNode(identifier, type) {
  array_expression_ = NULL;
  array_variable_ = false;
  next_variable_declaration_ = next_variable_declaration;
}


VariableDeclarationNode::~VariableDeclarationNode() {
  delete array_expression_;
  delete next_variable_declaration_;
}

void VariableDeclarationNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}

ExpressionNode *VariableDeclarationNode::array_expression() const {
  return array_expression_;
}

VariableDeclarationNode *VariableDeclarationNode::next_variable_declaration() const {
  return next_variable_declaration_;
}

void VariableDeclarationNode::set_array_expression(ExpressionNode *expression) {
  array_expression_ = expression;
}

void VariableDeclarationNode::set_array_variable(const bool array_variable) {
  array_variable_ = array_variable;
}

void VariableDeclarationNode::set_next_variable_declaration(
    VariableDeclarationNode *next_variable_declaration) {
  next_variable_declaration_ = next_variable_declaration;
}