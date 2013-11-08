// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#ifndef COMPILER_ASTNODE_H_
#define COMPILER_ASTNODE_H_

#include <string>

#include "administrator.h"
#include "ast_node_visitor.h"

class ASTNode {
public:
  //
  virtual ~ASTNode();

  //
  virtual ASTNode *next_node() const;

  //
  void set_next_node(ASTNode *next_node);

  //
  virtual void Accept(ASTNodeVisitor *visitor) = 0;

  //
  void set_line_number(const int line_number);

  //
  int line_number() const {
    return line_number_;
  }

protected:
  //
  ASTNode();

  //
  ASTNode *next_node_;

  //
  int line_number_;
};

class DeclarationNode : public ASTNode {
public:
  // Deletes this node and the next declaration node.
  ~DeclarationNode() { }

  //
  void Accept(ASTNodeVisitor *visitor);

  // Returns the index of this declaration's string ID in the spelling table.
  int identifier() const {
    return identifier_;  
  }

  // Sets the spelling table index for the ID of this declaration.
  void set_identifier(int index);

  // Sets the string ID of this declaration.
  void set_identifier(const std::string &string_identifier);

  // Sets the type of token. Can be INT, BOOL, or VOID.
  void set_type(const Token::TokenName &type);

  // Returns the string of this declaration's ID.
  std::string StringIdentifier() const;
  
  // Returns the type of this declaration. 
  Token::TokenName type() const;

protected:
  // Protected to hide declaration node base class constructor to force user
  // to use subclass constructors.
  // Creates an empty declaration node. It is up to the user to set attributes.
  DeclarationNode();

  // Creates a declaration node with a spelling table index and a type.
  DeclarationNode(const int identifier, const Token::TokenName &type);

  // Spelling table's index of this declaration's identifier.
  int identifier_;

  // Type of the declaration (INT, BOOL, or VOID).
  Token::TokenName type_;
};

class ProgramNode : public ASTNode {
public:
  //
  ProgramNode();

  // Create a program node, with a pointer to the first declaration in the
  // program.
  explicit ProgramNode(DeclarationNode *declaration_node);

  // Deletes this node and its child, declaration node.
  ~ProgramNode();

  //
  void Accept(ASTNodeVisitor *visitor);

  // Get the pointer to the first declaration in this program.
  DeclarationNode *declaration_node() const;

  //
  void set_declaration_node(DeclarationNode *declaration_node);

private:
  // Pointer to the first declaration of the program.
  DeclarationNode *declaration_node_;
};

class ParameterNode : public ASTNode {
public:
  // Creates an empty parameter node. User has to make sure to change 
  // identifier index.
  ParameterNode();

  // Creates a parameter node with an identifer and a type.
  ParameterNode(const int identifier, const Token::TokenName &type);

  // Creates a parameter node with an identifier, a type, and a pointer to
  // the next parameter.
  ParameterNode(const int identifier, const Token::TokenName &type,
                ParameterNode *next_parameter);

  // Deletes this parameter and the next parameter
  ~ParameterNode();

  //
  void Accept(ASTNodeVisitor *visitor);

  // Returns true if this is an array parameter.
  bool array_parameter() const {
    return array_parameter_;
  }

  // Returns the spelling table index of the identifier.
  int identifier() const {
    return identifier_;
  }

  // Returns a pointer to the next parameter.
  ParameterNode *next_parameter() const;

  // Returns true if this is a reference parameter.
  bool reference_parameter() const {
    return reference_parameter_;
  }

  // Sets this node to be an array parameter. If this is true then it sets 
  // reference parameter to false.
  void set_array_parameter(const bool array_parameter);

  // Sets the spelling table index for the identifier.
  void set_identifier(const int index);

  // Sets the identifier as a new string. Updates the spelling table.
  void set_identifier(const std::string &string_identifier);

  // Sets the pointer to the next parameter.
  void set_next_parameter(ParameterNode *next_parameter);

  // Sets this node to be a reference parameter. If this is true then it sets
  // array parameter to false.
  void set_reference_parameter(const bool reference_paramter);

  // Sets the type of this parameter. Can be INT, BOOL, or VOID.
  void set_type(const Token::TokenName &type);

  // Returns the string value of the identifier from the spelling table.
  std::string StringIdentifier() const;
  
  // Returns the type of this paramter.
  Token::TokenName type() const;


private:
  // True if this parameter is an array type.
  bool array_parameter_;

  // Spelling table index of the string identifier of this parameter.
  int identifier_;

  // Pointer to the next parameter for this function. Can be null.
  ParameterNode *next_parameter_;
  
  // True if this parameter is a refernce.
  bool reference_parameter_;
  
  // Type of this paramter. Can be INT or BOOL.
  Token::TokenName type_;
};

class StatementNode : public virtual ASTNode {
public:
  //
  virtual void Accept(ASTNodeVisitor *visitor);
};

class ExpressionNode : public virtual ASTNode {
public:
  ExpressionNode() {
    type_ = Token::UNIVERSAL;
    value_ = 0;
  }

  //
  void Accept(ASTNodeVisitor *visitor);

  //
  Token::TokenName type() const {
    return type_;
  }

  //
  void set_type(const Token::TokenName type) {
    type_ = type;
  }

  //
  int value() const {
    return value_;
  }

  //
  void set_value(const int value) {
    value_ = value;
  }

private:
  Token::TokenName type_;

  int value_;
};

class VariableDeclarationNode : public DeclarationNode {
public:
  // Creates an empty variable declaration node. It is up to the user to set
  // attributes.
  VariableDeclarationNode();
  
  // Creates a variable declaration node with a pointer to the next variable.
  // It is up to the user to set the rest of the attributes.
  explicit VariableDeclarationNode(
      VariableDeclarationNode *next_variable_declaration);

  // Creates a variable declaration node with a name and a type. It is up to 
  // the user to set the rest of the attributes.
  VariableDeclarationNode(const int identifier, const Token::TokenName &type);

  // Creates a variable declaration node with a name, a type, and a pointer to
  // the next variable.
  VariableDeclarationNode(const int identifier, const Token::TokenName &type,
                          VariableDeclarationNode *next_variable_declaration);

  // Deletes this node and the next variable declaration node.
  ~VariableDeclarationNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  ExpressionNode *array_expression() const;

  // Returns a pointer to the next variable declaration.
  VariableDeclarationNode *next_variable_declaration() const;

  //
  void set_array_expression(ExpressionNode *expression);

  //
  void set_array_variable(const bool array_variable);

  // Sets the pointer to the next variable declaration.
  void set_next_variable_declaration(
      VariableDeclarationNode *next_variable_declaration);

  //
  void set_index(const int index) {
    index_ = index;
  }

  //
  int index() const {
    return index_;
  }

  void set_array_size(const int size) {
    array_size_ = size;
  }

  int array_size() const {
    return array_size_;
  }

  bool array_variable() const {
    return array_variable_;
  }

private:
  //
  ExpressionNode *array_expression_;

  //
  bool array_variable_;

  // Pointer to the next variable of the same type but different identifier.
  VariableDeclarationNode *next_variable_declaration_;

  //
  int index_;

  //
  int array_size_;
};

class CompoundNode : public StatementNode {
public:
  //
  CompoundNode();

  //
  CompoundNode(StatementNode *statement);

  //
  CompoundNode(VariableDeclarationNode *variable, StatementNode *statement);

  //
  ~CompoundNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  VariableDeclarationNode *local_variables() const;

  //
  void set_local_variable(VariableDeclarationNode *variable);

  //
  void set_statement(StatementNode *statement);

  //
  StatementNode *statements() const;

private:
  // Pointer to the list of variables local to this compound. Can be null.
  VariableDeclarationNode *local_variables_;

  // Pointer to the list of statemetnts in this compound.
  StatementNode *statements_;
};

class FunctionDeclarationNode  : public DeclarationNode {
public:
  // Creates an empty function node. Users has to set attributes manually.
  FunctionDeclarationNode();

  // Creates a function with at least one parameter.
  FunctionDeclarationNode(ParameterNode *parameter);

  // Creates a void function with an identifier and return type.
  FunctionDeclarationNode(const int identifier, const Token::TokenName &type);

  // Creates an function with at least one parameter, with an identifier and
  // a return type.
  FunctionDeclarationNode(const int identifier, const Token::TokenName &type,
                          ParameterNode *parameter);

  // Deletes this and its parameters.
  ~FunctionDeclarationNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  CompoundNode *compound() const;

  //
  int num_parameters() const {
    return num_parameters_;
  }

  // Returns a pointer to the first paramter of this function. Returns null if
  // this function takes no arguments.
  ParameterNode *parameters() const;

  //
  void set_compound(CompoundNode *compound);

  // Sets the paramater pointer.
  void set_parameter(ParameterNode *parameter);

  //
  void set_num_parameters(const int num) {
    num_parameters_ = num;
  }

private:
  //
  int num_parameters_;

  // List of parameters for this function. Null if this function takes no
  // arguments.
  ParameterNode *parameters_;

  //
  CompoundNode *compound_;
};

class AssignmentNode : public StatementNode {
public:
  //
  AssignmentNode();

  //
  AssignmentNode(const int identifier, ExpressionNode *expression);

  // 
  AssignmentNode(const int identifier, ExpressionNode *array_expression, 
                 ExpressionNode *expression, 
                 const bool array_assignment);

  //
  ~AssignmentNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  bool array_assignment() const {
    return array_assignment_;
  }

  //
  ExpressionNode *array_expression() const;

  //
  int identifier() const {
    return identifier_;
  }

  // 
  void set_array_assignment(const bool array_assignment);

  //
  void set_array_expression(ExpressionNode *array_expression);

  //
  void set_identifier(const int identifier);

  //
  void set_identifier(const std::string &string_identifier);

  //
  void set_value(ExpressionNode *value);

  //
  std::string StringIdentifier() const;

  //
  ExpressionNode *value() const;

private:
  //
  bool array_assignment_;
  
  //
  ExpressionNode *array_expression_;

  //
  int identifier_;

  //
  ExpressionNode *value_;
};

class IfNode : public StatementNode {
public:
  //
  IfNode();

  // Create an if then statement.
  IfNode(ExpressionNode *expression, StatementNode *then_statement);

  // Create an if then else statement.
  IfNode(ExpressionNode *expression, StatementNode *then_statement, 
         StatementNode *else_statement);

  //
  ~IfNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  StatementNode *else_statement() const;

  //
  ExpressionNode *expression() const;

  //
  void set_else_statement(StatementNode *statement);

  //
  void set_expression(ExpressionNode *expression);

  //
  void set_then_statement(StatementNode *statement);

  //
  StatementNode *then_statement() const;

private:
  //
  StatementNode *else_statement_;

  //
  ExpressionNode *expression_;

  //
  StatementNode *then_statement_;
};

class LoopNode : public StatementNode {
public:
  //
  LoopNode(StatementNode *statement);

  //
  ~LoopNode();

  //
  void Accept(ASTNodeVisitor *visitor);

  //
  void set_statements(StatementNode *statement);

  //
  StatementNode *statements() const;

private:
  //
  StatementNode *statements_;
};

class ReturnNode : public StatementNode {
public:
  //
  ReturnNode();

  //
  ReturnNode(ExpressionNode *expression);

  //
  ~ReturnNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  ExpressionNode *expression() const;

  //
  void set_expression(ExpressionNode *expression);

private:
  // Pointer to the expression being returned. Can be null if returning from 
  // a void function.
  ExpressionNode *expression_;
};

class CaseNode : public StatementNode {
public:
  // Creates an empty case statement.
  CaseNode();

  // Creates a default case statement.
  CaseNode(StatementNode *statement);

  // Creates a number case statement.
  CaseNode(const int case_number, StatementNode *statement);

  //
  ~CaseNode();

  //
  void Accept(ASTNodeVisitor *visitor);

  //
  int case_number() const {
    return case_number_;
  }

  //
  bool default_case() const {
    return default_case_;
  }

  //
  void set_case_number(const int case_number);

  //
  void set_default_case(const bool default_case);

  //
  void set_statement(StatementNode *statement);

  //
  StatementNode *statement() const;

private:
  //
  int case_number_;

  //
  bool default_case_;

  //
  StatementNode *statement_;
};

class BranchNode : public StatementNode {
public:
  //
  BranchNode();

  //
  BranchNode(ExpressionNode *expression, CaseNode *cases);

  //
  ~BranchNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  CaseNode *cases() const;

  //
  ExpressionNode *expresion() const;

  //
  void set_cases(CaseNode *cases);

  //
  void set_expression(ExpressionNode *expression);

private:
  //
  CaseNode *cases_;

  //
  ExpressionNode *expression_;
};

class ExitNode : public StatementNode {
public:
  //
  ~ExitNode() { }

  //
  void Accept(ASTNodeVisitor *visitor);
};

class ContinueNode : public StatementNode {
public:
  //
  ~ContinueNode() { }
  
  //
  void Accept(ASTNodeVisitor *visitor);
};

class NullNode : public StatementNode {
public:
  //
  ~NullNode() { }
  
  //
  void Accept(ASTNodeVisitor *visitor);
};

class BinaryNode : public ExpressionNode {
public:
  //
  BinaryNode();

  // 
  BinaryNode(const Token::TokenName &op);

  //
  BinaryNode(ExpressionNode *left_expression, ExpressionNode *right_expression);

  //
  BinaryNode(const Token::TokenName &op, ExpressionNode *left_expression,
             ExpressionNode *right_expression);

  //
  ~BinaryNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  ExpressionNode *left_expression() const;

  //
  Token::TokenName op() const;

  //
  ExpressionNode *right_expression() const;

  // 
  void set_left_expression(ExpressionNode *expression);

  //
  void set_op(const Token::TokenName &op);

  //
  void set_right_expression(ExpressionNode *expression);

private:
  //
  ExpressionNode *left_expression_;

  //
  Token::TokenName op_;

  //
  ExpressionNode *right_expression_;
};

class UnaryNode : public ExpressionNode {
public:
  //
  UnaryNode();

  //
  UnaryNode(const Token::TokenName &op);

  //
  UnaryNode(ExpressionNode *expression);

  //
  UnaryNode(const Token::TokenName &op, ExpressionNode *expression);

  //
  ~UnaryNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);
  
  //
  ExpressionNode *expression() const;

  //
  Token::TokenName op() const;

  //
  void set_expression(ExpressionNode *expression);

  //
  void set_op(const Token::TokenName &op);

private:
  //
  ExpressionNode *expression_;

  //
  Token::TokenName op_;
};

class LiteralNode : public ExpressionNode {
public:
  //
  LiteralNode() : boolean_literal_(false), number_literal_(false), 
      value_(0) { }

  //
  LiteralNode(const int value, const bool boolean_literal, 
              const bool number_literal) : boolean_literal_(boolean_literal), 
      number_literal_(number_literal), value_(value) { }

  //
  ~LiteralNode() { }
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  bool boolean_literal() const {
    return boolean_literal_;
  }

  //
  bool number_literal() const {
    return number_literal_;
  }

  //
  void set_boolean_literal(const bool boolean_literal);

  //
  void set_number_literal(const bool number_literal);

  //
  void set_value(const int value);

  //
  int value() const {
    return value_;
  }

private:
  //
  bool boolean_literal_;

  //
  bool number_literal_;

  //
  int value_;
};

class VariableNode : public ExpressionNode {
public:
  //
  VariableNode();

  //
  VariableNode(const int identifier);

  //
  VariableNode(const int identifier, ExpressionNode *array_expression);

  //
  ~VariableNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  ExpressionNode *array_expression() const;

  //
  bool array_variable() const {
    return array_variable_;
  }

  //
  int identifier() const {
    return identifier_;
  }

  //
  void set_array_expression(ExpressionNode *array_expression);

  //
  void set_array_variable(const bool array_variable);

  //
  void set_identifier(const int identifier);

  //
  void set_identifier(const std::string &string_identifier);

  //
  std::string StringIdentifier() const;

  //
  DeclarationNode *declaration_pointer() const;

  //
  void set_declaration_pointer(DeclarationNode *declaration_pointer);

  //
  void set_array_index(const int index) {
    index_ = index;
  }

  //
  int get_array_index() const {
    return index_;
  }

private:
  //
  DeclarationNode *declaration_pointer_;

  //
  ExpressionNode *array_expression_;

  //
  bool array_variable_;

  //
  int identifier_;

  //
  int index_;
};

class CallNode : public ExpressionNode, public StatementNode {
public:
  //
  CallNode();

  //
  CallNode(const int identifier);

  //
  CallNode(const int identifier, ExpressionNode *arguments);

  //
  ~CallNode();
  
  //
  void Accept(ASTNodeVisitor *visitor);

  //
  ExpressionNode *arguments() const;

  int identifier() const {
    return identifier_;
  }

  //
  void set_argurments(ExpressionNode *argument);

  //
  void set_identifier(const int identifier);

  //
  void set_identifier(const std::string &string_identifier);

  //
  std::string StringIdentifier() const;

private:
  //
  ExpressionNode *arguments_;

  //
  int identifier_;
};

#endif // COMPILER_ASTNODE_H