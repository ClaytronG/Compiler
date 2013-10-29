// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#ifndef KOMPILER_PARSER_H_
#define KOMPILER_PARSER_H_

#include <set>
#include <string>
#include <vector>

#include "scanner.h"
#include "token.h"

class ASTNode;
class Administrator;

// 
class Parser {
public:
  typedef std::vector<Token::TokenName> SynchSet;
  typedef ASTNode* (Parser::*ParserFunction)(void);

  Parser(const std::string &file, const std::string &filename, 
         Administrator *administrator);
  
  // 
  inline int depth() const {
    return depth_;
  }

  // 
  inline std::string filename() const {
    return filename_;
  }

  // 
  inline bool good() const {
    return good_;
  }
  
  // 
  ASTNode *Parse();

  // 
  inline void set_depth(int depth) {
    depth_ = depth;
  }

  //
  inline Administrator *administrator() const {
    return administrator_;
  }

private:    //
  ASTNode *transition(const std::string &function_name,
                      ParserFunction function);

  ASTNode *Program(/*SynchSet &set*/);
  ASTNode *Declaration(/*SynchSet &set*/);
  Token::TokenName NonvoidSpecifier(/*SynchSet &set*/);
  ASTNode *DecTail(/*SynchSet &set*/);
  ASTNode *VarDecTail(/*SynchSet &set*/);
  ASTNode *VarName(/*SynchSet &set*/);
  ASTNode *FunDecTail(/*SynchSet &set*/);
  ASTNode *Params(/*SynchSet &set*/);
  ASTNode *Param(/*SynchSet &set*/);
  ASTNode *Statement(/*SynchSet &set*/);
  ASTNode *IdStmt(/*SynchSet &set*/);
  ASTNode *IdStmtTail(/*SynchSet &set*/);
  ASTNode *AssignStmtTail(/*SynchSet &set*/);
  ASTNode *CallStmtTail(/*SynchSet &set*/);
  ASTNode *CallTail(/*SynchSet &set*/);
  ASTNode *Arguments(/*SynchSet &set*/);
  ASTNode *CompoundStmt(/*SynchSet &set*/);
  ASTNode *IfStmt(/*SynchSet &set*/);
  ASTNode *LoopStmt(/*SynchSet &set*/);
  ASTNode *ExitStmt(/*SynchSet &set*/);
  ASTNode *ContinueStmt(/*SynchSet &set*/);
  ASTNode *ReturnStmt(/*SynchSet &set*/);
  ASTNode *NullStmt(/*SynchSet &set*/);
  ASTNode *BranchStmt(/*SynchSet &set*/);
  ASTNode *CaseStmt(/*SynchSet &set*/);
  ASTNode *Expression(/*SynchSet &set*/);
  ASTNode *AddExp(/*SynchSet &set*/);
  ASTNode *Term(/*SynchSet &set*/);
  ASTNode *Factor(/*SynchSet &set*/);
  ASTNode *NidFactor(/*SynchSet &set*/);
  ASTNode *IdFactor(/*SynchSet &set*/);
  ASTNode *IdTail(/*SynchSet &set*/);
  ASTNode *VarTail(/*SynchSet &set*/);
  ASTNode *Relop(/*SynchSet &set*/);
  ASTNode *Addop(/*SynchSet &set*/);
  ASTNode *Multop(/*SynchSet &set*/);
  ASTNode *Uminus(/*SynchSet &set*/);

  void Match(const Token::TokenName &expected/*, SynchSet &synch*/);
  void SyntaxError(const std::string &message/*, SynchSet &synch*/);
  void SyntaxCheck(const std::string &message/*, SynchSet &synch*/);
  
  // Pointer to the calling administrator object. Used to access the messenger.
  Administrator *administrator_;

  // How deep, number of functions, the recursive parser is. Used for 
  // formatting Trace messages.
  int depth_;

  // 
  const std::string filename_;
  
  //
  SynchSet synch_;

  //
  bool good_;

  // TokenName representation of the lookahead token. Used in checking first
  // sets.
  Token::TokenName lookahead_;

  // Actual token object of lookahead.
  Token lookahead_token_;

  //
  Scanner scanner_;

  //DISALLOW_COPY_AND_ASSIGN(Parser);
};

#endif // KOMPILER_PARSER_H_
