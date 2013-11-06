// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

void NullNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}