// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SEMANTICANALYZER_H_
#define COMPILER_SEMANTICANALYZER_H_

#include "ast_node.h"
#include "symbol_table.h"

class SemanticAnalyzer {
public:

  SemanticAnalyzer(ASTNode *root);

  void InitTraversal();

  void FullTraversal();

private:
  // 
  bool error_free_;

  //
  ASTNode *root_;

  //
  SymbolTable symbol_table_;

};

#endif // COMPILER_SEMANTICANALYZER_H_