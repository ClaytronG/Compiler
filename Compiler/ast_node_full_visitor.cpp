// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"

ASTNodeFullVisitor::ASTNodeFullVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator)
  : symbol_table_(symbol_table), filename_(filename), administrator_(administrator), case_numbers_() {
  depth_ = 0;
  compound_variable_ = false;
  error_free_ = false;
  in_loop_ = false;
  nonvoid_function_ = false;
  return_statement_ = false;
  exit_statement_ = false;
  function_return_type_ = Token::ERROR;
  expression_return_type = Token::ERROR;
  default_case_ = false;
}

void ASTNodeFullVisitor::Visit(AssignmentNode &node) { 
  // Make sure the variable that's being assigned to has been defined
  if (symbol_table_->acces_table_.at(node.identifier()) == 0) {
    std::string message = "Undeclared identifier";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
    // Add it to the symbol table to avoid more errors
    IdentificationTableEntry entry;
    entry.L = depth_;
    entry.DecPtr = NULL;
    entry.Next = 0;
    entry.LexI = node.identifier();
    // Update the access table
    symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size() - 1;
    // Add this variable to the identifier table
    symbol_table_->identifier_table_.push_back(entry);
  }
  // Otherwise it's been defined the assignment type should be checked
  else {
    // TODO match assignment type
  }
}

void ASTNodeFullVisitor::Visit(ASTNode &node) { }

void ASTNodeFullVisitor::Visit(BinaryNode &node) { }

void ASTNodeFullVisitor::Visit(BranchNode &node) { 
  // TODO check if branch's expression resolves to INT


  // Check the cases
  node.cases()->Accept(this);
  default_case_ = false;
  case_numbers_.clear();
}

void ASTNodeFullVisitor::Visit(CallNode &node) { 
  // Make sure the function has been declared
  if (symbol_table_->acces_table_.at(node.identifier()) == 0) {
    std::string message = "Function not declarared";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
    expression_return_type = Token::UNIVERSAL;
    // Add this to the symbol table to avoid more errors
    IdentificationTableEntry entry;
    entry.L = depth_;
    entry.DecPtr = NULL;
    entry.Next = 0;
    entry.LexI = node.identifier();
    // Add it to the identifier table
    symbol_table_->identifier_table_.push_back(entry);
    // Update the access table
    symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size() - 1;
    return;
  }

  // Make sure arguments are consistent

}

void ASTNodeFullVisitor::Visit(CaseNode &node) { 
  // Check if it's a default case
  if (node.default_case()) {
    if (default_case_) {
      std::string message = "Default case already defined";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
    else {
      default_case_ = true;
    }
  }
  // Otherwise it's a numbered case, check to see if the number hasn't been used yet
  else {
    auto it = case_numbers_.find(node.case_number());
    if (it != case_numbers_.end()) {
      std::string message = "Case number already defined";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
    else {
      case_numbers_.insert(node.case_number());
    }
  }
  ++depth_;
  node.statement()->Accept(this);
  --depth_;
}

void ASTNodeFullVisitor::Visit(CompoundNode &node) { 
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

void ASTNodeFullVisitor::Visit(ContinueNode &node) { 
  if (!in_loop_) {
    std::string message = "CONTINUE statement outside of LOOP";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
}

void ASTNodeFullVisitor::Visit(DeclarationNode &node) { }

void ASTNodeFullVisitor::Visit(ExitNode &node) { 
  if (!in_loop_) {
    std::string message = "Exit statement outside of LOOP";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  else {
    exit_statement_ = true;
  }
}

void ASTNodeFullVisitor::Visit(ExpressionNode &node) { }

void ASTNodeFullVisitor::Visit(FunctionDeclarationNode &node) {
  function_return_type_ = node.type();

  // Add the parameter variables to the symbol table
  if (node.parameters() != NULL) {
    ++depth_;
    node.parameters()->Accept(this);
    nonvoid_function_ = true;
    --depth_;
  }
  // Continue onto the compound
  node.compound()->Accept(this);

  // Move onto the next declaration node
  nonvoid_function_ = false;
  function_return_type_ = Token::ERROR;
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(IfNode &node) { 
  // Check the that the if expression is of BOOLEAN


  // Move on to the if statement
  node.then_statement()->Accept(this);

  // If there is an else statement visit it
  if (node.else_statement()) {
    node.else_statement()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(LiteralNode &node) { }

void ASTNodeFullVisitor::Visit(LoopNode &node) { 
  ++depth_;
  in_loop_ = true;

  node.statements()->Accept(this);

  if (!exit_statement_) {
    std::string message = "Loop body missing exit statement";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }

  --depth_;
  in_loop_ = false;
}

void ASTNodeFullVisitor::Visit(NullNode &node) { }

void ASTNodeFullVisitor::Visit(ParameterNode &node) {
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

void ASTNodeFullVisitor::Visit(ProgramNode &node) {
  // Continue onto the declarations of the program.
  node.declaration_node()->Accept(this);
}

void ASTNodeFullVisitor::Visit(ReturnNode &node) {
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
      // TODO get type from return expression
    }
  }
}

void ASTNodeFullVisitor::Visit(StatementNode &node) { }

void ASTNodeFullVisitor::Visit(UnaryNode &node) { }

void ASTNodeFullVisitor::Visit(VariableDeclarationNode &node) { 
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
      symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size() - 1;
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

void ASTNodeFullVisitor::Visit(VariableNode &node) {
  // Check to see if this is an array variable
  if (node.array_variable()) {
    // If it is make sure the expression resolves to an integer
    // TODO check expression
  }
  IdentificationTableEntry entry;
  int index = symbol_table_->acces_table_.at(node.identifier());
  if (index == 0) {
    std::string message = "Undeclared identifier";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
    entry.L = depth_;
    entry.DecPtr = symbol_table_->identifier_table_.at(index).DecPtr;
    entry.Next = index;
    entry.LexI = node.identifier();
    // Add this declaration to the Identification table
    symbol_table_->identifier_table_.push_back(entry);
    // Update the access table
    symbol_table_->acces_table_.at(node.identifier()) = symbol_table_->identifier_table_.size() - 1;
  }
  else {
    // Link this variable with its declaration
    node.set_declaration_pointer(dynamic_cast<DeclarationNode*>(symbol_table_->identifier_table_.at(index).DecPtr));
  }
  // gross
  expression_return_type = dynamic_cast<const VariableDeclarationNode*>(symbol_table_->identifier_table_.at(symbol_table_->acces_table_.at(node.identifier())).DecPtr)->type();
}

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