// Author: Clayton Green (kgreen1@unbc.ca)

#include "symbol_table.h"

SymbolTable::SymbolTable() : access_table_(), identification_table_() {
  InitAccessTable();
  InitIdentificationTable();
}

void SymbolTable::InitAccessTable() {
  for (auto& e : access_table_) {
    e = 0;
  }
}

void SymbolTable::InitIdentificationTable() {
  IdentificationTableEntry null_entry = CreateIdentificationTableEntry(0, NULL, 0, -1);
  identification_table_.push_back(null_entry);
}

SymbolTable::IdentificationTableEntry SymbolTable::CreateIdentificationTableEntry(
  int l, DeclarationNode *dec_ptr, int next, int lex_i) 
{
  IdentificationTableEntry return_entry;
  return_entry.L = l;
  return_entry.DecPtr = dec_ptr;
  return_entry.Next = next;
  return_entry.LexI = lex_i;
  return return_entry;
}

void SymbolTable::PopBlock() {
  auto it = identification_table_.end();
  int level = it->L;
  const auto begin = identification_table_.begin();
  while (it != begin && it->L == level) {
    identification_table_.pop_back();
    --it;
  }
}

void SymbolTable::PushBack(
    const SymbolTable::IdentificationTableEntry &entry) {
  identification_table_.push_back(entry);
}

std::string SymbolTable::ToString() {
  std::string temp = "\nL\tDecPtr\tNext\tLexI\n";
  printf("%s", temp.c_str());
  for (auto& e : identification_table_) {
    if (true) {
      printf("%d\t-\t%d\t%d\n", e.L, e.Next, e.LexI);
    } //else 
      //printf("%d\t-\t%d\t%s\n", e.L, e.Next, Administrator::spelling_table[e.LexI].c_str());
  }
  return temp;
}