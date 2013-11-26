// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

BranchNode::BranchNode() {
  cases_ = NULL;
  expression_ = NULL;
}

BranchNode::BranchNode(ExpressionNode *expression, CaseNode *cases) {
  cases_ = cases;
  expression_ = expression;
}

BranchNode::~BranchNode() {
  delete cases_;
  delete expression_;
}

void BranchNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

CaseNode *BranchNode::cases() const {
  return cases_;
}

ExpressionNode *BranchNode::expresion() const {
  return expression_;
}

void BranchNode::set_cases(CaseNode *cases) {
  cases_ = cases;
}

void BranchNode::set_expression(ExpressionNode *expression) {
  expression_ = expression;
}