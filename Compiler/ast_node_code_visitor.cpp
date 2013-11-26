#include "ast_node_code_visitor.h"

ASTNodeCodeVisitor::ASTNodeCodeVisitor(std::string &output) : output_(&output) {
  function_compound_ = false;
  current_num_parameters_ = -1;
  current_temp_variable_ = 1;
  last_variable_ = -1;
  current_continue_jump_ = 1;
  current_exit_jump_ = 1;
  current_label_ = 1;
  current_branch_ = 1;
};

void ASTNodeCodeVisitor::Visit(AssignmentNode &node) {
  CallNode *call =dynamic_cast<CallNode*>(node.value());
  LiteralNode *lit = dynamic_cast<LiteralNode*>(node.value());
  VariableNode *var = dynamic_cast<VariableNode*>(node.value());
  std::string result = Administrator::spelling_table[node.identifier()];
  if (call != NULL) {
    if (Administrator::spelling_table[call->identifier()].compare("readint") == 0) {
      *output_ += CreateQuad("rdi", "-", "-", result);
    }
    else if (Administrator::spelling_table[call->identifier()].compare("readbool") == 0) {
      *output_ += CreateQuad("rdb", "-", "-", result);
    }
    // Otherwise it is not a call to the compiler functions
    else {
      std::string temp = "t" + Administrator::IntToString(++current_temp_variable_);
      call->Accept(this);
      *output_ += CreateQuad("asg", temp, "-", result);
    }
  }
  else if (lit != NULL) {
    if (lit->number_literal()) {
      *output_ += CreateQuad("asg", Administrator::IntToString(lit->value()), "-", result);
    }
    else if (lit->boolean_literal()) {
      std::string value;
      if (lit->value() == 1) {
        value = "true";
      }
      else if (lit->value() == 0) {
        value = "false";
      }
      *output_ += CreateQuad("asg", value, "-", result);
    }
    else {
      printf("Something Went Wrong :(\n");
    }
  }
  else if (var != NULL) {
    *output_ += CreateQuad("asg", Administrator::spelling_table[var->identifier()], "-", result);
  }
  else {
    // Create the quadruples for the assignment
    node.value()->Accept(this);
    // Assign the value
    std::string temp = "t" + Administrator::IntToString(current_temp_variable_);
    *output_ += CreateQuad("asg", temp, "-", result);
  }

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(BinaryNode &node) {
  LiteralNode *left_lit = dynamic_cast<LiteralNode*>(node.left_expression());
  LiteralNode *right_lit = dynamic_cast<LiteralNode*>(node.right_expression());
  VariableNode *left_var = dynamic_cast<VariableNode*>(node.left_expression());
  VariableNode *right_var = dynamic_cast<VariableNode*>(node.right_expression());
  std::string left, right, result = "t" + Administrator::IntToString(++current_temp_variable_);

  // Have to do special short circuit things with ANDTHEN
  if (node.op() == Token::ANDTHEN) {

  }
  // and ORELSE
  else if (node.op() == Token::ORELSE) {

  }
  else {
    std::string op;
    switch (node.op()) {
    case Token::LTEQ:
      op = "lte";
      break;
    case Token::LT:
      op = "lt";
      break;
    case Token::GT:
      op = "gt";
      break;
    case Token::GTEQ:
      op = "gte";
      break;
    case Token::EQ:
      op = "eq";
      break;
    case Token::NEQ:
      op = "neq";
      break;
    case Token::PLUS:
      op = "add";
      break;
    case Token::MINUS:
      op = "sub";
      break;
    case Token::OR:
      op = "or";
      break;
    case Token::MULT:
      op = "mul";
      break;
    case Token::DIV:
      op = "div";
      break;
    case Token::MOD:
      op = "mod";
      break;
    case Token::AND:
      op = "and";
      break;
    }

    if (left_var != NULL) {
      if (left_var->array_variable()) {

      }
      else {
        left = Administrator::spelling_table[left_var->identifier()];
      }
    }
    else if (left_lit != NULL) {
      if (left_lit->boolean_literal()) {
        if (left_lit->value() == 0) {
          left = "false";
        }
        else if (left_lit->value() == 1) {
          left = "true";
        }
      }
      else if (left_lit->number_literal()) {
        left = Administrator::IntToString(left_lit->value());
      }
    }
    else {
      left = "t" + Administrator::IntToString(current_temp_variable_++);
      node.left_expression()->Accept(this);
    }


    if (right_var != NULL) {
      if (right_var->array_variable()) {

      }
      else {
        right = Administrator::spelling_table[right_var->identifier()];
      }
    }
    else if (right_lit != NULL) {
      if (right_lit->boolean_literal()) {
        if (right_lit->value() == 0) {
          right = "false";
        }
        else if (right_lit->value() == 1) {
          right = "true";
        }
      }
      else if (right_lit->number_literal()) {
        right = Administrator::IntToString(right_lit->value());
      }
    }
    else {
      right = "t" + Administrator::IntToString(current_temp_variable_++);
      node.right_expression()->Accept(this);
    }
    *output_ += CreateQuad(op, left, right, result);
  }
}

void ASTNodeCodeVisitor::Visit(BranchNode &node) {
  // Create all of the jumps for each case statement
  LiteralNode *lit = dynamic_cast<LiteralNode*>(node.expresion());
  VariableNode *var = dynamic_cast<VariableNode*>(node.expresion());
  std::string exp;
  if (lit != NULL) {
    if (lit->boolean_literal()) {
      if (lit->value() == 0) {
        exp = "false";
      }
      else if (lit->value() == 1) {
        exp = "true";
      }
    }
    else if (lit->number_literal()) {
      exp = Administrator::IntToString(lit->value());
    }
  }
  else if (var != NULL) {
    exp = Administrator::spelling_table[var->identifier()];
  }
  else {
    exp = "t" + Administrator::IntToString(current_temp_variable_++);
    node.expresion()->Accept(this);
  }
  std::string end_branch = "L" + Administrator::IntToString(current_label_++);
  std::string lab = "L" + Administrator::IntToString(current_label_++);
  std::string temp = "t" + Administrator::IntToString(current_temp_variable_++);
  std::string num;
  CaseNode *case_node = node.cases();
  CaseNode *default_case = NULL;
  while (case_node != NULL) {
    // Ignore the default case for now, handle it at the end
    if (case_node->default_case()) {
      default_case = case_node;      
    }
    else {
      num = Administrator::IntToString(case_node->case_number());
      *output_ += CreateQuad("eq", exp, num, temp);
      *output_ += CreateQuad("iff", temp, "-", lab);
      // Create quadruples for this case's contents
      case_node->statement()->Accept(this);
      *output_ += CreateQuad("goto", "-", "-", end_branch);
      *output_ += CreateQuad("lab", "-", "-", lab);
      lab = "L" + Administrator::IntToString(current_label_++);
    }
    case_node = dynamic_cast<CaseNode*>(case_node->next_node()); 
  }
  // If there is a default case create quadruples for it
  if (default_case != NULL) {
    default_case->statement()->Accept(this);
  }
  *output_ += CreateQuad("lab", "-", "-", end_branch);

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(CallNode &node) {
  // Check if this function call returns a value
  if (node.type() == Token::BOOL || node.type() == Token::INT) {
    std::string temp = "t" + Administrator::IntToString(current_temp_variable_++);
    *output_ += CreateQuad("rval", "-", "-", temp);
    // If there are arguments push them in reverse order
    if (node.arguments() != NULL) {
      PushArguments(node);
    }
    *output_ += CreateQuad("call", Administrator::spelling_table[node.identifier()], "-", "-");
  }
  // Otherwise it does not return a value and we do not need an RVAL quadruple.
  else {
    if (Administrator::spelling_table[node.identifier()].compare("writeint") == 0) {
      std::string value;
      VariableNode *var = dynamic_cast<VariableNode*>(node.arguments());
      LiteralNode *lit = dynamic_cast<LiteralNode*>(node.arguments());
      if (var != NULL) {
        value = Administrator::spelling_table[var->identifier()];
      }
      else if (lit != NULL) {
        value = Administrator::IntToString(lit->value());
      }
      // Otherwise this is an expression using temporaries
      else {
        value = "t" + Administrator::IntToString(current_temp_variable_++);
        node.arguments()->Accept(this);
      }
      *output_ += CreateQuad("wri", value, "-", "-");
    }
    else if (Administrator::spelling_table[node.identifier()].compare("writebool") == 0) {
      std::string value;
      VariableNode *var = dynamic_cast<VariableNode*>(node.arguments());
      LiteralNode *lit = dynamic_cast<LiteralNode*>(node.arguments());
      if (var != NULL) {
        value = Administrator::spelling_table[var->identifier()];
      }
      else if (lit != NULL) {
        value = Administrator::IntToString(lit->value());
      }
      // Otherwise this is an expression using temporaries
      else {
        value = "t" + Administrator::IntToString(current_temp_variable_++);
        node.arguments()->Accept(this);
      }
      *output_ += CreateQuad("wrb", value, "-", "-");
    }
    else {
      // If there are arguments push them in reverse order
      if (node.arguments() != NULL) {
        PushArguments(node);
      }
      *output_ += CreateQuad("call", Administrator::spelling_table[node.identifier()], "-", "-");
    }
  }

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::PushArguments(const CallNode &node) {
  int args = node.declaration()->num_parameters();
  for (int i = args; i > 0; --i) {
    ExpressionNode *arg = dynamic_cast<ExpressionNode*>(node.arguments());
    for (int j = 1; j < i; ++j) {
      arg = dynamic_cast<ExpressionNode*>(arg->next_node());
    }
    LiteralNode *lit = dynamic_cast<LiteralNode*>(arg);
    VariableNode *var = dynamic_cast<VariableNode*>(arg);
    std::string value;
    if (lit != NULL) {
      if (lit->boolean_literal()) {
        if (lit->value() == 0) {
          value = "false";
        }
        else if (lit->value() == 1) {
          value = "true";
        }
      }
    }
    else if (var != NULL) {
      value = Administrator::spelling_table[var->identifier()];
    }
    // Otherwise it is an expression that uses temporaries
    else {
      value = "t" + Administrator::IntToString(++current_temp_variable_);
      arg->Accept(this);
    }
    *output_ += CreateQuad("arg", value, "-", "-");
  }
}

void ASTNodeCodeVisitor::Visit(CompoundNode &node) {
  // If there are local variables then create an ECS quad with the number
  bool lcs = false;
  if (!function_compound_ && node.local_variables() > 0) {
    lcs = true;
    *output_ += CreateQuad("ecs", std::to_string(node.num_locals()), "-", "-");
  }

  function_compound_ = false;
  // Move onto the statements of this compound
  node.statements()->Accept(this);

  // If there was an ECS quad add the LCS quad.
  if (lcs) {
    *output_ += CreateQuad("lcs", "-", "-", "-");
  }
}

void ASTNodeCodeVisitor::Visit(ContinueNode &node) {
  // LS -> Label Start
  std::string label = "LS" + Administrator::IntToString(current_continue_jump_);
  *output_ += CreateQuad("goto", "-", "-", label);
}

void ASTNodeCodeVisitor::Visit(ExitNode &node) {
  // LE -> Label End
  std::string label = "LE" + Administrator::IntToString(current_exit_jump_);
  *output_ += CreateQuad("goto", "-", "-", label);
}

void ASTNodeCodeVisitor::Visit(FunctionDeclarationNode &node) {
  // Create the quad for the function header
  *output_ += "\n";
  *output_ += CreateQuad("fun", Administrator::spelling_table[node.identifier()], std::to_string(node.compound()->num_locals()), "-");
  // Move onto the body of the function
  function_compound_ = true;
  current_num_parameters_ = node.num_parameters();
  node.compound()->Accept(this);
  current_num_parameters_ = -1;
  // Move onto the next declaration
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(IfNode &node) {
  VariableNode *var = dynamic_cast<VariableNode*>(node.expression());
  LiteralNode *lit = dynamic_cast<LiteralNode*>(node.expression());
  std::string else_label;
  std::string end_label;
  if (node.else_statement() != NULL) {
    else_label = "L" + Administrator::IntToString(current_label_++);
    end_label = "L" + Administrator::IntToString(current_label_++);
  }
  else {
    end_label = "L" + Administrator::IntToString(current_label_++);
  }
  // Check if the expression is just a variable
  if (var != NULL) {
    // If there is an else statment jump to it if not true
    if (node.else_statement() != NULL) {
      *output_ += CreateQuad("iff", Administrator::spelling_table[var->identifier()], "-", else_label);
    }
    else {
      *output_ += CreateQuad("iff", Administrator::spelling_table[var->identifier()], "-", end_label);
    }
  }
  else if (lit != NULL) {
    if (lit->boolean_literal()) {
      std::string value;
      if (lit->value() == 0) {
        value = "false";
      }
      else if (lit->value() == 1) {
        value = "true";
      }
      // If there is an else statment jump to it if not true
      if (node.else_statement() != NULL) {
        *output_ += CreateQuad("iff", value, "-", else_label);
      }
      else {
        *output_ += CreateQuad("iff", value, "-", end_label);
      }
    }
    else {
      printf("Something Went Wrong :(\n");
    }
  }
  // Otherwise the if expression is a longer expression involving temporary variables.
  else {

  }

  node.then_statement()->Accept(this);
  if (node.else_statement()) {
    *output_ += CreateQuad("goto", "-", "-", end_label);
    *output_ += CreateQuad("lab", "-", "-", else_label);
    node.else_statement()->Accept(this);
  }
  *output_ += CreateQuad("lab", "-", "-", end_label);
  
}

void ASTNodeCodeVisitor::Visit(LoopNode &node) {
  // Create the label to jump back to the start of the loop
  std::string label = "LS" + Administrator::IntToString(current_continue_jump_);
  *output_ += CreateQuad("lab", "-", "-", label);

  node.statements()->Accept(this);
  ++current_continue_jump_;

  label = "LE" + Administrator::IntToString(current_exit_jump_++);
  *output_ += CreateQuad("lab", "-", "-", label);
}

void ASTNodeCodeVisitor::Visit(ProgramNode &node) {
  // Start with the first declaration of the program
  node.declaration_node()->Accept(this);
}

void ASTNodeCodeVisitor::Visit(ReturnNode &node) {
  // This returns a value
  if (node.expression() != NULL) {
    // TODO: Figure out return value temporaries
    LiteralNode* literal = dynamic_cast<LiteralNode*>(node.expression());
    VariableNode* variable = dynamic_cast<VariableNode*>(node.expression());
    std::string parameters = Administrator::IntToString(current_num_parameters_);
    if (literal != NULL) {
      *output_ += CreateQuad("retv", parameters, Administrator::IntToString(literal->value()), "-");
    }
    else if (variable != NULL) {
      *output_ += CreateQuad("retv", parameters, Administrator::spelling_table[variable->identifier()], "-");
    }
    // Otherwise it is a binary, unary or call node, in which case a temporary 
    // variable has been used
    else {
      // Visit the expression then create its quadruples
      node.expression()->Accept(this);
      std::string temp = "t" + Administrator::IntToString(current_temp_variable_++);
      *output_ += CreateQuad("retv", parameters, temp, "-");
    }
  }
  // Or not
  else {
    *output_ += CreateQuad("ret", Administrator::IntToString(current_num_parameters_), "-", "-");
  }

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}


void ASTNodeCodeVisitor::Visit(UnaryNode &node) {
  LiteralNode *lit = dynamic_cast<LiteralNode*>(node.expression());
  VariableNode *var = dynamic_cast<VariableNode*>(node.expression());
  // TODO
  std::string temp = "t" + Administrator::IntToString(current_temp_variable_++);
  std::string value;

  std::string op;
  switch (node.op()) {
  case Token::MINUS:
    op = "uminus";
    break;
  case Token::NOT:
    op = "not";
    break;
  }

  if (lit != NULL) {
    if (lit->boolean_literal()) {
      if (lit->value() == 0) {
        value = "false";
      }
      else if (lit->value() == 1) {
        value = "true";
      }
    }
    else if (lit->number_literal()) {
      value = Administrator::IntToString(lit->value());
    }
  }
  else if (var != NULL) {
    value = Administrator::spelling_table[var->identifier()];
  }
  // Otherwise it is a longer expression with temporaries
  else {
    value = "t" + Administrator::IntToString(current_temp_variable_);
    node.expression()->Accept(this);
  }
  *output_ += CreateQuad(op, value, "-", temp);
}

void ASTNodeCodeVisitor::Visit(VariableDeclarationNode &node) {
  // Variable declarations do not produce quadruples
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(ASTNode &node) { }
void ASTNodeCodeVisitor::Visit(CaseNode &node) { }
void ASTNodeCodeVisitor::Visit(DeclarationNode &node) { }
void ASTNodeCodeVisitor::Visit(ExpressionNode &node) { }
void ASTNodeCodeVisitor::Visit(LiteralNode &node) { }
void ASTNodeCodeVisitor::Visit(NullNode &node) { }
void ASTNodeCodeVisitor::Visit(ParameterNode &node) { }
void ASTNodeCodeVisitor::Visit(StatementNode &node) { }
void ASTNodeCodeVisitor::Visit(VariableNode &node) { }

std::string ASTNodeCodeVisitor::CreateQuad(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result) {
  std::string quad = "(";
  quad += op;
  quad += ",";
  quad += arg1;
  quad += ",";
  quad += arg2;
  quad += ",";
  quad += result;
  quad += ")\n";
  return quad;
}