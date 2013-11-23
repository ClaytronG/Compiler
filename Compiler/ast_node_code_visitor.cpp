#include "ast_node_code_visitor.h"

ASTNodeCodeVisitor::ASTNodeCodeVisitor(std::string &output) : output_(&output) {
  function_compound_ = false;
  current_num_parameters_ = -1;
  current_temp_variable_ = 1;
  last_variable_ = -1;
  current_continue_jump_ = 1;
  current_exit_jump_ = 1;
};

void ASTNodeCodeVisitor::Visit(AssignmentNode &node) {

}

void ASTNodeCodeVisitor::Visit(BinaryNode &node) {

}

void ASTNodeCodeVisitor::Visit(BranchNode &node) {

}

void ASTNodeCodeVisitor::Visit(CallNode &node) {

}

void ASTNodeCodeVisitor::Visit(CaseNode &node) {

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
  std::string label = "LS" + Administrator::IntToString(current_continue_jump_);
  *output_ += CreateQuad("goto", "-", "-", label);
}

void ASTNodeCodeVisitor::Visit(ExitNode &node) {
  std::string label = "LE" + Administrator::IntToString(current_exit_jump_);
  *output_ += CreateQuad("goto", "-", "-", label);
}

void ASTNodeCodeVisitor::Visit(FunctionDeclarationNode &node) {
  // Create the quad for the function header
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

}

void ASTNodeCodeVisitor::Visit(LiteralNode &node) { 
  last_literal_ = node;
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
      // Visit the expression the create its quadruples
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
  // Create the quadruples for the expression first
  node.expression()->Accept(this);
  // Get the current temp variable in string form
  std::string current_temp = "t" + Administrator::IntToString(current_temp_variable_);
  // Get the next temp variable in string form
  std::string next_temp = "t" + Administrator::IntToString(++current_temp_variable_);
  switch (node.op()) {
  case Token::NOT:
    *output_ += CreateQuad("not", current_temp, "-", next_temp);
    break;
  case Token::MINUS:
    *output_ += CreateQuad("uminus", current_temp, "-", next_temp);
    break;
  default:
    printf("%s\n", "Something went wrong");
    break;
  }
}

void ASTNodeCodeVisitor::Visit(ASTNode &node) { }
void ASTNodeCodeVisitor::Visit(DeclarationNode &node) { }
void ASTNodeCodeVisitor::Visit(NullNode &node) { }
void ASTNodeCodeVisitor::Visit(ParameterNode &node) { }
void ASTNodeCodeVisitor::Visit(StatementNode &node) { }
void ASTNodeCodeVisitor::Visit(ExpressionNode &node) { }

void ASTNodeCodeVisitor::Visit(VariableDeclarationNode &node) {
  // Variable declarations do not produce quadruples
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(VariableNode &node) {
  last_variable_ = node;
}

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