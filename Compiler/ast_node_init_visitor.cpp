// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_init_visitor.h"

ASTNodeInitVisitor::ASTNodeInitVisitor(SymbolTable *symbol_table) 
  : symbol_table_(symbol_table) {

}

void ASTNodeInitVisitor::Visit(const AssignmentNode &node) { }
void ASTNodeInitVisitor::Visit(const ASTNode &node) { }
void ASTNodeInitVisitor::Visit(const BinaryNode &node) { }
void ASTNodeInitVisitor::Visit(const BranchNode &node) { }
void ASTNodeInitVisitor::Visit(const CallNode &node) { }
void ASTNodeInitVisitor::Visit(const CaseNode &node) { }
void ASTNodeInitVisitor::Visit(const CompoundNode &node) { }
void ASTNodeInitVisitor::Visit(const ContinueNode &node) { }
void ASTNodeInitVisitor::Visit(const DeclarationNode &node) { }
void ASTNodeInitVisitor::Visit(const ExitNode &node) { }
void ASTNodeInitVisitor::Visit(const ExpressionNode &node) { }

void ASTNodeInitVisitor::Visit(const FunctionDeclarationNode &node) {
  IdentificationTableEntry entry;
  entry.L = 0;
  entry.DecPtr = &node;
  entry.Next = 0;
  entry.LexI = node.identifier();
  symbol_table_->PushBack(entry);
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeInitVisitor::Visit(const IfNode &node) { }
void ASTNodeInitVisitor::Visit(const LiteralNode &node) { }
void ASTNodeInitVisitor::Visit(const LoopNode &node) { }
void ASTNodeInitVisitor::Visit(const NullNode &node) { }
void ASTNodeInitVisitor::Visit(const ParameterNode &node) { }

void ASTNodeInitVisitor::Visit(const ProgramNode &node) {
  node.declaration_node()->Accept(this);
}

void ASTNodeInitVisitor::Visit(const ReturnNode &node) { }
void ASTNodeInitVisitor::Visit(const StatementNode &node) { }
void ASTNodeInitVisitor::Visit(const UnaryNode &node) { }

void ASTNodeInitVisitor::Visit(const VariableDeclarationNode &node) {
  IdentificationTableEntry entry;
  entry.L = 0;
  entry.DecPtr = &node;
  entry.Next = 0;
  entry.LexI = node.identifier();
  symbol_table_->PushBack(entry);
  if (node.next_variable_declaration()) {
    node.next_variable_declaration()->Accept(this);
  }
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeInitVisitor::Visit(const VariableNode &node) { }