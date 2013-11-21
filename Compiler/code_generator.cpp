#include "code_generator.h"

CodeGenerator::CodeGenerator(ASTNode *root, const std::string filename, Administrator *administrator) 
  : root_(root), administrator_(administrator), output_() {
  error_free_ = true;
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
        count += node->array_size();
      }
      else {
        ++count;
      }
      if (node->next_node() != NULL) {
        current_node = dynamic_cast<VariableDeclarationNode*>(node->next_node());
      }
      else {
        current_node = current_node->next_node();
      }
    }
    else {
      current_node = current_node->next_node();
    }
  }
  std::string quadruple = "(start,";
  quadruple += std::to_string(count);
  quadruple += ",-,-)\n";
  output_ += quadruple;
  output_ += "(rval,-,-,t1)\n";
  output_ += "(call,main,-,-)\n";
  output_ += "(hlt,-,-,-)";
}

std::string CodeGenerator::output() const {
  return output_;
}

void CodeGenerator::GenerateCode() {

}