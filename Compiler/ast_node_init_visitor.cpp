// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_init_visitor.h"

ASTNodeInitVisitor::ASTNodeInitVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator) 
  : symbol_table_(symbol_table), administrator_(administrator), filename_(filename) {
  error_free_ = true;
}

void ASTNodeInitVisitor::Visit(const ProgramNode &node) {
  node.declaration_node()->Accept(this);
}

void ASTNodeInitVisitor::Visit(const FunctionDeclarationNode &node) {
  // Check to see if this is a double definition
  if (symbol_table_->acces_table_.at(node.identifier()) == 0) {
    IdentificationTableEntry entry;
    entry.L = 0;
    entry.DecPtr = &node;
    entry.Next = 0;
    entry.LexI = node.identifier();
    // Add this declaration to the Identification Table
    symbol_table_->identifier_table_.push_back(entry);
    // Update the Access Table
    symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size() - 1;
  } else {
    std::string message = "Double Definiton: " + Token::kTokenStrings[node.type()] + " " + Administrator::spelling_table[node.identifier()] + "()";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeInitVisitor::Visit(const VariableDeclarationNode &node) {
  if (symbol_table_->acces_table_.at(node.identifier()) == 0) {
    IdentificationTableEntry entry;
    entry.L = 0;
    entry.DecPtr = &node;
    entry.Next = 0;
    entry.LexI = node.identifier();
    // Add this declaration to the Identification Table
    symbol_table_->identifier_table_.push_back(entry);
    // Update the Access Table
    symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size()-1;
  } else {
    std::string message = "Double Definiton: " + Token::kTokenStrings[node.type()] + " " + Administrator::spelling_table[node.identifier()];
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  if (node.next_variable_declaration()) {
    node.next_variable_declaration()->Accept(this);
  }
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

// These nodes aren't traversed
void ASTNodeInitVisitor::Visit(const VariableNode &node) { }
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
void ASTNodeInitVisitor::Visit(const IfNode &node) { }
void ASTNodeInitVisitor::Visit(const LiteralNode &node) { }
void ASTNodeInitVisitor::Visit(const LoopNode &node) { }
void ASTNodeInitVisitor::Visit(const NullNode &node) { }
void ASTNodeInitVisitor::Visit(const ParameterNode &node) { }
void ASTNodeInitVisitor::Visit(const ReturnNode &node) { }
void ASTNodeInitVisitor::Visit(const StatementNode &node) { }
void ASTNodeInitVisitor::Visit(const UnaryNode &node) { }