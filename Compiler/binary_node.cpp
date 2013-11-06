// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

BinaryNode::BinaryNode() {
  left_expression_ = NULL;
  op_ = Token::VOID;
  right_expression_ = NULL;
}

BinaryNode::BinaryNode(const Token::TokenName &op) {
  left_expression_ = NULL;
  op_ = op;
  right_expression_ = NULL;
}

BinaryNode::BinaryNode(ExpressionNode *left_expression, 
                       ExpressionNode *right_expression) {
  left_expression_ = left_expression;
  op_ = Token::VOID;
  right_expression_ = right_expression;
}

BinaryNode::BinaryNode(const Token::TokenName &op, 
                       ExpressionNode *left_expression, 
                       ExpressionNode *right_expression) {
  left_expression_ = left_expression;
  op_ = op;
  right_expression_ = right_expression;
}

BinaryNode::~BinaryNode() {
  delete left_expression_;
  delete right_expression_;
}

void BinaryNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

ExpressionNode *BinaryNode::left_expression() const {
  return left_expression_;
}

Token::TokenName BinaryNode::op() const {
  return op_;
}

ExpressionNode *BinaryNode::right_expression() const {
  return right_expression_;
}

void BinaryNode::set_left_expression(ExpressionNode *expression) {
  left_expression_ = expression;
}

void BinaryNode::set_op(const Token::TokenName &op) {
  op_ = op;
}

void BinaryNode::set_right_expression(ExpressionNode *expression) {
  right_expression_ = expression;
}