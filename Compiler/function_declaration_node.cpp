// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node.h"

FunctionDeclarationNode::FunctionDeclarationNode() {
  parameters_ = NULL;
}

FunctionDeclarationNode::FunctionDeclarationNode(ParameterNode *parameter) {
  parameters_ = parameter;
}

FunctionDeclarationNode::FunctionDeclarationNode(const int identifier,
                                                 const Token::TokenName &type)
  : DeclarationNode(identifier, type) {
  parameters_ = NULL;
}

FunctionDeclarationNode::FunctionDeclarationNode(const int identifier,
                                                 const Token::TokenName &type,
                                                 ParameterNode *parameter)
  : DeclarationNode(identifier, type) {
  parameters_ = parameter;
}

void FunctionDeclarationNode::Accept(ASTNodeVisitor *visitor) const {
  visitor->Visit(*this);
}

CompoundNode *FunctionDeclarationNode::compound() const {
  return compound_;
}

FunctionDeclarationNode::~FunctionDeclarationNode() {
  delete parameters_;
}

ParameterNode *FunctionDeclarationNode::parameters() const {
  return parameters_;
}

void FunctionDeclarationNode::set_compound(CompoundNode *compound) {
  compound_ = compound;
}

void FunctionDeclarationNode::set_parameter(ParameterNode *parameter) {
  parameters_ = parameter;
}