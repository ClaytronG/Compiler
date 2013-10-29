// Author: Clayton Green (kgreen1@unbc.ca)

#include <algorithm>
#include <memory>

#include "ast_node.h"
#include "administrator.h"
#include "parser.h"

Parser::Parser(const std::string &file, const std::string &filename,
               Administrator *administrator) 
  : scanner_(file, filename, *administrator), filename_(filename), depth_(0), 
    administrator_(administrator), lookahead_token_(Token::ERROR), synch_() {
  good_ = scanner_.good();
  //synch_.push_back(Token::SEMI);
}

ASTNode *Parser::transition(const std::string &function_name,
                            ParserFunction function) {
  // Print entering message
  std::string message = std::string(++depth_, ' ');
  message += "Entering " + function_name + "\n";
  administrator()->messenger()->PrintMessage(message);

  ASTNode *node = (*this.*function)();

  // Print exiting message
  message = std::string(depth_--, ' ');
  message += "Exiting " + function_name + "\n";
  administrator()->messenger()->PrintMessage(message);

  return node;
}

void Parser::Match(const Token::TokenName &expected/*, SynchSet &synch*/) {
  // --
  std::string message = std::string(depth_+1, ' ');
  message += "Matching " + Token::kTokenStrings[lookahead_] + "\n";
  administrator_->messenger()->PrintMessage(message);
  // --

  if (lookahead_ == expected) {
    lookahead_token_ = scanner_.GetToken();
    lookahead_ = lookahead_token_.name();

    // -- 
    message = std::string(depth_+1, ' ');
    message += Administrator::IntToString(scanner_.line_number());
    message += "> " + lookahead_token_.toString() + "\n";
    administrator_->messenger()->PrintMessage(message);
    // -- 
    message = std::string(depth_+1, ' ');
    message += "Loading " + Token::kTokenStrings[lookahead_] + "\n";
    administrator_->messenger()->PrintMessage(message);
    // --

  } else {
    message = "match ";
    message += Token::kTokenStrings[expected];
    SyntaxError(message);
  }
  //SyntaxCheck(message);
}

void Parser::SyntaxCheck(const std::string &message/*, SynchSet &synch*/) {
  auto it = std::find(synch_.begin(), synch_.end(), lookahead_);
  // If the lookahead_ isn't in the synch set...
  if (it == synch_.end()) {
    fprintf(stdout, "nope\n");
    SyntaxError(message);
  }
}

void Parser::SyntaxError(const std::string &message/*, SynchSet &synch*/) {
  administrator_->messenger()->AddError(filename_, scanner_.line_number(), message);
  auto it = std::find(synch_.begin(), synch_.end(), lookahead_);
  // While the lookahead_ isn't in the synch set...
  while (it == synch_.end()) {
    lookahead_token_ = scanner_.GetToken();
    lookahead_ = lookahead_token_.name();
    it = std::find(synch_.begin(), synch_.end(), lookahead_);
  }
}

ASTNode *Parser::Parse() {
  Token token = scanner_.GetToken();
  lookahead_ = token.name();
  // --
  std::string message = std::string(depth_ + 1, ' ');
  message += Administrator::IntToString(scanner_.line_number());
  message += "> " + token.toString() + "\n";
  administrator_->messenger()->PrintMessage(message);
  message = std::string(depth_ + 1, ' ');
  message += "Loading " + Token::kTokenStrings[lookahead_] + "\n";
  administrator_->messenger()->PrintMessage(message);
  // --
  //SynchSet set;
  //set.push_back(Token::INT, Token::BOOL, Token::VOID, Token::ENDFILE);
  return transition("program", &Parser::Program);
}

ASTNode *Parser::Program(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::INT);
  ////synch_.push_back(Token::BOOL);
  ////synch_.push_back(Token::VOID);
  ////synch_.push_back(Token::ENDFILE);
  ProgramNode *program = new ProgramNode();
  program->set_declaration_node(dynamic_cast<DeclarationNode*>(transition(
    "declaration", &Parser::Declaration)));
  DeclarationNode *current_node = program->declaration_node();
  while (lookahead_ == Token::INT || lookahead_ == Token::BOOL || 
         lookahead_ == Token::VOID) {
    current_node->set_next_node(dynamic_cast<DeclarationNode*>(transition(
      "declaration", &Parser::Declaration)));
    current_node = dynamic_cast<DeclarationNode*>(current_node->next_node());
  }
  return program;
}

ASTNode *Parser::Declaration(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::INT);
  ////synch_.push_back(Token::BOOL);
  ////synch_.push_back(Token::VOID);
  ////synch_.push_back(Token::ENDFILE);
  if (lookahead_ == Token::VOID) {
	  Match(Token::VOID/*, set*/);
    int identifier = lookahead_token_.value();
    Match(Token::ID/*, set*/);
    FunctionDeclarationNode *function = dynamic_cast<FunctionDeclarationNode*>(
      transition("fun-dec-tail", &Parser::FunDecTail));
    function->set_identifier(identifier);
    function->set_type(Token::VOID);
    return function;
  } else if (lookahead_ == Token::INT || lookahead_ == Token::BOOL) {
    Token::TokenName type = NonvoidSpecifier();
    int identifier = lookahead_token_.value();
    ////synch_.push_back(Token::LSQR);
    ////synch_.push_back(Token::SEMI);
    ////synch_.push_back(Token::COMMA);
    ////synch_.push_back(Token::LPAREN);
    Match(Token::ID/*, set*/);
    ASTNode *node = transition("dec-tail", &Parser::DecTail);
    VariableDeclarationNode *var = dynamic_cast<VariableDeclarationNode*>(node);
    FunctionDeclarationNode *fun = dynamic_cast<FunctionDeclarationNode*>(node);
    if (fun) {
      delete var;
      fun->set_type(type);
      fun->set_identifier(identifier);
      return fun;      
    } else if (var) {
      delete fun;
      var->set_identifier(identifier);
      VariableDeclarationNode *current_var = var;
      while (current_var != NULL) {
        current_var->set_type(type);
        current_var = current_var->next_variable_declaration();
      }
      return var;
    }
    return NULL;
  } else {
    SyntaxError("declaration");
    return NULL;
  }
}

Token::TokenName Parser::NonvoidSpecifier(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::ID);
  std::string message = std::string(++depth_, ' ');
  message += "Entering non-void-specifier\n";
  std::string leave_message = std::string(depth_--, ' ');
  leave_message += "Leaving non-void-specifier\n";
  administrator_->messenger()->PrintMessage(message);
  if (lookahead_ == Token::INT) {
    Match(Token::INT/*, set*/);
    administrator_->messenger()->PrintMessage(leave_message);
    return Token::INT;
  } else if (lookahead_ == Token::BOOL) {
    Match(Token::BOOL/*, set*/);
    administrator_->messenger()->PrintMessage(leave_message);
    return Token::BOOL;
  } else {    
    administrator_->messenger()->PrintMessage(leave_message);
    SyntaxError("nonvoid-specifier");
    return Token::ERROR;
  }
}

ASTNode *Parser::DecTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::INT);
  ////synch_.push_back(Token::BOOL);
  ////synch_.push_back(Token::VOID);
  ////synch_.push_back(Token::ENDFILE);
  if (lookahead_ == Token::LSQR || lookahead_ == Token::SEMI || 
      lookahead_ == Token::COMMA) {
    return transition("var-dec-tail", &Parser::VarDecTail);
  } else if (lookahead_ == Token::LPAREN) {
    return transition("fun-dec-tail", &Parser::FunDecTail);
  } else {
    SyntaxError("dec-tail");
    return NULL;
  }
}

ASTNode *Parser::VarDecTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::INT);
  ////synch_.push_back(Token::BOOL);
  ////synch_.push_back(Token::VOID);
  ////synch_.push_back(Token::ENDFILE);
  ////synch_.push_back(Token::LCRLY);
  ////synch_.push_back(Token::IF);
  ////synch_.push_back(Token::LOOP);
  ////synch_.push_back(Token::EXIT);
  ////synch_.push_back(Token::CONTINUE);
  ////synch_.push_back(Token::RETURN);
  ////synch_.push_back(Token::SEMI);
  ////synch_.push_back(Token::ID);
  ////synch_.push_back(Token::BRANCH);
  VariableDeclarationNode *variable = new VariableDeclarationNode();
  //////synch_.push_back(Token::LSQR);
  //SyntaxCheck("var-dec-tail");
  if (lookahead_ == Token::LSQR) {
    Match(Token::LSQR/*, set*/);
    variable->set_array_variable(true);
    variable->set_array_expression(dynamic_cast<ExpressionNode*>(transition(
      "add-exp", &Parser::AddExp)));
    Match(Token::RSQR/*, set*/);
  }

  VariableDeclarationNode *node = variable;
  //////synch_.push_back(Token::COMMA);
  //SyntaxCheck("var-dec-tail");
  while (lookahead_ == Token::COMMA) {
    Match(Token::COMMA/*, set*/);
    node->set_next_variable_declaration(dynamic_cast<VariableDeclarationNode*>(
      transition("var-name", &Parser::VarName)));
    node = node->next_variable_declaration();
  }
  Match(Token::SEMI/*, set*/);
  return variable;
}

ASTNode *Parser::VarName(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::COMMA);
  ////synch_.push_back(Token::SEMI);
  VariableDeclarationNode *variable = new VariableDeclarationNode();
  int identifier = lookahead_token_.value();
  Match(Token::ID/*, set*/);
  variable->set_identifier(identifier);
  //////synch_.push_back(Token::LSQR);
  //SyntaxCheck("var-name");
  if (lookahead_ == Token::LSQR) {
    Match(Token::LSQR/*, set*/);
    variable->set_array_variable(true);
    variable->set_array_expression(dynamic_cast<ExpressionNode*>(transition(
      "add-exp", &Parser::AddExp)));
    Match(Token::RSQR/*, set*/);
  }
  return variable;
}

ASTNode *Parser::FunDecTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::INT);
  ////synch_.push_back(Token::BOOL);
  ////synch_.push_back(Token::VOID);
  ////synch_.push_back(Token::ENDFILE);
  int identifier = lookahead_token_.value();
  Match(Token::LPAREN/*, set*/);
  FunctionDeclarationNode *function = new FunctionDeclarationNode();
  function->set_identifier(identifier);
  function->set_parameter(dynamic_cast<ParameterNode*>(transition(
    "params", &Parser::Params)));
  Match(Token::RPAREN/*, set*/);
  function->set_compound(dynamic_cast<CompoundNode*>(transition(
    "compound-stmt", &Parser::CompoundStmt)));
  //Trace("Compound-Stmt", *this, &Parser::CompoundStmt/*, set*/);
  return function;
}

ASTNode *Parser::Params(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::RPAREN);
  if (lookahead_ == Token::REF || lookahead_ == Token::INT ||
      lookahead_ == Token::BOOL) {
    ASTNode *node = transition("param", &Parser::Param);
    ParameterNode *current_node = dynamic_cast<ParameterNode*>(node);
    //////synch_.push_back(Token::COMMA);
    //SyntaxCheck("params");
    while (lookahead_ == Token::COMMA) {
      Match(Token::COMMA/*, set*/);
      current_node->set_next_parameter(dynamic_cast<ParameterNode*>(
        transition("param", &Parser::Param)));
      current_node = current_node->next_parameter(); 
    }
    return node;
  } else if (lookahead_ == Token::VOID) {
    Match(Token::VOID/*, set*/);
    return NULL;
  } else {
    SyntaxError("params");
    return NULL;
  }
}

ASTNode *Parser::Param(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::COMMA);
  ////synch_.push_back(Token::RPAREN);
  ParameterNode *param = new ParameterNode();
  if (lookahead_ == Token::REF) {
    Match(Token::REF/*, set*/);
    Token::TokenName type = NonvoidSpecifier();
    int identifier = lookahead_token_.value();
    Match(Token::ID/*, set*/);
    param->set_identifier(identifier);
    param->set_type(type);
    param->set_reference_parameter(true);
    return param;
  } else if (lookahead_ == Token::INT || lookahead_ == Token::BOOL) {
    Token::TokenName type = NonvoidSpecifier();
    int identifier = lookahead_token_.value();
    Match(Token::ID/*, set*/);
    //////synch_.push_back(Token::LSQR);
    //SyntaxCheck("param");
    if (lookahead_ == Token::LSQR) {
      Match(Token::LSQR/*, set*/);
      Match(Token::RSQR/*, set*/);
      param->set_array_parameter(true);
    }
    param->set_identifier(identifier);
    param->set_type(type);
    return param;
  } else {
    SyntaxError("param");
    return NULL;
  }
}

ASTNode *Parser::Statement(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::LCRLY);
  ////synch_.push_back(Token::IF);
  ////synch_.push_back(Token::LOOP);
  ////synch_.push_back(Token::EXIT);
  ////synch_.push_back(Token::CONTINUE);
  ////synch_.push_back(Token::RETURN);
  ////synch_.push_back(Token::SEMI);
  ////synch_.push_back(Token::ID);
  ////synch_.push_back(Token::RCRLY);
  ////synch_.push_back(Token::ELSE);
  ////synch_.push_back(Token::END);
  ////synch_.push_back(Token::BRANCH);
  ////synch_.push_back(Token::CASE);
  ////synch_.push_back(Token::DEFAULT);
  if (lookahead_ == Token::ID) {
    return transition("id-stmt", &Parser::IdStmt);
  } else if (lookahead_ == Token::LCRLY) {
    return transition("compound-stmt", &Parser::CompoundStmt);
  } else if (lookahead_ == Token::IF) {
    return transition("if-stmt", &Parser::IfStmt);
  } else if (lookahead_ == Token::LOOP) {
    return transition("loop-stmt", &Parser::LoopStmt);
  } else if (lookahead_ == Token::EXIT) {
    return transition("exit-stmt", &Parser::ExitStmt);
  } else if (lookahead_ == Token::CONTINUE) {
    return transition("continue-stmt", &Parser::ContinueStmt);
  } else if (lookahead_ == Token::RETURN) {
    return transition("return-stmt", &Parser::ReturnStmt);
  } else if (lookahead_ == Token::SEMI) {
    return transition("null-stmt", &Parser::NullStmt);
  } else if (lookahead_ == Token::BRANCH) {
    return transition("branch-stmt", &Parser::BranchStmt);
  } else {
    SyntaxError("statement");
    return NULL;
  }
}

ASTNode *Parser::IdStmt(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::LCRLY);
  ////synch_.push_back(Token::IF);
  ////synch_.push_back(Token::LOOP);
  ////synch_.push_back(Token::EXIT);
  ////synch_.push_back(Token::CONTINUE);
  ////synch_.push_back(Token::RETURN);
  ////synch_.push_back(Token::SEMI);
  ////synch_.push_back(Token::ID);
  ////synch_.push_back(Token::RCRLY);
  ////synch_.push_back(Token::ELSE);
  ////synch_.push_back(Token::END);
  ////synch_.push_back(Token::BRANCH);
  ////synch_.push_back(Token::CASE);
  ////synch_.push_back(Token::DEFAULT);
  int identifier = lookahead_token_.value();
  Match(Token::ID/*, set*/);
  ASTNode *node = transition("id-stmt-tail", &Parser::IdStmtTail);
  AssignmentNode *assign = dynamic_cast<AssignmentNode*>(node);
  CallNode *call = dynamic_cast<CallNode*>(node);
  if (assign) {
    delete call;
    assign->set_identifier(identifier);
    return assign;
  } else if (call) {
    delete assign;
    call->set_identifier(identifier);
    return call;
  }
  return NULL;
}

ASTNode *Parser::IdStmtTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::LCRLY);
  ////synch_.push_back(Token::IF);
  ////synch_.push_back(Token::LOOP);
  ////synch_.push_back(Token::EXIT);
  ////synch_.push_back(Token::CONTINUE);
  ////synch_.push_back(Token::RETURN);
  ////synch_.push_back(Token::SEMI);
  ////synch_.push_back(Token::ID);
  ////synch_.push_back(Token::RCRLY);
  ////synch_.push_back(Token::ELSE);
  ////synch_.push_back(Token::END);
  ////synch_.push_back(Token::BRANCH);
  ////synch_.push_back(Token::CASE);
  ////synch_.push_back(Token::DEFAULT);
  if (lookahead_ == Token::LSQR || lookahead_ == Token::ASSIGN) {
    return transition("assign-stmt-tail", &Parser::AssignStmtTail);
  } else if (lookahead_ == Token::LPAREN) {
    return transition("call-stmt-tail", &Parser::CallStmtTail);
  } else {
    SyntaxError("id-stmt-tail");
    return NULL;
  }
}

ASTNode *Parser::AssignStmtTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::LCRLY);
  ////synch_.push_back(Token::IF);
  ////synch_.push_back(Token::LOOP);
  ////synch_.push_back(Token::EXIT);
  ////synch_.push_back(Token::CONTINUE);
  ////synch_.push_back(Token::RETURN);
  ////synch_.push_back(Token::SEMI);
  ////synch_.push_back(Token::ID);
  ////synch_.push_back(Token::RCRLY);
  ////synch_.push_back(Token::ELSE);
  ////synch_.push_back(Token::END);
  ////synch_.push_back(Token::BRANCH);
  ////synch_.push_back(Token::CASE);
  ////synch_.push_back(Token::DEFAULT);
  AssignmentNode *assign = new AssignmentNode();
  //////synch_.push_back(Token::LSQR);
  //SyntaxCheck("assign-stmt-tail");
  if (lookahead_ == Token::LSQR) {
    Match(Token::LSQR/*, set*/);
    assign->set_array_assignment(true);
    assign->set_array_expression(dynamic_cast<ExpressionNode*>(transition(
      "add-exp", &Parser::AddExp)));
    Match(Token::RSQR/*, set*/);
  }
  Match(Token::ASSIGN/*, set*/);
  assign->set_value(dynamic_cast<ExpressionNode*>(transition(
    "expression", &Parser::Expression)));
  Match(Token::SEMI/*, set*/);
  return assign;
}

ASTNode *Parser::CallStmtTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::LCRLY);
  ////synch_.push_back(Token::IF);
  ////synch_.push_back(Token::LOOP);
  ////synch_.push_back(Token::EXIT);
  ////synch_.push_back(Token::CONTINUE);
  ////synch_.push_back(Token::RETURN);
  ////synch_.push_back(Token::SEMI);
  ////synch_.push_back(Token::ID);
  ////synch_.push_back(Token::RCRLY);
  ////synch_.push_back(Token::ELSE);
  ////synch_.push_back(Token::END);
  ////synch_.push_back(Token::BRANCH);
  ////synch_.push_back(Token::CASE);
  ////synch_.push_back(Token::DEFAULT);
  CallNode *call = new CallNode();
  call->set_argurments(dynamic_cast<ExpressionNode*>(
  transition("call-tail", &Parser::CallTail)));
  Match(Token::SEMI/*, set*/);
  return call;
}

ASTNode *Parser::CallTail(/*SynchSet &set*/) {
  ////synch_.clear();
  ////synch_.push_back(Token::LTEQ);
  ////synch_.push_back(Token::LT);
  ////synch_.push_back(Token::GT);
  ////synch_.push_back(Token::GTEQ);
  ////synch_.push_back(Token::EQ);
  ////synch_.push_back(Token::NEQ);
  ////synch_.push_back(Token::PLUS);
  ////synch_.push_back(Token::MINUS);
  ////synch_.push_back(Token::OR);
  ////synch_.push_back(Token::ORELSE);
  ////synch_.push_back(Token::MULT);
  //synch_.push_back(Token::DIV);
  //synch_.push_back(Token::MOD);
  //synch_.push_back(Token::AND);
  //synch_.push_back(Token::ANDTHEN);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  ASTNode *node = NULL;
  Match(Token::LPAREN/*, set*/);
  ////synch_.push_back(Token::MINUS);
  ////synch_.push_back(Token::NOT);
  ////synch_.push_back(Token::LPAREN);
  ////synch_.push_back(Token::NUM);
  ////synch_.push_back(Token::BLIT);
  ////synch_.push_back(Token::ID);
  //SyntaxCheck("call-tail");
  if (lookahead_ == Token::MINUS  || lookahead_ == Token::NOT ||
      lookahead_ == Token::LPAREN || lookahead_ == Token::NUM ||
      lookahead_ == Token::BLIT   ||lookahead_ == Token::ID) {
    node = transition("arguments", &Parser::Arguments);
  }
  Match(Token::RPAREN/*, set*/);
  return node;
}

ASTNode *Parser::Arguments(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::RPAREN);
  ExpressionNode *expression = dynamic_cast<ExpressionNode*>(transition(
    "expression", &Parser::Expression));
  ExpressionNode *node = expression;
  //SyntaxCheck("arguments");
  while (lookahead_ == Token::COMMA) {
    Match(Token::COMMA/*, set*/);
    node->set_next_node(dynamic_cast<ExpressionNode*>(transition(
      "expression", &Parser::Expression)));
    node = dynamic_cast<ExpressionNode*>(node->next_node());
  }
  return expression;
}

ASTNode *Parser::CompoundStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  //synch_.push_back(Token::INT);
  //synch_.push_back(Token::BOOL);
  //synch_.push_back(Token::VOID);
  //synch_.push_back(Token::ENDFILE);
  Match(Token::LCRLY/*, set*/);
  CompoundNode *compound = new CompoundNode();
  Token::TokenName type;
  // Parse optional variable declarations
  VariableDeclarationNode *first_variable = NULL;
  ////synch_.push_back(Token::INT);
  ////synch_.push_back(Token::BOOL);
  //SyntaxCheck("compound-stmt");
  if (lookahead_ == Token::INT || lookahead_ == Token::BOOL) {
    first_variable = new VariableDeclarationNode();
    type = NonvoidSpecifier();
    int identifier = lookahead_token_.value();
    Match(Token::ID/*, set*/);
    first_variable = dynamic_cast<VariableDeclarationNode*>(transition(
      "var-dec-tail", &Parser::VarDecTail));
    first_variable->set_identifier(identifier);
    VariableDeclarationNode *current_node = first_variable;
    while (current_node != NULL) {
      current_node->set_type(type);
      current_node = current_node->next_variable_declaration();
    }
  }
  compound->set_local_variable(first_variable);

  // Parse the statement list
  StatementNode *first_statement = dynamic_cast<StatementNode*>(transition(
    "statement", &Parser::Statement));
  compound->set_statement(first_statement);
  StatementNode *current_statement = first_statement;
  while(lookahead_ == Token::LCRLY     || lookahead_ == Token::IF ||
         lookahead_ == Token::LOOP     || lookahead_ == Token::EXIT ||
         lookahead_ == Token::CONTINUE || lookahead_ == Token::RETURN ||
         lookahead_ == Token::SEMI     || lookahead_ == Token::ID ||
         lookahead_ == Token::BRANCH) {
    current_statement->set_next_node(dynamic_cast<StatementNode*>(transition(
      "statement", &Parser::Statement)));
    current_statement = dynamic_cast<StatementNode*>(
      current_statement->next_node());
  }
  Match(Token::RCRLY/*, set*/);
  return compound;
}

ASTNode *Parser::IfStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  Match(Token::IF/*, set*/);
  Match(Token::LPAREN/*, set*/);
  ExpressionNode *expression = dynamic_cast<ExpressionNode*>(transition(
    "expression", &Parser::Expression));
  Match(Token::RPAREN/*, set*/);
  StatementNode *statement = dynamic_cast<StatementNode*>(transition(
    "statement", &Parser::Statement));
  IfNode *node = new IfNode(expression, statement);
  ////synch_.push_back(Token::ELSE);
  //SyntaxCheck("if-stmt");
  if (lookahead_ == Token::ELSE) {
    Match(Token::ELSE/*, set*/);
    node->set_else_statement(dynamic_cast<StatementNode*>(transition(
      "statement", &Parser::Statement)));
  }
  return node;
}

ASTNode *Parser::LoopStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  Match(Token::LOOP/*, set*/);
  LoopNode *loop = new LoopNode(dynamic_cast<StatementNode*>(transition(
    "statement", &Parser::Statement)));
  StatementNode *node = loop->statements();
  while(lookahead_ == Token::LCRLY     || lookahead_ == Token::IF ||
         lookahead_ == Token::LOOP     || lookahead_ == Token::EXIT ||
         lookahead_ == Token::CONTINUE || lookahead_ == Token::RETURN ||
         lookahead_ == Token::SEMI     || lookahead_ == Token::ID ||
         lookahead_ == Token::BRANCH) {
    node->set_next_node(transition("statement", &Parser::Statement));
    node = dynamic_cast<StatementNode*>(node->next_node());
  }
  Match(Token::END/*, set*/);
  Match(Token::SEMI/*, set*/);
  return loop;
}

ASTNode *Parser::ExitStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  Match(Token::EXIT/*, set*/);
  Match(Token::SEMI/*, set*/);
  return new ExitNode();
}

ASTNode *Parser::ContinueStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  Match(Token::CONTINUE/*, set*/);
  Match(Token::SEMI/*, set*/);
  return new ContinueNode();
}

ASTNode *Parser::ReturnStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  Match(Token::RETURN/*, set*/);
  ReturnNode *return_node = new ReturnNode();  
  ////synch_.push_back(Token::MINUS);
  ////synch_.push_back(Token::NOT);
  ////synch_.push_back(Token::LPAREN);
 // //synch_.push_back(Token::NUM);
  ////synch_.push_back(Token::BLIT);
  ////synch_.push_back(Token::ID);
  //SyntaxCheck("return-stmt");
  if (lookahead_ == Token::MINUS  || lookahead_ == Token::NOT ||
      lookahead_ == Token::LPAREN || lookahead_ == Token::NUM ||
      lookahead_ == Token::BLIT   || lookahead_ == Token::ID) {
    return_node->set_expression(dynamic_cast<ExpressionNode*>(transition(
      "expression", &Parser::Expression)));
  }
  Match(Token::SEMI/*, set*/);
  return return_node;
}

ASTNode *Parser::NullStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  Match(Token::SEMI/*, set*/);
  return new NullNode();
}

ASTNode *Parser::BranchStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  BranchNode *branch = new BranchNode();
  Match(Token::BRANCH/*, set*/);
  Match(Token::LPAREN/*, set*/);
  ExpressionNode *expression = dynamic_cast<ExpressionNode*>(transition(
    "add-exp", &Parser::AddExp));
  Match(Token::RPAREN/*, set*/);
  branch->set_expression(expression);
  CaseNode *case_statement = dynamic_cast<CaseNode*>(transition(
    "case-stmt", &Parser::CaseStmt));
  branch->set_cases(case_statement);
  while (lookahead_ == Token::CASE || lookahead_ == Token::DEFAULT) {
    case_statement->set_next_node(dynamic_cast<CaseNode*>(transition(
      "case-stmt", &Parser::CaseStmt)));
    case_statement = dynamic_cast<CaseNode*>(case_statement->next_node());
  }
  Match(Token::END/*, set*/);
  Match(Token::SEMI/*, set*/);
  return branch;
}

ASTNode *Parser::CaseStmt(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LCRLY);
  //synch_.push_back(Token::IF);
  //synch_.push_back(Token::LOOP);
  //synch_.push_back(Token::EXIT);
  //synch_.push_back(Token::CONTINUE);
  //synch_.push_back(Token::RETURN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::ID);
  //synch_.push_back(Token::RCRLY);
  //synch_.push_back(Token::ELSE);
  //synch_.push_back(Token::END);
  //synch_.push_back(Token::BRANCH);
  //synch_.push_back(Token::CASE);
  //synch_.push_back(Token::DEFAULT);
  if (lookahead_ == Token::CASE) {
    Match(Token::CASE/*, set*/);
    Match(Token::NUM/*, set*/);
    int number = lookahead_token_.value();
    Match(Token::COLON/*, set*/);
    StatementNode *statement = dynamic_cast<StatementNode*>(transition(
      "statement", &Parser::Statement));
    return new CaseNode(number, statement);
  } else if (lookahead_ == Token::DEFAULT) {
    Match(Token::DEFAULT/*, set*/);
    Match(Token::COLON/*, set*/);
    StatementNode *statement = dynamic_cast<StatementNode*>(transition(
      "statement", &Parser::Statement));
    return new CaseNode(statement);
  } else {
    SyntaxError("case-stmt");
    return NULL;
  }
}

ASTNode *Parser::Expression(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  ExpressionNode *expression = dynamic_cast<ExpressionNode*>(transition(
    "add-exp", &Parser::AddExp));
  ////synch_.push_back(Token::LTEQ);
  ////synch_.push_back(Token::LT);
  ////synch_.push_back(Token::GT);
  ////synch_.push_back(Token::GTEQ);
  ////synch_.push_back(Token::EQ);
  ////synch_.push_back(Token::NEQ);
  //SyntaxCheck("expression");
  if (lookahead_ == Token::LTEQ || lookahead_ == Token::LT ||
      lookahead_ == Token::GT   || lookahead_ == Token::GTEQ ||
      lookahead_ == Token::EQ   || lookahead_ == Token::NEQ) {
    BinaryNode *relop = dynamic_cast<BinaryNode*>(transition(
      "relop", &Parser::Relop));
    relop->set_left_expression(expression);
    ExpressionNode *right = dynamic_cast<ExpressionNode*>(transition(
      "add-exp", &Parser::AddExp));
    relop->set_right_expression(right);
    return relop;
  }
  return expression;
}


ASTNode *Parser::AddExp(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  ExpressionNode *expression;
  UnaryNode *unary = NULL;
  bool unary_set = false;
  ////synch_.push_back(Token::MINUS);
  //SyntaxCheck("add-exp");
  if (lookahead_ == Token::MINUS) {
    unary = dynamic_cast<UnaryNode*>(transition("uminus", &Parser::Uminus));
  }
  ExpressionNode *term = dynamic_cast<ExpressionNode*>(transition(
    "term", &Parser::Term));
  if (unary != NULL) {
    unary->set_expression(term);
    expression = unary;
  } else {
    delete unary;
    expression = term;
  }
  ExpressionNode *node = expression;
  ASTNode *return_node = NULL;
  ////synch_.push_back(Token::PLUS);
  ////synch_.push_back(Token::OR);
  ////synch_.push_back(Token::ORELSE);
  //SyntaxCheck("add-exp");
  while(lookahead_ == Token::PLUS || lookahead_ == Token::MINUS ||
         lookahead_ == Token::OR  || lookahead_ == Token::ORELSE) {
    BinaryNode *add_op = dynamic_cast<BinaryNode*>(transition(
      "add-op", &Parser::Addop));
    if (return_node == NULL) {
      return_node = add_op;
    }
    add_op->set_left_expression(node);
    ExpressionNode *right = dynamic_cast<ExpressionNode*>(transition(
      "term", &Parser::Term));
    add_op->set_right_expression(right);
    node = right;
  }
  if (return_node != NULL) {
    return return_node;
  }
  delete return_node;
  return expression;
}

ASTNode *Parser::Term(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::PLUS);
  //synch_.push_back(Token::MINUS);
  //synch_.push_back(Token::OR);
  //synch_.push_back(Token::ORELSE);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  ASTNode *expression = transition("factor", &Parser::Factor);
  ExpressionNode *node = dynamic_cast<ExpressionNode*>(expression);
  ASTNode *return_node = NULL;
  ////synch_.push_back(Token::MULT);
  ////synch_.push_back(Token::DIV);
  ////synch_.push_back(Token::MOD);
  ////synch_.push_back(Token::AND);
  ////synch_.push_back(Token::ANDTHEN);
  //SyntaxCheck("term");
  while(lookahead_ == Token::MULT || lookahead_ == Token::DIV ||
         lookahead_ == Token::MOD || lookahead_ == Token::AND ||
         lookahead_ == Token::ANDTHEN) {
    BinaryNode *bin_op = dynamic_cast<BinaryNode*>(transition(
      "multop", &Parser::Multop));
    if (return_node == NULL) {
      return_node = bin_op;
    }
    bin_op->set_left_expression(node);
    ExpressionNode *right = dynamic_cast<ExpressionNode*>(
      transition("factor", &Parser::Factor));
    bin_op->set_right_expression(right);
    node = right;
  }
  if (return_node != NULL) {
    return return_node;
  }
  delete return_node;
  return expression;
}

ASTNode *Parser::Factor(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::PLUS);
  //synch_.push_back(Token::MINUS);
  //synch_.push_back(Token::OR);
  //synch_.push_back(Token::ORELSE);
  //synch_.push_back(Token::MULT);
  //synch_.push_back(Token::DIV);
  //synch_.push_back(Token::MOD);
  //synch_.push_back(Token::AND);
  //synch_.push_back(Token::ANDTHEN);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  if (lookahead_ == Token::NOT || lookahead_ == Token::LPAREN ||
      lookahead_ == Token::NUM || lookahead_ == Token::BLIT) {
    return transition("nid-factor", &Parser::NidFactor);
  } else if (lookahead_ == Token::ID) {
    return transition("id-factor", &Parser::IdFactor);
  } else {
    SyntaxError("factor");
    return NULL;
  }
}

ASTNode *Parser::NidFactor(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::PLUS);
  //synch_.push_back(Token::MINUS);
  //synch_.push_back(Token::OR);
  //synch_.push_back(Token::ORELSE);
  //synch_.push_back(Token::MULT);
  //synch_.push_back(Token::DIV);
  //synch_.push_back(Token::MOD);
  //synch_.push_back(Token::AND);
  //synch_.push_back(Token::ANDTHEN);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  if (lookahead_ == Token::NOT) {
    Match(Token::NOT/*, set*/);
    ExpressionNode *node = dynamic_cast<ExpressionNode*>(
      transition("factor", &Parser::Factor));
    return new UnaryNode(Token::NOT, node);
  } else if (lookahead_ == Token::LPAREN) {
    Match(Token::LPAREN/*, set*/);
    ASTNode *node = transition("expression", &Parser::Expression);
    Match(Token::RPAREN/*, set*/);
    return node;
  } else if (lookahead_ == Token::NUM) {
    int value = lookahead_token_.value();
    Match(Token::NUM/*, set*/);
    return new LiteralNode(value, false, true);
  } else if (lookahead_ == Token::BLIT) {
    int value = lookahead_token_.value();
    Match(Token::BLIT/*, set*/);
    return new LiteralNode(value, true, false);
  } else {
    SyntaxError("nid-factor");
    return NULL;
  }
}

ASTNode *Parser::IdFactor(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::PLUS);
  //synch_.push_back(Token::MINUS);
  //synch_.push_back(Token::OR);
  //synch_.push_back(Token::ORELSE);
  //synch_.push_back(Token::MULT);
  //synch_.push_back(Token::DIV);
  //synch_.push_back(Token::MOD);
  //synch_.push_back(Token::AND);
  //synch_.push_back(Token::ANDTHEN);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  int identifier = lookahead_token_.value();
  Match(Token::ID/*, set*/);
  ASTNode *base_node = transition("id-tail", &Parser::IdTail);
  VariableNode *var_node = dynamic_cast<VariableNode*>(base_node);
  CallNode *call_node = dynamic_cast<CallNode*>(base_node);
  if (var_node == NULL) {
    delete var_node;
    call_node->set_identifier(identifier);
    return call_node;
  } else if (call_node == NULL) {
    delete call_node;
    var_node->set_identifier(identifier);
    return var_node;
  }
  return NULL;
}

ASTNode *Parser::IdTail(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::PLUS);
  //synch_.push_back(Token::MINUS);
  //synch_.push_back(Token::OR);
  //synch_.push_back(Token::ORELSE);
  //synch_.push_back(Token::MULT);
  //synch_.push_back(Token::DIV);
  //synch_.push_back(Token::MOD);
  //synch_.push_back(Token::AND);
  //synch_.push_back(Token::ANDTHEN);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  if (lookahead_ == Token::LSQR) {
    return transition("var-tail", &Parser::VarTail);
  } else if (lookahead_ == Token::LPAREN) {
    CallNode *node = new CallNode();
    ExpressionNode *arguments = dynamic_cast<ExpressionNode*>(
      transition("call-tail", &Parser::CallTail));
    node->set_argurments(arguments);
    return node;
  } else {
    //SyntaxCheck("id-tail");
  }
  return new VariableNode();
}

ASTNode *Parser::VarTail(/*SynchSet &set*/) {
  //synch_.clear();
  //synch_.push_back(Token::LTEQ);
  //synch_.push_back(Token::LT);
  //synch_.push_back(Token::GT);
  //synch_.push_back(Token::GTEQ);
  //synch_.push_back(Token::EQ);
  //synch_.push_back(Token::NEQ);
  //synch_.push_back(Token::PLUS);
  //synch_.push_back(Token::MINUS);
  //synch_.push_back(Token::OR);
  //synch_.push_back(Token::ORELSE);
  //synch_.push_back(Token::MULT);
  //synch_.push_back(Token::DIV);
  //synch_.push_back(Token::MOD);
  //synch_.push_back(Token::AND);
  //synch_.push_back(Token::ANDTHEN);
  //synch_.push_back(Token::RPAREN);
  //synch_.push_back(Token::RSQR);
  //synch_.push_back(Token::SEMI);
  //synch_.push_back(Token::COMMA);
  VariableNode *variable = new VariableNode();
  ////synch_.push_back(Token::MINUS);
  ////synch_.push_back(Token::NOT);
  ////synch_.push_back(Token::LPAREN);
  ////synch_.push_back(Token::NUM);
  ////synch_.push_back(Token::BLIT);
  ////synch_.push_back(Token::ID);
  //SyntaxCheck("var-tail");
  if (lookahead_ == Token::LSQR) {
    Match(Token::LSQR/*, set*/);
    ExpressionNode *expression = dynamic_cast<ExpressionNode*>(
      transition("add-exp", &Parser::AddExp));
    Match(Token::RSQR/*, set*/);
    variable->set_array_expression(expression);
    variable->set_array_variable(true);
  }
  return variable;
}

ASTNode *Parser::Relop(/*SynchSet &set*/) {
  if (lookahead_ == Token::LTEQ) {
    Match(Token::LTEQ/*, set*/);
    return new BinaryNode(Token::LTEQ);
  } else if (lookahead_ == Token::LT) {
    Match(Token::LT/*, set*/);
    return new BinaryNode(Token::LT);
  } else if (lookahead_ == Token::GT) {
    Match(Token::GT/*, set*/);
    return new BinaryNode(Token::GT);
  } else if (lookahead_ == Token::GTEQ) {
    Match(Token::GTEQ/*, set*/);
    return new BinaryNode(Token::GTEQ);
  } else if (lookahead_ == Token::EQ) {
    Match(Token::EQ/*, set*/);
    return new BinaryNode(Token::EQ);
  } else if (lookahead_ == Token::NEQ) {
    Match(Token::NEQ/*, set*/);
    return new BinaryNode(Token::NEQ);
  } else {
    SyntaxError("Relop"/*, set*/);
    return NULL;
  }
}

ASTNode *Parser::Addop(/*SynchSet &set*/) {
  if (lookahead_ == Token::PLUS) {
    Match(Token::PLUS/*, set*/);
    return new BinaryNode(Token::PLUS);
  } else if (lookahead_ == Token::MINUS) {
    Match(Token::MINUS/*, set*/);
    return new BinaryNode(Token::MINUS);
  } else if (lookahead_ == Token::OR) {
    Match(Token::OR/*, set*/);
    return new BinaryNode(Token::OR);
  } else if (lookahead_ == Token::ORELSE) {
    Match(Token::ORELSE/*, set*/);
    return new BinaryNode(Token::ORELSE);
  } else {
    SyntaxError("Addop"/*, set*/);
    return NULL;
  }
}

ASTNode *Parser::Multop(/*SynchSet &set*/) {
  if (lookahead_ == Token::MULT) {
    Match(Token::MULT/*, set*/);
    return new BinaryNode(Token::MULT);
  } else if (lookahead_ == Token::DIV) {
    Match(Token::DIV/*, set*/);
    return new BinaryNode(Token::DIV);
  } else if (lookahead_ == Token::MOD) {
    Match(Token::MOD/*, set*/);
    return new BinaryNode(Token::MOD);
  } else if (lookahead_ == Token::AND) {
    Match(Token::AND/*, set*/);
    return new BinaryNode(Token::AND);
  } else if (lookahead_ == Token::ANDTHEN) {
    Match(Token::ANDTHEN/*, set*/);
    return new BinaryNode(Token::ANDTHEN);
  } else {
    SyntaxError("Addop"/*, set*/);
    return NULL;
  }
}

ASTNode *Parser::Uminus(/*SynchSet &set*/) {
  if (lookahead_ == Token::MINUS) {
    Match(Token::MINUS/*, set*/);
    return new UnaryNode(Token::MINUS);
  } else {
    SyntaxError("Addop"/*, set*/);
    return NULL;
  }
}
