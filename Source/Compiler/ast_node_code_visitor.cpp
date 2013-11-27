// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_code_visitor.h"

ASTNodeCodeVisitor::ASTNodeCodeVisitor(std::string &output) : output_(&output) {
  function_compound_ = false;
  current_num_parameters_ = -1;
  current_temp_variable_ = 2;
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
  std::string op = "asg", arg1 = "-", arg2 = "-", 
    result = Administrator::spelling_table[node.identifier()];

  // If the assignment is a call...
  if (call != NULL) {
    // check if it is a system call (readint/readbool)
    if (Administrator::spelling_table[call->identifier()].compare("readint") == 0) {
      op = "rdi";
    }
    else if (Administrator::spelling_table[call->identifier()].compare("readbool") == 0) {
      op = "rdb";
    }
    // Otherwise it is not a call to the compiler functions
    else {
      // Create the quadruples for this call
      arg1 = "t" + Administrator::IntToString(current_temp_variable_);
      call->Accept(this);
    }
  }
  else if (lit != NULL) {
    arg1 = GetLiteral(lit);
  }
  else if (var != NULL) {
    arg1 = GetVariable(var);
  }
  else {
    // Create the quadruples for the assignment
    node.value()->Accept(this);
    arg1 = "t" + Administrator::IntToString(current_temp_variable_++);
  }

  if (node.array_assignment()) {
    op = "tae";
    call = dynamic_cast<CallNode*>(node.array_expression());
    lit = dynamic_cast<LiteralNode*>(node.array_expression());
    var = dynamic_cast<VariableNode*>(node.array_expression());

    if (call != NULL) {
      arg2 = "t" + Administrator::IntToString(current_temp_variable_);
      call->Accept(this);
    }
    else if (lit != NULL) {
      arg2 = GetLiteral(lit);
    }
    else if (var != NULL) {
      arg2 = GetVariable(var);
    }
    else {
      node.array_expression()->Accept(this);
      arg2 = "t" + Administrator::IntToString(current_temp_variable_++);
    }
  }

  *output_ += CreateQuad(op, arg1, arg2, result);

  // Clean up the pointers
  //delete call;
  //delete lit;
  //delete var;

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(BinaryNode &node) {
  LiteralNode *left_lit = dynamic_cast<LiteralNode*>(node.left_expression());
  LiteralNode *right_lit = dynamic_cast<LiteralNode*>(node.right_expression());
  VariableNode *left_var = dynamic_cast<VariableNode*>(node.left_expression());
  VariableNode *right_var = dynamic_cast<VariableNode*>(node.right_expression());
  CallNode *left_call = dynamic_cast<CallNode*>(node.left_expression());
  CallNode *right_call = dynamic_cast<CallNode*>(node.right_expression());
  std::string op, left, right, result;

  // Have to do special short circuit things with ANDTHEN
  if (node.op() == Token::ANDTHEN) {

  }
  // and ORELSE
  else if (node.op() == Token::ORELSE) {

  }
  else {
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

    // Check the right expression
    if (right_var != NULL) {
      right = GetVariable(right_var);
    }
    else if (right_lit != NULL) {
      right = GetLiteral(right_lit);
    }
    else if (right_call != NULL) {
      right = "t" + Administrator::IntToString(current_temp_variable_);
      right_call->Accept(this);
    }
    else {
      node.right_expression()->Accept(this);
      right = "t" + Administrator::IntToString(current_temp_variable_++);
    }

    // Check the left expression
    if (left_var != NULL) {
      left = GetVariable(left_var);
    }
    else if (left_lit != NULL) {
      left = GetLiteral(left_lit);
    }
    else if (left_call != NULL) {
      left = "t" + Administrator::IntToString(current_temp_variable_);
      left_call->Accept(this);
    }
    else {
      node.left_expression()->Accept(this);
      left = "t" + Administrator::IntToString(current_temp_variable_++);
    }
  }

  result = "t" + Administrator::IntToString(current_temp_variable_);

  *output_ += CreateQuad(op, left, right, result);

  //delete left_lit;
  //delete right_lit;
  //delete left_var;
  //delete right_var;

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(BranchNode &node) {
  // Create all of the jumps for each case statement
  LiteralNode *lit = dynamic_cast<LiteralNode*>(node.expresion());
  VariableNode *var = dynamic_cast<VariableNode*>(node.expresion());
  CallNode *call = dynamic_cast<CallNode*>(node.expresion());
  std::string exp;

  // Get the add-exp in the branch
  if (lit != NULL) {
    exp = GetLiteral(lit);
  }
  else if (var != NULL) {
    exp = GetVariable(var);
  }
  else if (call != NULL) {
    exp = "t" + Administrator::IntToString(current_temp_variable_);
    call->Accept(this);
  }
  else {
    node.expresion()->Accept(this);
    exp = "t" + Administrator::IntToString(current_temp_variable_++);
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

  //delete lit;
  //delete var;
  //delete case_node;
  //delete default_case;

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(CallNode &node) {
  std::string op = "call", arg1 = Administrator::spelling_table[node.identifier()];

  // Check if this function call returns a value
  if (node.type() == Token::BOOL || node.type() == Token::INT) {
    std::string temp = "t" + Administrator::IntToString(current_temp_variable_++);
    *output_ += CreateQuad("rval", "-", "-", temp);
    // If there are arguments push them in reverse order
    if (node.arguments() != NULL) {
      PushArguments(node);
    }
  }
  // Otherwise it does not return a value and we do not need an RVAL quadruple.
  else {
    if (Administrator::spelling_table[node.identifier()].compare("writeint") == 0) {
      VariableNode *var = dynamic_cast<VariableNode*>(node.arguments());
      LiteralNode *lit = dynamic_cast<LiteralNode*>(node.arguments());
      CallNode *call = dynamic_cast<CallNode*>(node.arguments());

      if (var != NULL) {
        arg1 = GetVariable(var);
      }
      else if (lit != NULL) {
        arg1 = GetLiteral(lit);
      }
      else if (call != NULL) {
        arg1 = "t" + Administrator::IntToString(current_temp_variable_);
        call->Accept(this);
      }
      // Otherwise this is an expression using temporaries
      else {
        node.arguments()->Accept(this);
        arg1 = "t" + Administrator::IntToString(current_temp_variable_++);
      }

      op = "wri";

      //delete var;
      //delete lit;
    }
    else if (Administrator::spelling_table[node.identifier()].compare("writebool") == 0) {
      VariableNode *var = dynamic_cast<VariableNode*>(node.arguments());
      LiteralNode *lit = dynamic_cast<LiteralNode*>(node.arguments());
      CallNode *call = dynamic_cast<CallNode*>(node.arguments());

      if (var != NULL) {
        arg1 = GetVariable(var);
      }
      else if (lit != NULL) {
        arg1 = GetLiteral(lit);
      }
      else if (call != NULL) {
        arg1 = "t" + Administrator::IntToString(current_temp_variable_);
        call->Accept(this);
      }
      // Otherwise this is an expression using temporaries
      else {
        node.arguments()->Accept(this);
        arg1 = "t" + Administrator::IntToString(current_temp_variable_++);
      }

      op = "wrb";

      //delete var;
      //delete lit;
    }
    else {
      // If there are arguments push them in reverse order
      if (node.arguments() != NULL) {
        PushArguments(node);
      }
    }
  }

  *output_ += CreateQuad(op, arg1, "-", "-");

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::PushArguments(const CallNode &node) {
  int args = node.declaration()->num_parameters();
  std::string op;
  for (int i = args; i > 0; --i) {
    ExpressionNode *arg = dynamic_cast<ExpressionNode*>(node.arguments());
    ParameterNode *param = dynamic_cast<ParameterNode*>(node.declaration()->parameters());
    for (int j = 1; j < i; ++j) {
      arg = dynamic_cast<ExpressionNode*>(arg->next_node());
      param = dynamic_cast<ParameterNode*>(param->next_parameter());
    }
    if (param->array_parameter() || param->reference_parameter()) {
      op = "arga";
    }
    else {
      op = "arg";
    }
    LiteralNode *lit = dynamic_cast<LiteralNode*>(arg);
    VariableNode *var = dynamic_cast<VariableNode*>(arg);
    CallNode *call = dynamic_cast<CallNode*>(arg);
    std::string value;
    if (lit != NULL) {
      value = GetLiteral(lit);
    }
    else if (var != NULL) {
      value = GetVariable(var);
    }
    else if (call != NULL) {
      value = "t" + Administrator::IntToString(current_temp_variable_);
      call->Accept(this);
    }
    // Otherwise it is an expression that uses temporaries
    else {
      arg->Accept(this);
      value = "t" + Administrator::IntToString(current_temp_variable_++);
    }
    *output_ += CreateQuad(op, value, "-", "-");
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

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(ContinueNode &node) {
  // LS -> Label Start
  std::string label = "LS" + Administrator::IntToString(current_continue_jump_);
  *output_ += CreateQuad("goto", "-", "-", label);

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(ExitNode &node) {
  // LE -> Label End
  std::string label = "LE" + Administrator::IntToString(current_exit_jump_);
  *output_ += CreateQuad("goto", "-", "-", label);

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
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
  CallNode *call = dynamic_cast<CallNode*>(node.expression());
  std::string else_label, end_label;

  if (node.else_statement() != NULL) {
    else_label = "L" + Administrator::IntToString(current_label_++);
    end_label = "L" + Administrator::IntToString(current_label_++);
  }
  else {
    end_label = "L" + Administrator::IntToString(current_label_++);
  }
  std::string value;
  // Check if the expression is just a variable
  if (var != NULL) {
    value = GetVariable(var);
  }
  else if (lit != NULL) {
    value = GetLiteral(lit);
  }
  else if (call != NULL) {
    value = "t" + Administrator::IntToString(current_temp_variable_);
    call->Accept(this);
  }
  else {
    node.expression()->Accept(this);
    value = "t" + Administrator::IntToString(current_temp_variable_++);
  }
  // If there is an else statment jump to it if not true
  if (node.else_statement() != NULL) {
    *output_ += CreateQuad("iff", value, "-", else_label);
  }
  else {
    *output_ += CreateQuad("iff", value, "-", end_label);
  }

  node.then_statement()->Accept(this);
  if (node.else_statement()) {
    *output_ += CreateQuad("goto", "-", "-", end_label);
    *output_ += CreateQuad("lab", "-", "-", else_label);
    node.else_statement()->Accept(this);
  }
  *output_ += CreateQuad("lab", "-", "-", end_label);
  
  //delete lit;
  //delete var;

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(LoopNode &node) {
  // Create the label to jump back to the start of the loop
  std::string label = "LS" + Administrator::IntToString(current_continue_jump_);
  *output_ += CreateQuad("lab", "-", "-", label);

  node.statements()->Accept(this);
  ++current_continue_jump_;

  *output_ += CreateQuad("goto", "-", "-", label);

  label = "LE" + Administrator::IntToString(current_exit_jump_++);
  *output_ += CreateQuad("lab", "-", "-", label);

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(ProgramNode &node) {
  // Start with the first declaration of the program
  node.declaration_node()->Accept(this);
}

void ASTNodeCodeVisitor::Visit(ReturnNode &node) {
  std::string op, arg1 = "-", arg2 = "-";

  // Check to see if this returns a value
  if (node.expression() != NULL) {
    LiteralNode *lit = dynamic_cast<LiteralNode*>(node.expression());
    VariableNode *var = dynamic_cast<VariableNode*>(node.expression());
    arg1 = Administrator::IntToString(current_num_parameters_);
    if (lit != NULL) {
      arg2 = GetLiteral(lit);
    }
    else if (var!= NULL) {
      arg2 = GetVariable(var);
    }
    // Otherwise it is a binary, unary or call node, in which case a temporary 
    // variable has been used
    else {
      // Visit the expression then create its quadruples
      node.expression()->Accept(this);
      arg2 = "t" + Administrator::IntToString(current_temp_variable_++);
    }

    op = "retv";

    //delete lit;
    //delete var;
  }
  // Or not
  else {
    op = "ret";
    arg1 = Administrator::IntToString(current_num_parameters_);
  }

  *output_ += CreateQuad(op, arg1, arg2, "-");

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}


void ASTNodeCodeVisitor::Visit(UnaryNode &node) {
  LiteralNode *lit = dynamic_cast<LiteralNode*>(node.expression());
  VariableNode *var = dynamic_cast<VariableNode*>(node.expression());
  CallNode *call = dynamic_cast<CallNode*>(node.expression());
  std::string temp = "t" + Administrator::IntToString(current_temp_variable_);
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
    value = GetLiteral(lit);
  }
  else if (var != NULL) {
    value = GetVariable(var);
  }
  else if (call != NULL) {
    value = "t" + Administrator::IntToString(current_temp_variable_);
    call->Accept(this);
  }
  // Otherwise it is a longer expression with temporaries
  else {
    node.expression()->Accept(this);
    value = "t" + Administrator::IntToString(current_temp_variable_++);
  }

  *output_ += CreateQuad(op, value, "-", temp);

  //delete lit;
  //delete var;

  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(VariableDeclarationNode &node) {
  // Variable declarations do not produce quadruples
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(NullNode &node) { 
  // Null nodes do not produce quadruples
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(ASTNode &node) { }
void ASTNodeCodeVisitor::Visit(CaseNode &node) { }
void ASTNodeCodeVisitor::Visit(DeclarationNode &node) { }
void ASTNodeCodeVisitor::Visit(ExpressionNode &node) { }
void ASTNodeCodeVisitor::Visit(LiteralNode &node) { }
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

std::string ASTNodeCodeVisitor::GetLiteral(const LiteralNode *node) {
  std::string value;

  if (node->boolean_literal()) {
    if (node->value() == 0) {
      value = "false";
    }
    else if (node->value() == 1) {
      value = "true";
    }
  }
  else if (node->number_literal()) {
    value = Administrator::IntToString(node->value());
  }

  return value;
}

std::string ASTNodeCodeVisitor::GetVariable(const VariableNode *node) {
  std::string value;

  // Have to do more work for an array variable
  if (node->array_variable()) {
    LiteralNode *lit = dynamic_cast<LiteralNode*>(node->array_expression());
    VariableNode *var = dynamic_cast<VariableNode*>(node->array_expression());
    CallNode *call = dynamic_cast<CallNode*>(node->array_expression());

    std::string arg1 = Administrator::spelling_table[node->identifier()];

    if (lit != NULL) {
      std::string arg2 = GetLiteral(lit);
      value = "t" + Administrator::IntToString(current_temp_variable_++);
      *output_ += CreateQuad("fae", arg1, arg2, value);
    }
    else if (var != NULL) {
      value = GetVariable(var);
      std::string temp = "t" + Administrator::IntToString(current_temp_variable_++);
      *output_ += CreateQuad("asg", value, "-", temp);
      value = "t" + Administrator::IntToString(current_temp_variable_++);
      *output_ += CreateQuad("fae", arg1, temp, value);
    }
    else if (call != NULL) {
      value = "t" + Administrator::IntToString(current_temp_variable_);
      call->Accept(this);
    }

    //delete lit;
    //delete var;
    //delete call;
  }
  else {
    value = Administrator::spelling_table[node->identifier()];
  }

  return value;
}