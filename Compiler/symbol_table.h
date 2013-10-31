// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SYMBOLTABLE_H_
#define COMPILER_SYMBOLTABLE_H_

#include <array>
#include <string>
#include <vector>

#include "ast_node.h"

class SymbolTable {
public:

  struct IdentificationTableEntry {
    int L;                          // Block Level
    const DeclarationNode *DecPtr;  // Pointer to declaration node in the AST
    int Next;                       // Next closest declaration
    int LexI;                       // Spelling table entry
  };

  // 
  SymbolTable();

  //
  static IdentificationTableEntry CreateIdentificationTableEntry(int l, DeclarationNode *dec_ptr,
                                                                 int next, int lex_i);

  //
  void PopBlock();

  //
  void PushBack(const IdentificationTableEntry &entry);

  //
  std::string ToString();
                                                          
private:

  //
  void InitAccessTable();

  //
  void InitIdentificationTable();

  //
  std::array<int, 20> access_table_;

  //
  std::vector<IdentificationTableEntry> identification_table_;

};

#endif // COMPILER_SYMBOLTABLE_H_