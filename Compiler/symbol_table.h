// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SYMBOLTABLE_H_
#define COMPILER_SYMBOLTABLE_H_

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
  static IdentificationTableEntry CreateIdentificationTableEntry(
    int l, 
    DeclarationNode *dec_ptr,
    int next, 
    int lex_i);

  //
  void PopBlock();

  void AccessTableAddElementAt(const unsigned int index, const int value);

  //
  void PushBack(const IdentificationTableEntry &entry);

  IdentificationTableEntry IdentificationTableAt(const unsigned int index);

  //
  void PrintTables();

  //
  int IdentificationTableSize() const;
                                                          
private:

  //
  void InitAccessTable();

  //
  void InitIdentificationTable();

  //
  std::vector<int> access_table_;

  //
  std::vector<IdentificationTableEntry> identification_table_;

};

#endif // COMPILER_SYMBOLTABLE_H_