#ifndef COMPILER_CODEGENERATOR_H_
#define COMPILER_CODEGENERATOR_H_

#include <string>

#include "administrator.h"
#include "ast_node.h"

class CodeGenerator {
public:
  // 
  CodeGenerator(ASTNode *root, const std::string filename, Administrator *administrator);

  //
  std::string output() const;

  //
  void GenerateCode();

private:
  // Adds the standard preamble to the output
  void InitOutput();

  //
  Administrator *administrator_;

  //
  bool error_free_;

  //
  std::string filename_;

  //
  std::string output_;

  //
  ASTNode *root_;
};

#endif // COMPILER_CODEGENERATOR_H_