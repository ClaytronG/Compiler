// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SEMANTICANALYZER_H_
#define COMPILER_SEMANTICANALYZER_H_

#include "ast_node.h"

class SemanticAnalyzer {
public:

  struct SymbolTable {
    struct IdentificationTableEntry {
      int L;                          // Block Level
      const ASTNode *DecPtr;  // Pointer to declaration node in the AST
      int Next;                       // Next closest declaration
      int LexI;                       // Spelling table entry
    };

    std::vector<int> acces_table_;
    std::vector<IdentificationTableEntry> identifier_table_;
  };

  SemanticAnalyzer(ASTNode *root);

  void InitTraversal();

  void FullTraversal();

  void PrintSymbolTable() const;

private:
  //
  void InitSymbolTable();

  // 
  bool error_free_;

  //
  ASTNode *root_;

  //
  SymbolTable symbol_table_;

};

#endif // COMPILER_SEMANTICANALYZER_H_