// Author: Clayton Green (kgreen1@unbc.ca)

// Visual Studio complains about using fopen() because it is deprecated and 
// suggests using fopen_s() but G++ only has fopen().
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ast_node_full_visitor.h"
#include "ast_node_init_visitor.h"
#include "semantic_analyzer.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode *root, 
                                   const std::string &filename, 
                                   Administrator *administrator) 
  : root_(root), symbol_table_(), administrator_(administrator), 
    filename_(filename) {
  error_free_ = true;
}

void SemanticAnalyzer::InitTraversal() {
  ASTNodeInitVisitor *visitor = new ASTNodeInitVisitor(&symbol_table_, filename_, administrator_);
  root_->Accept(visitor);
  error_free_ = visitor->error_free();
  // Make sure "int main(void)" is the last declaration
  const FunctionDeclarationNode *last_declaration = dynamic_cast<const FunctionDeclarationNode*>((symbol_table_.identifier_table_.end()-1)->DecPtr);
  if (last_declaration == NULL) {
    administrator_->messenger()->AddError(filename_, 0, "Last Declaration must be 'int main(void)'");
    error_free_ = false;
  } else if (last_declaration->type() != Token::INT ||
      Administrator::spelling_table[last_declaration->identifier()].compare("main") != 0 ||
      last_declaration->parameters() != NULL) {
    administrator_->messenger()->AddError(filename_, last_declaration->line_number(), "Last Declaration must be 'int main(void)'");
    error_free_ = false;
  }
  administrator_->messenger()->PrintMessage(symbol_table_.ToString());
}

void SemanticAnalyzer::FullTraversal() {
  ASTNodeFullVisitor *visitor = new ASTNodeFullVisitor(&symbol_table_, filename_, administrator_);
  root_->Accept(visitor);
  error_free_ = visitor->error_free();
}

SemanticAnalyzer::SymbolTable::SymbolTable() : acces_table_(), identifier_table_() {
  InitSymbolTable();
}

void SemanticAnalyzer::SymbolTable::InitSymbolTable() {
  acces_table_.reserve(Administrator::spelling_table.size());
  acces_table_.assign(Administrator::spelling_table.size(), 0);
  SymbolTable::IdentificationTableEntry entry;
  entry.L = 0;
  entry.DecPtr = NULL;
  entry.Next = 0;
  entry.LexI = -1;
  identifier_table_.push_back(entry);
}

std::string SemanticAnalyzer::SymbolTable::ToString() const {
  std::string table = "Symbol Table\nIdentifier Table:\nIdI: L  DecPtr Next LexI Identifier:\n";
  int count = 0;
  auto it = identifier_table_.begin();
  const auto end = identifier_table_.end();
  char buf[50];
  while (it != end) {
    if (it->LexI == -1) {
      sprintf(buf, "%-5i%-3i%-7s%-5i%-5i%-12s\n", count++, it->L, "  -", it->Next, -1, "NULL");
      table += buf;
    } else {
      sprintf(buf, "%-5i%-3i%-7s%-5i%-5i%-12s\n", count++, it->L, "  -", it->Next, it->LexI, Administrator::spelling_table[it->LexI].c_str());
      table += buf;
    }
    ++it;
  }

  table += "\nAccess Table:\nIdentifier: LexI: IdI\n";
  count = 0;
  for (unsigned int i = 0; i < acces_table_.size(); ++i) {
    sprintf(buf, "%-12s%-6i%-4i\n", Administrator::spelling_table[i].c_str(), i, acces_table_.at(i));
    table += buf;
  }
  return table;
}
