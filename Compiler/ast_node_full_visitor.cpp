// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"

ASTNodeFullVisitor::ASTNodeFullVisitor(SymbolTable *symbol_table, const std::string &filename, Administrator *administrator)
  : symbol_table_(symbol_table), filename_(filename), administrator_(administrator), case_numbers_() {
  depth_ = 0;
  compound_variable_ = false;
  error_free_ = true;
  in_loop_ = false;
  nonvoid_function_ = false;
  return_statement_ = false;
  exit_statement_ = false;
  default_case_ = false;
  variable_array_expression_ = false;
  void_function_valid = true;
  function_return_type_ = Token::ERROR;
}

void ASTNodeFullVisitor::Visit(AssignmentNode &node) {
  // Make sure the variable that's being assigned to has been defined
  if (symbol_table_->acces_table_.at(node.identifier()) == 0) {
    std::string message = "Undeclared identifier";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
    // If it is an array, check that expression
    if (node.array_assignment()) {
      node.array_expression()->Accept(this);
      if (node.array_expression()->type() != Token::INT) {
        std::string message = "Array expression must be of type INT";
        administrator_->messenger()->AddError(filename_, node.line_number(), message);
        error_free_ = false;
      }
    }
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
  // If it is an array, check that expression
  if (node.array_assignment()) {
    void_function_valid = false;
    node.array_expression()->Accept(this);
    if (node.array_expression()->type() != Token::INT) {
      std::string message = "Array expression must be of type INT";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
  }

  void_function_valid = false;
  // Make sure the types match
  node.value()->Accept(this);
  Token::TokenName type = dynamic_cast<DeclarationNode*>(symbol_table_->identifier_table_.at(symbol_table_->acces_table_.at(node.identifier())).DecPtr)->type();
  if (node.value()->type() != Token::UNIVERSAL && type != node.value()->type()) {
    std::string message = "Invalid assignment type";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  void_function_valid = true;

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(BinaryNode &node) { 
  node.left_expression()->Accept(this);
  Token::TokenName left_type = node.left_expression()->type();
  node.right_expression()->Accept(this);
  Token::TokenName right_type = node.right_expression()->type();
  
  if (left_type != right_type) {
    // If one type is universal, an error has already been reported
    if (left_type == Token::UNIVERSAL) {
      node.set_type(right_type);
    }
    else if (right_type == Token::UNIVERSAL) {
      node.set_type(left_type);
    }
    else {
      std::string message = "Operands must be of the same type";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
      // Choose the left expression for reasons
      node.set_type(Token::UNIVERSAL);
    }
  }
  // Otherwise both expressions are of the same type
  else {
    if (left_type == Token::BOOL) {
      if (node.op() == Token::AND || node.op() == Token::ANDTHEN || node.op() == Token::OR || 
          node.op() == Token::ORELSE || node.op() == Token::NOT || node.op() == Token::LTEQ ||
          node.op() == Token::LT || node.op() == Token::GT || node.op() == Token::GTEQ || 
          node.op() == Token::EQ || node.op() == Token::NEQ) {
        node.set_type(Token::BOOL);
      }
      else {
        std::string message = "Invalid operator";
        administrator_->messenger()->AddError(filename_, node.line_number(), message);
        error_free_ = false;
        // TODO: What type should this be?
        node.set_type(Token::UNIVERSAL); // ?
      }
    }
    else if (left_type == Token::INT) {
      if (node.op() == Token::PLUS || node.op() == Token::MINUS || node.op() == Token::MULT || 
          node.op() == Token::DIV || node.op() == Token::MOD) {
        node.set_type(Token::INT);
      }
      else if (node.op() == Token::LTEQ || node.op() == Token::LT || node.op() == Token::GT || 
               node.op() == Token::GTEQ || node.op() == Token::EQ || node.op() == Token::NEQ) {
        node.set_type(Token::BOOL);
      }
      else {
        std::string message = "Invalid operator";
        administrator_->messenger()->AddError(filename_, node.line_number(), message);
        error_free_ = false;
        // TODO: What type should this be?
        node.set_type(Token::UNIVERSAL); // ?
      }
    }
    // Otherwise both nodes are UNIVERSAL type, so the operator has
    // to be as well.
    else {
      node.set_type(Token::UNIVERSAL);
    }
  }
}

void ASTNodeFullVisitor::Visit(BranchNode &node) {
  void_function_valid = false;
  node.expresion()->Accept(this);
  void_function_valid = true;
  if (node.expresion()->type() != Token::UNIVERSAL && node.expresion()->type() != Token::INT) {
    std::string message = "BRANCH expression must be of type INT";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }

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
  else {
    int identifier_index = symbol_table_->acces_table_.at(node.identifier());
    FunctionDeclarationNode *declaration = dynamic_cast<FunctionDeclarationNode*>(symbol_table_->identifier_table_.at(identifier_index).DecPtr);
    node.set_type(declaration->type());
    node.set_declaration(declaration);
  }

  if (node.arguments()) {
    node.arguments()->Accept(this);
  }

  // Make sure arguments are consistent
  ParameterNode *current_param = dynamic_cast<FunctionDeclarationNode*>(symbol_table_->identifier_table_.at(symbol_table_->acces_table_.at(node.identifier())).DecPtr)->parameters();
  ExpressionNode *current_arg = node.arguments();
  while (current_arg != NULL) {
    current_arg->Accept(this);
    // Check if current_param exist, if not then 
    if (current_param == NULL) {
      std::string message = "Too many arguments";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
      break;
    }
    // Check if the arguments are the correct type
    else {
      if (current_arg->type() != current_param->type()) {
        std::string message = "Invalid type";
        administrator_->messenger()->AddError(filename_, node.line_number(), message);
        error_free_ = false;
        break;
      }
      else if (current_param->array_parameter()) {
        VariableNode *temp = dynamic_cast<VariableNode*>(current_arg);
        if (temp == NULL) { // The node isn't a variable
          std::string message = "Expected array argument";
          administrator_->messenger()->AddError(filename_, node.line_number(), message);
          error_free_ = false;
        }
        else if (!temp->array_expression()) { // The node isn't an array variable
          std::string message = "Expected array argument";
          administrator_->messenger()->AddError(filename_, node.line_number(), message);
          error_free_ = false;
        }
      }
      else if (current_param->reference_parameter()) {

      }
    }
    current_arg = dynamic_cast<ExpressionNode*>(current_arg->next_node());
    current_param = dynamic_cast<ParameterNode*>(current_param->next_parameter());
  }
  if (current_param != NULL) {
    std::string message = "Missing arguments";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  
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
    // If the iterator is at the end, this is a new case number
    if (it == case_numbers_.end()) {
      case_numbers_.insert(node.case_number());
    }
    // Otherwise, it is a duplicate. Report an error.
    else {
      std::string message = "Case number already defined";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
  }
  ++depth_;
  node.statement()->Accept(this);
  --depth_;

  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
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

void ASTNodeFullVisitor::Visit(ExitNode &node) { 
  if (!in_loop_) {
    std::string message = "EXIT statement outside of LOOP";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }
  else {
    exit_statement_ = true;
  }
}

void ASTNodeFullVisitor::Visit(FunctionDeclarationNode &node) {
  function_return_type_ = node.type();

  // Add the parameter variables to the symbol table
  nonvoid_function_ = false;
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
  node.expression()->Accept(this);
  if (node.expression()->type() != Token::BOOL) {
    std::string message = "If expression must be of type BOOL";
    administrator_->messenger()->AddError(filename_, node.line_number(), message);
    error_free_ = false;
  }

  // Move on to the if statement
  node.then_statement()->Accept(this);

  // If there is an else statement visit it
  if (node.else_statement()) {
    node.else_statement()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(LiteralNode &node) { 
  if (node.boolean_literal()) {
    node.set_type(Token::BOOL);
  }
  else if (node.number_literal()) {
    node.set_type(Token::INT);
  }
}

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
  exit_statement_ = false;
  in_loop_ = false;
}

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
      node.expression()->Accept(this);
      if (function_return_type_ != node.expression()->type()) {
        std::string message = "Invalid return type";
        administrator_->messenger()->AddError(filename_, node.line_number(), message);
        error_free_ = false;
      }
    }
  }
}

void ASTNodeFullVisitor::Visit(UnaryNode &node) {
  node.expression()->Accept(this);

  if (node.op() == Token::MINUS) {
    if (node.expression()->type() != Token::UNIVERSAL &&
        node.expression()->type() != Token::INT) {
      std::string message = "Invalid type on operator, expected INT";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
    node.set_type(Token::INT);
  }
  else if (node.op() == Token::NOT) {
    if (node.expression()->type() != Token::UNIVERSAL && 
        node.expression()->type() != Token::BOOL) {
      std::string message = "Invalid type on operator, expected BOOL";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
    }
    node.set_type(Token::BOOL);
  }

}

void ASTNodeFullVisitor::Visit(VariableDeclarationNode &node) { 
  // If we are inside of a compound, then this is a local variable declaration. InitTraversal will have taken
  // care of global variable declarations.
  if (compound_variable_) {
    // Check to see if this identifier is already being used in this scope.
    if (symbol_table_->identifier_table_.at(symbol_table_->acces_table_.at(node.identifier())).L != depth_) {
      // If this is an array variable
      if (node.array_variable()) {
        // Make sure its size is statically defined
        variable_array_expression_ = false;
        node.array_expression()->Accept(this);
        if (variable_array_expression_) {
          std::string message = "Array expression must be static";
          administrator_->messenger()->AddError(filename_, node.line_number(), message);
          error_free_ = false;
          variable_array_expression_ = false;
        }

        if (node.array_expression()->type() != Token::UNIVERSAL && node.array_expression()->type() != Token::INT) {
          std::string message = "Array expression must be of type INT";
          administrator_->messenger()->AddError(filename_, node.line_number(), message);
          error_free_ = false;
        }
      }
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
  }
  if (node.next_node()) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeFullVisitor::Visit(VariableNode &node) {
  // Check to see if this is an array variable
  if (node.array_variable()) {
    // Go into the expression and resolve type
    node.array_expression()->Accept(this);
    // If it is make sure the expression resolves to an integer
    if (node.array_expression()->type() != Token::UNIVERSAL && node.array_expression()->type() != Token::INT) {
      std::string message = "Array expression must be of type INT";
      administrator_->messenger()->AddError(filename_, node.line_number(), message);
      error_free_ = false;
    }
  }
  int index = symbol_table_->acces_table_.at(node.identifier());
  if (index == 0) {
    IdentificationTableEntry entry;
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
    node.set_type(Token::UNIVERSAL);
  }
  else {
    // Link this variable with its declaration
    ASTNode *node1 = symbol_table_->identifier_table_.at(index).DecPtr;
    VariableDeclarationNode *var = dynamic_cast<VariableDeclarationNode*>(node1);
    ParameterNode *param = dynamic_cast<ParameterNode*>(node1);
    if (var != NULL) {
      node.set_declaration_pointer(var);
    }
    else if (param != NULL) {
      node.set_declaration_pointer(param);
    }
    //node.set_declaration_pointer(dynamic_cast<DeclarationNode*>(symbol_table_->identifier_table_.at(index).DecPtr));
    node.set_type(node.declaration_pointer()->type());
    int x = 3;
  }
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

void ASTNodeFullVisitor::AddNode(ASTNode *node) {
  IdentificationTableEntry entry;
  entry.L = depth_;
  entry.DecPtr = NULL;
}


void ASTNodeFullVisitor::Visit(ASTNode &node) { }
void ASTNodeFullVisitor::Visit(DeclarationNode &node) { }
void ASTNodeFullVisitor::Visit(ExpressionNode &node) { }
void ASTNodeFullVisitor::Visit(NullNode &node) { }
void ASTNodeFullVisitor::Visit(StatementNode &node) { }