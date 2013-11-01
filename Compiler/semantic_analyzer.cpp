// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"
#include "ast_node_init_visitor.h"
#include "semantic_analyzer.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode *root, 
                                   const std::string &filename, 
                                   Administrator *administrator) 
  : root_(root), symbol_table_(), administrator_(administrator), 
    filename_(filename) {
  InitSymbolTable();
  error_free_ = true;
}

void SemanticAnalyzer::InitTraversal() {
  root_->Accept(new ASTNodeInitVisitor(&symbol_table_, filename_, administrator_));
  // Make sure "int main(void)" is the last declaration
  const FunctionDeclarationNode *last_declaration = dynamic_cast<const FunctionDeclarationNode*>((symbol_table_.identifier_table_.end()-1)->DecPtr);
  if (last_declaration->type() != Token::INT ||
      Administrator::spelling_table[last_declaration->identifier()].compare("main") != 0 ||
      last_declaration->parameters() != NULL) {
    printf("has parameters\n");
    administrator_->messenger()->AddError(filename_, last_declaration->line_number(), "Last Declaration must be 'int main(void)'");
    error_free_ = false;
  }
  PrintSymbolTable(symbol_table_);
}

void SemanticAnalyzer::FullTraversal() {
  root_->Accept(new ASTNodeFullVisitor(&symbol_table_, filename_, administrator_));
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

void SemanticAnalyzer::PrintSymbolTable(const SymbolTable &table) {
  // Print the identification table
  printf("Symbol Table\nIdentifier Table:\n");
  printf("L\tDecPtr\tNext\tLexI\n");
  for (auto &e : table.identifier_table_) {
    if (e.LexI == -1) {
      printf("%d\t \t%d\t-\tNULL\n", e.L, e.Next);
    } else {
      printf("%d\t \t%d\t%d\t%s\n", e.L, e.Next, e.LexI, Administrator::spelling_table[e.LexI].c_str());
    }
  }

  printf("\nAccess Table:\n");

  int count = 0;
  for (auto &e : table.acces_table_) {
    printf("%d\t%s\n", e, Administrator::spelling_table[count++].c_str());
  }
}