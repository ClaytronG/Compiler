// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

LoopNode::LoopNode(StatementNode *statement) {
  statements_ = statement;
}

LoopNode::~LoopNode() {
  delete statements_;
}

void LoopNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}

void LoopNode::set_statements(StatementNode *statement) {
  statements_ = statement;
}

StatementNode *LoopNode::statements() const {
  return statements_;
}