// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

void ExitNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}