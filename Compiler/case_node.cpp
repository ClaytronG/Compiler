// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

CaseNode::CaseNode() {
  case_number_ = 0;
  default_case_ = false;
  statement_ = NULL;
}

CaseNode::CaseNode(StatementNode *statement) {
  case_number_ = 0;
  default_case_ = true;
  statement_ = statement;
}

CaseNode::CaseNode(const int case_number, StatementNode *statement) {
  case_number_ = case_number;
  default_case_ = false;
  statement_ = statement;
}

CaseNode::~CaseNode() {
  delete statement_;
}

void CaseNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}

void CaseNode::set_case_number(const int case_number) {
  case_number_ = case_number;
}

void CaseNode::set_default_case(const bool default_case) {
  default_case_ = default_case;
}

void CaseNode::set_statement(StatementNode *statement) {
  statement_ = statement;
}

StatementNode *CaseNode::statement() const {
  return statement_;
}