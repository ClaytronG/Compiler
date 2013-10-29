// Author: Clayton Green (kgreen1@unbc.ca)
// 
//

#include "ast_node.h"

void ExpressionNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}