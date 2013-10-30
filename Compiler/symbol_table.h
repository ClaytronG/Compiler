// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SYMBOLTABLE_H_
#define COMPILER_SYMBOLTABLE_H_

#include "ast_node.h"

class SymbolTable {
public:

  struct IdentificationTableEntry {
    int IdI;                  // 
    int L;                    // Block Level
    DeclarationNode *DecPtr;  // Pointer to declaration node in the AST
    int next;                 // 
    int LexI;                 //
  };

  struct AccessTableEntry {
    int LexI;
    int IdI;
  };

private:

};

#endif // COMPILER_SYMBOLTABLE_H_