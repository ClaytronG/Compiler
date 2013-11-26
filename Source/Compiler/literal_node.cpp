// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

void LiteralNode::Accept(ASTNodeVisitor *visitor) {
  visitor->Visit(*this);
}

void LiteralNode::set_boolean_literal(const bool boolean_literal) {
  boolean_literal_ = boolean_literal;
  if (boolean_literal) {
    number_literal_ = false;
  }
}

void LiteralNode::set_number_literal(const bool number_literal) {
  number_literal_ = number_literal;
  if (number_literal) {
    boolean_literal_ = false;
  }
}