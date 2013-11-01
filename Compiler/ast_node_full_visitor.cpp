// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"

ASTNodeFullVisitor::ASTNodeFullVisitor(SymbolTable *symbol_table)
  : symbol_table_(symbol_table) {
  depth_ = 0;
  compound_variable_ = false;
}

void ASTNodeFullVisitor::Visit(const AssignmentNode &node) { }
void ASTNodeFullVisitor::Visit(const ASTNode &node) { }
void ASTNodeFullVisitor::Visit(const BinaryNode &node) { }
void ASTNodeFullVisitor::Visit(const BranchNode &node) { }
void ASTNodeFullVisitor::Visit(const CallNode &node) { }
void ASTNodeFullVisitor::Visit(const CaseNode &node) { }

void ASTNodeFullVisitor::Visit(const CompoundNode &node) { 
  ++depth_;
  compound_variable_ = true;
  // If the compound contains local variable declarations, visit them
  if (node.local_variables()) {
    node.local_variables()->Accept(this);
  }
  node.statements()->Accept(this);
  --depth_;
}

void ASTNodeFullVisitor::Visit(const ContinueNode &node) { }
void ASTNodeFullVisitor::Visit(const DeclarationNode &node) { }
void ASTNodeFullVisitor::Visit(const ExitNode &node) { }
void ASTNodeFullVisitor::Visit(const ExpressionNode &node) { }

void ASTNodeFullVisitor::Visit(const FunctionDeclarationNode &node) { 
  ++depth_;
  // Add the parameter variables to the symbol table
  node.parameters()->Accept(this);

  // Continue onto the compound
  node.compound()->Accept(this);

  --depth_;
}

void ASTNodeFullVisitor::Visit(const IfNode &node) { }
void ASTNodeFullVisitor::Visit(const LiteralNode &node) { }
void ASTNodeFullVisitor::Visit(const LoopNode &node) { }
void ASTNodeFullVisitor::Visit(const NullNode &node) { }

void ASTNodeFullVisitor::Visit(const ParameterNode &node) { 
  IdentificationTableEntry entry;
  entry.L = depth_;
  entry.Next = symbol_table_->acces_table_.at(node.identifier());
  // Update the access table
  symbol_table_->acces_table_.at(symbol_table_->identifier_table_.size()) = node.identifier();
  entry.DecPtr = &node;
  entry.LexI = node.identifier();
  symbol_table_->identifier_table_.push_back(entry);
  // Move onto the next parameter node
  if (node.next_parameter()) {
    node.next_parameter()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(const ProgramNode &node) {
  // Continue onto the declarations of the program.
  node.declaration_node()->Accept(this);
}

void ASTNodeFullVisitor::Visit(const ReturnNode &node) { }
void ASTNodeFullVisitor::Visit(const StatementNode &node) { }
void ASTNodeFullVisitor::Visit(const UnaryNode &node) { }

void ASTNodeFullVisitor::Visit(const VariableDeclarationNode &node) { 
  // If we are inside of a compound, then this is a local variable declaration
  if (compound_variable_) {

  } else {
    node.next_node()->Accept(this);
  }
  compound_variable_ = false;
}

void ASTNodeFullVisitor::Visit(const VariableNode &node) { }