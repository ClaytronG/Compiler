#include "code_generator.h"

CodeGenerator::CodeGenerator(ASTNode *root, const std::string filename, Administrator *administrator) 
  : root_(root), administrator_(administrator), output_() {
  error_free_ = true;
  InitAST();
  InitOutput();
}

void CodeGenerator::InitOutput() {
  int count = 0;
  // Count how many global variable delcarations there are
  ASTNode* current_node = dynamic_cast<ProgramNode*>(root_)->declaration_node();
  while (current_node != NULL) {
    VariableDeclarationNode* node = dynamic_cast<VariableDeclarationNode*>(current_node);
    if (node != NULL) {
      if (node->array_expression() != NULL) {
        count += node->array_expression()->value();
      }
      else {
        ++count;
      }
      printf("count = %d\n", count);
    }
    current_node = current_node->next_node();
  }
}

void CodeGenerator::InitAST() {

}