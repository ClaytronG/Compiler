// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

void NullNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}