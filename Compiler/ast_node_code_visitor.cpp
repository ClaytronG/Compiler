#include "ast_node_code_visitor.h"

ASTNodeCodeVisitor::ASTNodeCodeVisitor(std::string &output) : output_(&output) { 
  global_scope_ = true;
  function_compound_ = false;
};

void ASTNodeCodeVisitor::Visit(AssignmentNode &node) {

}

void ASTNodeCodeVisitor::Visit(ASTNode &node) {

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

}

void ASTNodeCodeVisitor::Visit(DeclarationNode &node) {

}

void ASTNodeCodeVisitor::Visit(ExitNode &node) {

}

void ASTNodeCodeVisitor::Visit(ExpressionNode &node) {

}

void ASTNodeCodeVisitor::Visit(FunctionDeclarationNode &node) {
  // Create the quad for the function header
  *output_ += CreateQuad("fun", Administrator::spelling_table[node.identifier()], std::to_string(node.compound()->num_locals()), "-");
  // Move onto the body of the function
  function_compound_ = true;
  node.compound()->Accept(this);
  // Move onto the next declaration
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(IfNode &node) {

}

void ASTNodeCodeVisitor::Visit(LiteralNode &node) {

}

void ASTNodeCodeVisitor::Visit(LoopNode &node) {

}

void ASTNodeCodeVisitor::Visit(NullNode &node) { }

void ASTNodeCodeVisitor::Visit(ParameterNode &node) { }

void ASTNodeCodeVisitor::Visit(ProgramNode &node) {
  // Move onto the declaration nodes
  node.declaration_node()->Accept(this);
  // Move onto the next declaration
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(ReturnNode &node) {
  node.expression()->Accept(this);
  // This returns a value
  if (node.expression() != NULL) {

  }
  // Or not
  else {

  }
}

void ASTNodeCodeVisitor::Visit(StatementNode &node) {

}

void ASTNodeCodeVisitor::Visit(UnaryNode &node) {

}

void ASTNodeCodeVisitor::Visit(VariableDeclarationNode &node) {
  // Variable declarations do not produce quadruples
  if (node.next_node() != NULL) {
    node.next_node()->Accept(this);
  }
}

void ASTNodeCodeVisitor::Visit(VariableNode &node) {

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