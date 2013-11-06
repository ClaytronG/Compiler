// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

void StatementNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}