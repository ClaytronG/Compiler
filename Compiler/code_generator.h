#ifndef COMPILER_CODEGENERATOR_H_
#define COMPILER_CODEGENERATOR_H_

#include <string>

#include "administrator.h"
#include "ast_node.h"

class CodeGenerator {
public:
  CodeGenerator(ASTNode *root, const std::string filename, Administrator *administrator);

private:

  void InitOutput();

  // Updates the syntax tree so all declarations have scope level and displacement values
  void InitAST();

  Administrator *administrator_;

  bool error_free_;

  std::string filename_;

  std::string output_;

  ASTNode *root_;
};

#endif // COMPILER_CODEGENERATOR_H_