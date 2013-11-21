// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

VariableDeclarationNode::VariableDeclarationNode() {
  array_expression_ = NULL;
  array_variable_ = false;
}

VariableDeclarationNode::VariableDeclarationNode(
    VariableDeclarationNode *next_variable_declaration) {
  set_next_node(next_variable_declaration);
}

VariableDeclarationNode::VariableDeclarationNode(const int identifier, 
                                                 const Token::TokenName &type)
  : DeclarationNode(identifier, type) {
  array_expression_ = NULL;
  array_variable_ = false;
}

VariableDeclarationNode::VariableDeclarationNode(
    const int identifier,
    const Token::TokenName &type,
    VariableDeclarationNode *next_variable_declaration) 
  : DeclarationNode(identifier, type) {
  array_expression_ = NULL;
  array_variable_ = false;
  set_next_node(next_variable_declaration);
}


VariableDeclarationNode::~VariableDeclarationNode() {
  delete array_expression_;
}

void VariableDeclarationNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

ExpressionNode *VariableDeclarationNode::array_expression() const {
  return array_expression_;
}

void VariableDeclarationNode::set_array_expression(ExpressionNode *expression) {
  array_expression_ = expression;
}

void VariableDeclarationNode::set_array_variable(const bool array_variable) {
  array_variable_ = array_variable;
}