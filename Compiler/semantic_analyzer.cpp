// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"
#include "ast_node_init_visitor.h"
#include "semantic_analyzer.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode *root) : root_(root), symbol_table_() {
  InitSymbolTable();
  error_free_ = true;
}

void SemanticAnalyzer::InitTraversal() {
  root_->Accept(new ASTNodeInitVisitor(&symbol_table_));
  // Make sure void main is the last declaration
  PrintSymbolTable();
}

void SemanticAnalyzer::FullTraversal() {
  root_->Accept(new ASTNodeFullVisitor(&symbol_table_));
}

void SemanticAnalyzer::InitSymbolTable() {
  symbol_table_.acces_table_.reserve(Administrator::spelling_table.size());
  symbol_table_.acces_table_.assign(Administrator::spelling_table.size(), 0);
  SymbolTable::IdentificationTableEntry entry;
  entry.L = 0;
  entry.DecPtr = NULL;
  entry.Next = 0;
  entry.LexI = -1;
  symbol_table_.identifier_table_.push_back(entry);
}

void SemanticAnalyzer::PrintSymbolTable() const {
  // Print the identification table
  printf("L\tDecPtr\tNext\tLexI\n");
  for (auto &e : symbol_table_.identifier_table_) {
    if (e.LexI == -1) {
      printf("%d\t \t%d\t-\t%s\n", e.L, e.Next, e.LexI);
    } else {
      printf("%d\t \t%d\t%d\t%s\n", e.L, e.Next, e.LexI, Administrator::spelling_table[e.LexI]);
    }
  }

  printf("\n");

  int count = 0;
  for (auto &e : symbol_table_.acces_table_) {
    printf("%d\t%s\n", e, Administrator::spelling_table[count++]);
  }
}