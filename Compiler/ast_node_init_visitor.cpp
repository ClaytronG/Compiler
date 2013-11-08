// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_init_visitor.h"

ASTNodeInitVisitor::ASTNodeInitVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator) 
  : symbol_table_(symbol_table), administrator_(administrator), filename_(filename) {
  error_free_ = true;
  static_array_expression_ = true;
}

void ASTNodeInitVisitor::Visit(ProgramNode &node) {
  node.declaration_node()->Accept(this);
}

void ASTNodeInitVisitor::Visit(FunctionDeclarationNode &node) {
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

void ASTNodeInitVisitor::Visit(VariableDeclarationNode &node) {
  if (symbol_table_->acces_table_.at(node.identifier()) == 0) {
    // Check to see if the array expression is an integer and can be statically defined
    // i.e. no variables in the expression
    if (node.array_expression()) {
      node.array_expression()->Accept(this);
      if (!static_array_expression_) {
        std::string message = "Array expression must be static";
        administrator_->messenger()->AddError(filename_, node.line_number(), message);
        error_free_ = false;
      } else {
        node.set_array_size(node.array_expression()->value());
      }
      static_array_expression_ = true;
    }
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

void ASTNodeInitVisitor::Visit(BinaryNode &node) { 
  node.left_expression()->Accept(this);
  int left_value = node.left_expression()->value();
  node.right_expression()->Accept(this);
  int right_value = node.right_expression()->value();
}

void ASTNodeInitVisitor::Visit(CallNode &node) {
  static_array_expression_ = false;
  node.set_type(Token::UNIVERSAL);
}

void ASTNodeInitVisitor::Visit(LiteralNode &node) { 
  if (node.boolean_literal()) {
    node.set_type(Token::BOOL);
  } 
  else if (node.number_literal()) {
    node.set_type(Token::INT);
  }
}

void ASTNodeInitVisitor::Visit(UnaryNode &node) { 
  node.expression()->Accept(this);
  if (node.op() == Token::MINUS) {
    node.set_type(Token::INT);
    node.set_value(-1 * node.expression()->value());
  }
  else if (node.op() == Token::NOT) {
    node.set_type(Token::BOOL);
    node.set_value(!(bool)(node.expression()->value()));
  }
}

void ASTNodeInitVisitor::Visit(VariableNode &node) { 
  static_array_expression_ = false;
  node.set_type(Token::UNIVERSAL);
}

// These nodes aren't traversed
void ASTNodeInitVisitor::Visit(AssignmentNode &node) { }
void ASTNodeInitVisitor::Visit(ASTNode &node) { }
void ASTNodeInitVisitor::Visit(BranchNode &node) { }
void ASTNodeInitVisitor::Visit(CaseNode &node) { }
void ASTNodeInitVisitor::Visit(CompoundNode &node) { }
void ASTNodeInitVisitor::Visit(ContinueNode &node) { }
void ASTNodeInitVisitor::Visit(DeclarationNode &node) { }
void ASTNodeInitVisitor::Visit(ExitNode &node) { }
void ASTNodeInitVisitor::Visit(ExpressionNode &node) { }
void ASTNodeInitVisitor::Visit(IfNode &node) { }
void ASTNodeInitVisitor::Visit(LoopNode &node) { }
void ASTNodeInitVisitor::Visit(NullNode &node) { }
void ASTNodeInitVisitor::Visit(ParameterNode &node) { }
void ASTNodeInitVisitor::Visit(ReturnNode &node) { }
void ASTNodeInitVisitor::Visit(StatementNode &node) { }