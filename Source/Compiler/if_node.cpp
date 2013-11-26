// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

IfNode::IfNode() {
  else_statement_ = NULL;
  expression_ = NULL;
  then_statement_ = NULL;
}

IfNode::IfNode(ExpressionNode *expression, StatementNode *then_statement) {
  else_statement_ = NULL;
  expression_ = expression;
  then_statement_ = then_statement;
}

IfNode::IfNode(ExpressionNode *expression, StatementNode *then_statement,
               StatementNode *else_statement) {
  else_statement_ = else_statement;
  expression_ = expression;
  then_statement_ = then_statement;
}

IfNode::~IfNode() {
  delete else_statement_;
  delete expression_;
  delete then_statement_;
}

void IfNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

StatementNode *IfNode::else_statement() const {
  return else_statement_;
}

ExpressionNode *IfNode::expression() const {
  return expression_;
}

void IfNode::set_else_statement(StatementNode *statement) {
  else_statement_ = statement;
}

void IfNode::set_expression(ExpressionNode *expression) {
  expression_ = expression;
}

void IfNode::set_then_statement(StatementNode *statement) {
  then_statement_ = statement;
}

StatementNode *IfNode::then_statement() const {
  return then_statement_;
}