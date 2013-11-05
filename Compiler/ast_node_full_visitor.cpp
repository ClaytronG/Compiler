// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"

ASTNodeFullVisitor::ASTNodeFullVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator)
  : symbol_table_(symbol_table), filename_(filename), administrator_(administrator) {
  depth_ = 0;
  compound_variable_ = false;
  error_free_ = false;
  in_loop_ = false;
  nonvoid_function_ = false;
  return_statement_ = false;
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
  compound_variable_ = false;
  node.statements()->Accept(this);

  --depth_;
  SemanticAnalyzer::PrintSymbolTable(*symbol_table_);
  if ((symbol_table_->identifier_table_.end() - 1)->L != 0) {
    PopStack();
  }
}

void ASTNodeFullVisitor::Visit(const ContinueNode &node) { 
  if (!in_loop_) {
    std::string message = "CONTINUE statement outside of LOOP";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
}

void ASTNodeFullVisitor::Visit(const DeclarationNode &node) { }

void ASTNodeFullVisitor::Visit(const ExitNode &node) { 
  if (!in_loop_) {
    std::string message = "Exit statement outside of LOOP";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
}

void ASTNodeFullVisitor::Visit(const ExpressionNode &node) { }

void ASTNodeFullVisitor::Visit(const FunctionDeclarationNode &node) {
  // Add the parameter variables to the symbol table
  if (node.parameters() != NULL) {
    ++depth_;
    node.parameters()->Accept(this);
    nonvoid_function_ = true;
    --depth_;
  }
  // Continue onto the compound
  node.compound()->Accept(this);

  nonvoid_function_ = false;

  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(const IfNode &node) { 
  // Check the that the if expression is of BOOLEAN


  // Move on to the if statement
  node.then_statement()->Accept(this);

  // If there is an else statement visit it
  if (node.else_statement()) {
    node.else_statement()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(const LiteralNode &node) { }

void ASTNodeFullVisitor::Visit(const LoopNode &node) { 
  ++depth_;
  in_loop_ = true;

  node.statements()->Accept(this);

  --depth_;
  in_loop_ = false;
}

void ASTNodeFullVisitor::Visit(const NullNode &node) { }

void ASTNodeFullVisitor::Visit(const ParameterNode &node) {
  if (symbol_table_->identifier_table_.at(symbol_table_->acces_table_.at(node.identifier())).L != depth_) {
    IdentificationTableEntry entry;
    entry.L = depth_;
    entry.Next = symbol_table_->acces_table_.at(node.identifier());
    entry.DecPtr = &node;
    entry.LexI = node.identifier();
    symbol_table_->identifier_table_.push_back(entry);
    // Update the access table
    symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size()-1;
  } else {
    std::string message = "Double Definition: " + Token::kTokenStrings[node.type()] + " " + Administrator::spelling_table[node.identifier()];
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  // Move onto the next parameter node
  if (node.next_parameter()) {
    node.next_parameter()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(const ProgramNode &node) {
  // Continue onto the declarations of the program.
  node.declaration_node()->Accept(this);
}

void ASTNodeFullVisitor::Visit(const ReturnNode &node) {
  // Check if we're in a nonvoid function
  if (nonvoid_function_) {
    // If we're in a nonvoid function and there is no return expression, report
    // an error
    if (!node.expression()) {
      std::string message = "Return statement missing expression";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
    // If there is a return expression make sure it is returning the correct
    // type for the function.
    else {

    }
  }
}

void ASTNodeFullVisitor::Visit(const StatementNode &node) { }

void ASTNodeFullVisitor::Visit(const UnaryNode &node) { }

void ASTNodeFullVisitor::Visit(const VariableDeclarationNode &node) { 
  // If we are inside of a compound, then this is a local variable declaration
  if (compound_variable_) {
    if (symbol_table_->identifier_table_.at(symbol_table_->acces_table_.at(node.identifier())).L != depth_) {
      IdentificationTableEntry entry;
      entry.L = depth_;
      entry.DecPtr = &node;
      entry.Next = symbol_table_->acces_table_.at(node.identifier());
      entry.LexI = node.identifier();
      // Add this declaration to the Identification Table
      symbol_table_->identifier_table_.push_back(entry);
      // Update the Access Table
      symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size()-1;
      //symbol_table_->acces_table_.at(symbol_table_->identifier_table_.at(symbol_table_->identifier_table_.size()-1).Next) = node.identifier();
    } else {
      std::string message = "Double Definiton: " + Token::kTokenStrings[node.type()] + " " + Administrator::spelling_table[node.identifier()];
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
    if (node.next_variable_declaration()) {
      node.next_variable_declaration()->Accept(this);
    }
  }
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(const VariableNode &node) { }

void ASTNodeFullVisitor::PopStack() {  
  const int level = symbol_table_->identifier_table_.at(symbol_table_->identifier_table_.size() - 1).L;
  for (int i = symbol_table_->identifier_table_.size() - 1; i >= 0; --i) {
    // Check if we have popped all entries on this level
    if (symbol_table_->identifier_table_.at(i).L != level) {
      break;
    }
    // Update the access table
    symbol_table_->acces_table_.at(symbol_table_->identifier_table_.at(i).LexI) = symbol_table_->identifier_table_.at(i).Next;
    // Pop this element
    symbol_table_->identifier_table_.pop_back();
  }
}

Token::TokenName ASTNodeFullVisitor::EvaluateExpression(const ExpressionNode &node) {


  return Token::ERROR;
}