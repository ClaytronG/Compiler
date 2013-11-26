// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

ReturnNode::ReturnNode() {
  expression_ = NULL;
}

ReturnNode::ReturnNode(ExpressionNode *expression) {
  expression_ = expression;
}

ReturnNode::~ReturnNode() {
  delete expression_;
}

void ReturnNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

ExpressionNode *ReturnNode::expression() const {
  return expression_;
}

void ReturnNode::set_expression(ExpressionNode *expression) {
  expression_ = expression;
}