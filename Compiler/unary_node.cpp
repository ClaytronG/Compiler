// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

UnaryNode::UnaryNode() {
  op_ = Token::ERROR; // BAD
  expression_ = NULL;
}

UnaryNode::UnaryNode(const Token::TokenName &op) {
  op_ = op;
}

UnaryNode::UnaryNode(ExpressionNode *expression) {
  expression_ = expression;
}

UnaryNode::UnaryNode(const Token::TokenName &op, ExpressionNode *expression) {
  op_ = op;
  expression_ = expression;
}

UnaryNode::~UnaryNode() {
  delete expression_;
}

void UnaryNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
  expression_->Accept(visitor);
}

ExpressionNode *UnaryNode::expression() const {
  return expression_;
}

Token::TokenName UnaryNode::op() const {
  return op_;
}

void UnaryNode::set_expression(ExpressionNode *expression) {
  expression_ = expression;
}

void UnaryNode::set_op(const Token::TokenName &op) {
  op_ = op;
}