// Author: Clayton Green (kgreen1@unbc.ca)

#include "symbol_table.h"

SymbolTable::SymbolTable() : access_table_(Administrator::spelling_table.size()), identification_table_() {
  access_table_.reserve(Administrator::spelling_table.size());
  InitIdentificationTable();
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

void SymbolTable::AccessTableAddElementAt(const unsigned int index, const int value) {
  if (index > access_table_.size()) {
    return;
  }
  access_table_.at(index) = value;
}

void SymbolTable::PushBack(
    const SymbolTable::IdentificationTableEntry &entry) {
  identification_table_.push_back(entry);
}

void SymbolTable::PrintTables() {
  printf("\nL\tDecPtr\tNext\tLexI\n");
  for (auto& e : identification_table_) {
    if (e.LexI > -1) {
      printf("%d\t-\t%d\t%d\t%s\n", e.L, e.Next, e.LexI, Administrator::spelling_table[e.LexI].c_str());
    } else 
      printf("%d\t-\t%d\t%d\tNULL\n", e.L, e.Next, e.LexI);
  }
  printf("\n");
  int count = 0;
  for (auto& e : access_table_) {
    printf("%d\t%s\n", e, Administrator::spelling_table[count++].c_str());
  }
}

SymbolTable::IdentificationTableEntry SymbolTable::IdentificationTableAt(const unsigned int index) {
  return identification_table_.at(index);
}

int SymbolTable::IdentificationTableSize() const {
  return identification_table_.size();
}