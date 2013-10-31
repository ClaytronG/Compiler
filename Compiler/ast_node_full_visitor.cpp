// Author: Clayton Green (kgreen1@unbc.ca)

#include "ast_node_full_visitor.h"

ASTNodeFullVisitor::ASTNodeFullVisitor(SymbolTable *symbol_table)
  : symbol_table_(symbol_table) {
  depth_ = 0;
}

void ASTNodeFullVisitor::Visit(const AssignmentNode &node) { }
void ASTNodeFullVisitor::Visit(const ASTNode &node) { }
void ASTNodeFullVisitor::Visit(const BinaryNode &node) { }
void ASTNodeFullVisitor::Visit(const BranchNode &node) { }
void ASTNodeFullVisitor::Visit(const CallNode &node) { }
void ASTNodeFullVisitor::Visit(const CaseNode &node) { }
void ASTNodeFullVisitor::Visit(const CompoundNode &node) { }
void ASTNodeFullVisitor::Visit(const ContinueNode &node) { }
void ASTNodeFullVisitor::Visit(const DeclarationNode &node) { }
void ASTNodeFullVisitor::Visit(const ExitNode &node) { }
void ASTNodeFullVisitor::Visit(const ExpressionNode &node) { }
void ASTNodeFullVisitor::Visit(const FunctionDeclarationNode &node) { }
void ASTNodeFullVisitor::Visit(const IfNode &node) { }
void ASTNodeFullVisitor::Visit(const LiteralNode &node) { }
void ASTNodeFullVisitor::Visit(const LoopNode &node) { }
void ASTNodeFullVisitor::Visit(const NullNode &node) { }
void ASTNodeFullVisitor::Visit(const ParameterNode &node) { }
void ASTNodeFullVisitor::Visit(const ProgramNode &node) { }
void ASTNodeFullVisitor::Visit(const ReturnNode &node) { }
void ASTNodeFullVisitor::Visit(const StatementNode &node) { }
void ASTNodeFullVisitor::Visit(const UnaryNode &node) { }
void ASTNodeFullVisitor::Visit(const VariableDeclarationNode &node) { }
void ASTNodeFullVisitor::Visit(const VariableNode &node) { }