// Author: Clayton Green (kgreen1@unbc.ca)

#ifndef COMPILER_SEMANTICANALYZER_H_
#define COMPILER_SEMANTICANALYZER_H_

class SemanticAnalyzer {
public:

  SemanticAnalyzer();

  void InitTraversal();

  void FullTraversal();

private:
  // 
  bool error_free_;

};

#endif // COMPILER_SEMANTICANALYZER_H_