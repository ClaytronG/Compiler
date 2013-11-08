// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SEMANTICANALYZER_H_
#define COMPILER_SEMANTICANALYZER_H_

#include <string>

#include "administrator.h"
#include "ast_node.h"

class SemanticAnalyzer {
public:

  class SymbolTable {
  public:
    struct IdentificationTableEntry {
      int L;                          // Block Level
      ASTNode *DecPtr;                // Pointer to declaration node in the AST
      int Next;                       // Next closest declaration
      int LexI;                       // Spelling table entry
    };

    SymbolTable();

    std::string ToString() const;

    std::vector<int> acces_table_;
    std::vector<IdentificationTableEntry> identifier_table_;

  private:
    //
    void InitSymbolTable();
  };

  SemanticAnalyzer(ASTNode *root, const std::string &filename, Administrator *administrator);

  void InitTraversal();

  void FullTraversal();

private:

  // 
  bool error_free_;

  //
  ASTNode *root_;

  //
  SymbolTable symbol_table_;

  //
  Administrator *administrator_;

  //
  std::string filename_;

};

#endif // COMPILER_SEMANTICANALYZER_H_