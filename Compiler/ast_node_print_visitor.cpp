// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#include "ast_node_print_visitor.h"

ASTNodePrintVisitor::ASTNodePrintVisitor(Messenger *messenger) {
  depth_ = 0;
  messenger_ = messenger;
}

// Prints an assignment node and its children. Example:
//   Assignment
//     id: x
//     array expression:
//       ...
//     equals:
//       ...
void ASTNodePrintVisitor::Visit(const AssignmentNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "Assignment\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "id: " + Administrator::spelling_table[node.identifier()] + "\n";
  message += std::string(depth_*2, ' ');
  if (node.array_assignment()) {
    message += "array expression:\n";
    messenger_->PrintMessage(message);
    ++depth_;
    node.array_expression()->Accept(this);
    --depth_;
    message = std::string(depth_*2, ' ');
  }
  message += "equals:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.value()->Accept(this);
  depth_ = depth_ - 2;
}

// Prints a binary node and its children. Example:
//   Binary Node
//     op: PLUS
//     left:
//       ...
//     right:
//       ...
//   ...
void ASTNodePrintVisitor::Visit(const BinaryNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "Binary Node\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "op: ";
  message += Token::kTokenStrings[node.op()] + "\n";
  message += std::string(depth_*2, ' ');
  message += "left:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.left_expression()->Accept(this);
  --depth_;
  message = std::string(depth_*2, ' ');
  message += "right:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.right_expression()->Accept(this);
  depth_ = depth_ - 2;
}

// Prints a branch node and its children. Example:
//   Branch
//     expression:
//       ...
//     cases:
//       ...
void ASTNodePrintVisitor::Visit(const BranchNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "Branch\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "expression:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.expresion()->Accept(this);
  --depth_;
  message = std::string(depth_*2, ' ');
  message += "cases:\n";
  messenger_->PrintMessage(message);
  CaseNode *current_case = node.cases();
  while (current_case != NULL) {
    ++depth_;
    current_case->Accept(this);
    --depth_;
    current_case = dynamic_cast<CaseNode*>(
      current_case->next_node());
  }
  --depth_;
}

// Prints a call node and its children. Example:
//   Call
//     id: add
//     arguments:
//       ...
void ASTNodePrintVisitor::Visit(const CallNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "Call\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "id: ";
  message += Administrator::spelling_table[node.identifier()] + "\n";
  message += std::string(depth_*2, ' ');
  message += "arguments:\n";
  messenger_->PrintMessage(message);
  ExpressionNode *current_node = node.arguments();
  while (current_node != NULL) {
    ++depth_;
    current_node->Accept(this);
    --depth_;
    current_node = dynamic_cast<ExpressionNode*>(
      current_node->next_node());
  }
  --depth_;
}

// Prints a case node and its children. Example:
//   Case
//     number: 1
//     statement:
//       ...
void ASTNodePrintVisitor::Visit(const CaseNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "Case\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "number: ";
  if (node.default_case()) {
    message += "default\n";
  } else {
    message += Administrator::IntToString(node.case_number());
    message += "\n";
  }
  message += std::string(depth_*2, ' ');
  message += "statement:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.statement()->Accept(this);
  depth_ = depth_ - 2;
}

// Prints compound node and its children. Example:
//   Compound
//     variables:
//       ...
//     statements:
//       ...
void ASTNodePrintVisitor::Visit(const CompoundNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Compound\n";
  ++depth_;
  message += std::string(depth_ * 2, ' ');
  message += "variables:\n";
  messenger_->PrintMessage(message);
  VariableDeclarationNode *current_var = node.local_variables();
  while (current_var != NULL) {
    ++depth_;
    current_var->Accept(this);
    --depth_;
    current_var = dynamic_cast<VariableDeclarationNode*>(
      current_var->next_node());
  }  
  message = std::string(depth_*2, ' ');
  message += "statements:\n";
  messenger_->PrintMessage(message);
  StatementNode *current_node = node.statements();
  while (current_node != NULL) {
    ++depth_;
    current_node->Accept(this);
    --depth_;
    current_node = dynamic_cast<StatementNode*>(
      current_node->next_node());
  }
  --depth_;
}

// Prints a continue node. Exmaple:
//   Continue
void ASTNodePrintVisitor::Visit(const ContinueNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Continue\n";
  messenger_->PrintMessage(message);
}

// Prints a declaration node. Because there is no base declaration, this method
// tries casting to subclasses and calls their accept.
void ASTNodePrintVisitor::Visit(const DeclarationNode &node) {
  const FunctionDeclarationNode *func = 
    dynamic_cast<const FunctionDeclarationNode*>(&node);
  const VariableDeclarationNode *variable =
    dynamic_cast<const VariableDeclarationNode*>(&node);

  if (func) {
    delete variable;
    func->Accept(this);
  } else if (variable) {
    delete func;
    variable->Accept(this);
  }
}

// Prints an exit node. Example:
//   Exit
void ASTNodePrintVisitor::Visit(const ExitNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "EXIT\n";
  messenger_->PrintMessage(message);
}

// Prints an expression node. Because there is no base expression, this method
// tries casting to subclasses and calls their Accept().
void ASTNodePrintVisitor::Visit(const ExpressionNode &node) {
  const BinaryNode *binary = dynamic_cast<const BinaryNode*>(&node);
  const UnaryNode *unary = dynamic_cast<const UnaryNode*>(&node);
  const LiteralNode *literal = dynamic_cast<const LiteralNode*>(&node);
  const VariableNode *variable = dynamic_cast<const VariableNode*>(&node);

  if (binary) {
    delete unary;
    delete literal;
    delete variable;
    binary->Accept(this);
  } else if (unary) {
    delete binary;
    delete literal;
    delete variable;
    unary->Accept(this);
  } else if (literal) {
    delete binary;
    delete unary;
    delete variable;
    literal->Accept(this);
  } else if (variable) {
    delete binary;
    delete unary;
    delete literal;
    variable->Accept(this);
  }
}

// Prints function node and its children. Example:
//   Function Declaration
//     id: add
//     return: int
//     parameters:
//       ...
//     compound:
//       ...
void ASTNodePrintVisitor::Visit(const FunctionDeclarationNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Function Declaration\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "id: " + Administrator::spelling_table[node.identifier()] + "\n";
  message += std::string(depth_*2, ' ');
  message += "return: " + Token::kTokenStrings[node.type()] + "\n";
  if (node.parameters()) { 
    message += std::string(depth_*2, ' ');
    message += "parameters:\n";
    messenger_->PrintMessage(message);
    ++depth_;
    node.parameters()->Accept(this);
    --depth_;
  } else {
    messenger_->PrintMessage(message);
  }
  message = std::string(depth_*2, ' ');
  message += "compound:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.compound()->Accept(this);
  depth_ = depth_ - 2;
}

// Prints an if node and its children. Example:
//   If Node
//     expression:
//       ...
//     then statement:
//       ...
//     else statement:
//       ...
void ASTNodePrintVisitor::Visit(const IfNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "If Node\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "expression:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.expression()->Accept(this);
  --depth_;
  message = std::string(depth_*2, ' ');
  message += "then statement:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.then_statement()->Accept(this);
  --depth_;
  if (node.else_statement() != NULL) {
    message = std::string(depth_*2, ' ');
    message += "else statement:\n";
    messenger_->PrintMessage(message);
    ++depth_;
    node.else_statement()->Accept(this);
    --depth_;
  }
  --depth_;
}

// Prints a boolean or number literal. Example:
//   false
// OR
//   123
void ASTNodePrintVisitor::Visit(const LiteralNode &node) {
  std::string message = std::string(depth_*2, ' ');
  if (node.boolean_literal()) {
    message += Token::kBlitStrings[node.value()] + "\n";
  } else if (node.number_literal()) {
    message += Administrator::IntToString(node.value()) + "\n";
  }
  messenger_->PrintMessage(message);
}

// Prints a loop node and its children. Example:
//   Loop
//     statement:
//       ...
void ASTNodePrintVisitor::Visit(const LoopNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Loop\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "statement:\n";
  messenger_->PrintMessage(message);
  StatementNode *current_node = node.statements();
  ++depth_;
  while (current_node != NULL) {
    current_node->Accept(this);
    current_node = dynamic_cast<StatementNode*>(current_node->next_node());
  }
  depth_ = depth_ - 2;
}

// Prints a null node. Example:
//   Null
void ASTNodePrintVisitor::Visit(const NullNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "NULL\n";
  messenger_->PrintMessage(message);
}

// Prints a paramter node and its children. Example:
//   Param
//     id: x
//     type: int
//   Param
//     id: y
//     type: ref int
//   Param
//     id: done
//     type: bool[]
//   ...
void ASTNodePrintVisitor::Visit(const ParameterNode &node){
  const ParameterNode *param = &node;
  while (param != NULL) {
    std::string message = std::string(depth_*2, ' ');
    message += "Param\n";
    ++depth_;
    message += std::string(depth_*2, ' ');
    message += "id: " + Administrator::spelling_table[param->identifier()];
    message += "\n" + std::string(depth_*2, ' ') + "type: ";
    if (param->reference_parameter()) {
      message += "ref ";
    }
    message += Token::kTokenStrings[param->type()];
    if (param->array_parameter()) {
      message += "[]";
    }
    message += "\n";
    messenger_->PrintMessage(message);
    param = param->next_parameter();
    --depth_;
  }
}

// Prints a program node and its children. Example:
//   Program
//     ...
void ASTNodePrintVisitor::Visit(const ProgramNode &node){ 
  messenger_->PrintMessage("Program\n");
  DeclarationNode *current_node = node.declaration_node();
  ++depth_;
  while (current_node != NULL) {
    current_node->Accept(this);
    current_node = dynamic_cast<DeclarationNode*>(current_node->next_node());
  }
  --depth_;
}

// Prints a return node and its children. Example:
//   Return
//     expression:
//       ...
void ASTNodePrintVisitor::Visit(const ReturnNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Return\n";
  if (node.expression() != NULL) {
    ++depth_;
    message += std::string(depth_*2, ' ');
    message += "expression:\n";
    messenger_->PrintMessage(message);
    ++depth_;
    node.expression()->Accept(this);
    depth_ = depth_ - 2;
  } else {
    messenger_->PrintMessage(message);
  }
}

// Prints a statement node. Because there is no base statement, this method
// tries casting to subclasses and calls their Accept().
void ASTNodePrintVisitor::Visit(const StatementNode &node) {
  const AssignmentNode *assign = dynamic_cast<const AssignmentNode*>(&node);
  const IfNode *if_statement = dynamic_cast<const IfNode*>(&node);
  const LoopNode *loop = dynamic_cast<const LoopNode*>(&node);
  const ReturnNode *return_statement = dynamic_cast<const ReturnNode*>(&node);
  const BranchNode *branch = dynamic_cast<const BranchNode*>(&node);
  const CaseNode *case_statement = dynamic_cast<const CaseNode*>(&node);
  const ExitNode *exit = dynamic_cast<const ExitNode*>(&node);
  const ContinueNode *continue_statement = 
    dynamic_cast<const ContinueNode*>(&node);
  const NullNode *null_statement = dynamic_cast<const NullNode*>(&node);

  if (assign) {
    delete if_statement;
    delete loop;
    delete return_statement;
    delete branch;
    delete case_statement;
    delete exit;
    delete continue_statement;
    delete null_statement;
    assign->Accept(this);
  } else if (if_statement) {
    delete assign;
    delete loop;
    delete return_statement;
    delete branch;
    delete case_statement;
    delete exit;
    delete continue_statement;
    delete null_statement;
    if_statement->Accept(this);
  } else if (loop) {
    delete assign;
    delete if_statement;
    delete return_statement;
    delete branch;
    delete case_statement;
    delete exit;
    delete continue_statement;
    delete null_statement;
    loop->Accept(this);
  } else if (return_statement) {
    delete assign;
    delete if_statement;
    delete loop;
    delete branch;
    delete case_statement;
    delete exit;
    delete continue_statement;
    delete null_statement;
    return_statement->Accept(this);
  } else if (branch) {
    delete assign;
    delete if_statement;
    delete loop;
    delete return_statement;
    delete case_statement;
    delete exit;
    delete continue_statement;
    delete null_statement;
    branch->Accept(this);
  } else if (case_statement) {
    delete assign;
    delete if_statement;
    delete loop;
    delete return_statement;
    delete branch;
    delete exit;
    delete continue_statement;
    delete null_statement;
    case_statement->Accept(this);
  } else if (exit) {
    delete assign;
    delete if_statement;
    delete loop;
    delete return_statement;
    delete branch;
    delete case_statement;
    delete continue_statement;
    delete null_statement;
    exit->Accept(this);
  } else if (continue_statement) {
    delete assign;
    delete if_statement;
    delete loop;
    delete return_statement;
    delete branch;
    delete case_statement;
    delete exit;
    delete null_statement;
    continue_statement->Accept(this);
  } else if (null_statement) {
    delete assign;
    delete if_statement;
    delete loop;
    delete return_statement;
    delete branch;
    delete case_statement;
    delete exit;
    delete continue_statement;
    null_statement->Accept(this);
  }
}

// Prints a unary node and its children. Example:
//   Unary Node
//     op: MINUS
//     expression:
//       ...
void ASTNodePrintVisitor::Visit(const UnaryNode &node) {
  std::string message = std::string(depth_*2, ' ');
  message += "Unary Node\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "op: " + Token::kTokenStrings[node.op()] + "\n";
  message += std::string(depth_*2, ' ');
  message += "expression:\n";
  messenger_->PrintMessage(message);
  ++depth_;
  node.expression()->Accept(this);
  depth_ = depth_ - 2;
}

// Prints a variable declaration node and its children. Example
//   Variable Declaration
//     var:
//       id: x
//       type: int[]
//       array expression:
//         ...
//     var:
//       id: y
//       type: int
//     ...
void ASTNodePrintVisitor::Visit(const VariableDeclarationNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Variable Declaration\n";
  messenger_->PrintMessage(message);
  ++depth_;
  const VariableDeclarationNode *current_var = &node;
  while (current_var != NULL) {
    message = std::string(depth_*2, ' ');
    message += "var:\n";
    ++depth_;
    message += std::string(depth_*2, ' ');
    message += "id: " + 
      Administrator::spelling_table[current_var->identifier()] + "\n";
    message += std::string(depth_*2, ' ');
    message += "type: " + Token::kTokenStrings[current_var->type()];
    if (current_var->array_expression()) {
      message += "[]\n";
      message += std::string(depth_*2, ' ');
      message += "array expression:\n";
      messenger_->PrintMessage(message);
      ++depth_;
      current_var->array_expression()->Accept(this);
      --depth_;
    } else {
      message += "\n";
      messenger_->PrintMessage(message);
    }
    current_var = current_var->next_variable_declaration();
    --depth_;
  }
  --depth_;
}

// Prints a variable node. Example:
//   Variable
//     id: x
//     array expression:
//       ...
void ASTNodePrintVisitor::Visit(const VariableNode &node){ 
  std::string message = std::string(depth_*2, ' ');
  message += "Variable\n";
  ++depth_;
  message += std::string(depth_*2, ' ');
  message += "id: " + Administrator::spelling_table[node.identifier()] + "\n";
  if (node.array_expression()) {
    message += std::string(depth_*2, ' ');
    message += "array expression:\n";
    messenger_->PrintMessage(message);
    ++depth_;
    node.array_expression()->Accept(this);
    --depth_;
  } else {
    messenger_->PrintMessage(message);
  }
  --depth_;
}
