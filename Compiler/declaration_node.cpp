// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

void DeclarationNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}

void DeclarationNode::set_identifier(int index) {
  // Make sure the index isn't being set out of bounds.
#pragma warning(disable:4018)
  if (index < 0 || index > Administrator::spelling_table.size() ) {
    return;
  }
#pragma warning(default:4018)
  identifier_ = index;
}

void DeclarationNode::set_identifier(const std::string &string_identifier) {
  // Access the the spelling table in the administrator and give it the 
  // new value.
  Administrator::spelling_table[identifier_] = string_identifier;
}

void DeclarationNode::set_type(const Token::TokenName &type) {
  type_ = type;
}

std::string DeclarationNode::StringIdentifier() const {
  return Administrator::spelling_table[identifier_];
}

Token::TokenName DeclarationNode::type() const {
  return type_;
}

DeclarationNode::DeclarationNode() {
  identifier_ = -1; // Not good, user must change or bad things will happen.
  type_ = Token::VOID;
}

DeclarationNode::DeclarationNode(const int identifier, 
                                 const Token::TokenName &type) {
  identifier_ = identifier;
  type_ = type;
}