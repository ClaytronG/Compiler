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
  if (last_declaration == NULL) {
    administrator_->messenger()->AddError(filename_, 0, "Last Declaration must be 'int main(void)'");
  } else if (last_declaration->type() != Token::INT ||
      Administrator::spelling_table[last_declaration->identifier()].compare("main") != 0 ||
      last_declaration->parameters() != NULL) {
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
  printf("IdI: L  DecPtr Next LexI Identifier:\n");
  int count = 0;
  auto it = table.identifier_table_.begin();
  const auto end = table.identifier_table_.end();
  while (it != end) {
    if (it->LexI == -1) {
      printf("%-5i%-3i%-7s%-5i%-5i%-12s\n", count++, it->L, "  -", it->Next, -1, "NULL");
    } else {
      printf("%-5i%-3i%-7s%-5i%-5i%-12s\n", count++, it->L, "  -", it->Next, it->LexI, Administrator::spelling_table[it->LexI].c_str());
    }
    ++it;
  }

  printf("\nAccess Table:\n");
  printf("Identifier: LexI: IdI\n");
  count = 0;
  for (unsigned int i = 0; i < table.acces_table_.size(); ++i) {
    printf("%-12s%-6i%-4i\n", Administrator::spelling_table[i].c_str(), i, table.acces_table_.at(i));
  }
}
