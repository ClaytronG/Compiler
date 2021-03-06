// Author: Clayton Green (kgreen1@unbc.ca)

// Visual Studio complains about using fopen() because it is deprecated and 
// suggests using fopen_s() but G++ only has fopen().
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <map>

#include "administrator.h"
#include "ast_node.h"
#include "ast_node_print_visitor.h"
#include "code_generator.h"
#include "messenger.h"
#include "semantic_analyzer.h"

const std::string Administrator::kBuiltInFunctions = 
  "int readint(void) {return 0;}\nvoid writeint(int outint) {;}\nbool readbool(void) {return true;}\nvoid writebool(bool outbool) {;}\n";

Administrator::Administrator(const std::vector<std::string> file_list)
  : input_file_list_(file_list), messenger_(), show_trace_(false), output_filename_(), output_file_(stdout) {
}

Administrator::Administrator(const std::vector<std::string> file_list,
  const std::string &error_file)
  : input_file_list_(file_list), messenger_(error_file), show_trace_(false), output_filename_(), output_file_(stdout) {
}

bool Administrator::Compile() {
  auto it = input_file_list_.begin();
  const auto end = input_file_list_.end();
  while (it != end) {
    // Get the AST for the built in functions
    // disable trace
    bool trace = show_trace_;
    std::string filename = (*it).substr((*it).find_last_of("/") + 1, (*it).length());
    Parser parser(*it, filename, this);
    if (!parser.good()) {
      fprintf(stderr, "Error parsing file: %s\n", filename.c_str());
      ++it;
      continue;
    }
    messenger_.set_show_trace(false);
    Parser p(kBuiltInFunctions, this);
    messenger_.set_show_trace(trace);
    ASTNode *new_root = p.Parse();
    ASTNode *root = parser.Parse();
    if (!root) {
      // If the root node is NULL then the parser encountered errors. Print
      // those errors and continue on to the next source file.
      messenger_.PrintErrors();
      ++it;
      continue;
    }
    else {
      // Append the original root not to the end of the new_root
      // Get to the last declaration in new_root
      DeclarationNode *node = dynamic_cast<ProgramNode*>(new_root)->declaration_node();
      while (node->next_node()) {
        node = dynamic_cast<DeclarationNode*>(node->next_node());
      }
      DeclarationNode *next = dynamic_cast<ProgramNode*>(root)->declaration_node();
      node->set_next_node(next);
      // Begin semantic analysis
      SemanticAnalyzer sem(new_root, filename, this);
      sem.InitTraversal();
      sem.FullTraversal();
      if (!sem.error_free()) {
        messenger_.PrintErrors();
        ++it;
        continue;
      }
      // Remove the system calls from the AST (read/write int/bool)
      node = dynamic_cast<ProgramNode*>(new_root)->declaration_node();
      while (node->identifier() < 5) {
        node = dynamic_cast<DeclarationNode*>(node->next_node());
      }
      // Generate the quadruples
      CodeGenerator gen(new ProgramNode(node), filename, this);
      gen.GenerateCode();
      // Create the output file
      if (!output_filename_.empty()) {
        output_file_ = fopen(output_filename_.c_str(), "w");
        fprintf(output_file_, "%s", gen.output().c_str());
      }
      std::string message = "\n" + gen.output();
      messenger_.PrintMessage(message);
    }
    messenger_.PrintErrors();
    ++it;
  }

  return true;
}

void Administrator::set_output_file(const std::string &file) {
  output_filename_ = file;
}

Administrator::SpellingTable Administrator::CreateSpellingTable() {
  SpellingTable table;
  return table;
}

Administrator::WordMap Administrator::CreateWordMap() {
	Administrator::WordMap map;
	// Add all of the reserved words that have no values (if, and, or, etc.)
  // Entries are of the form: (lexeme, (TokenName, value)).
  for (int i = 0; i < Token::ID; ++i) {
    auto value = std::make_pair(Token::TokenName(i), -1);
    auto pair = std::make_pair(reserved_words_list[i], value);
		map.insert(pair);
	}
	// Add true and false with their respective values, 1 and 0
  auto true_token = std::make_pair("true", std::make_pair(Token::BLIT, 1));
  auto false_token = std::make_pair("false", std::make_pair(Token::BLIT, 0));
  map.insert(true_token);
  map.insert(false_token);

  return map;
}

int Administrator::GetSpellingTableIndex(const std::string &key) {
#pragma warning(disable:4018)  // Unsigned int compared to signed int
  for (int index = 0; index < spelling_table.size(); ++index) {
    if (spelling_table[index] == key) {
      return index;
    }
  }
#pragma warning(default:4018)
  return -1;
}

Administrator::WordMap::const_iterator Administrator::GetWord(
    const std::string &key) {
  auto it = word_map.find(key);
  if (it == word_map.end()) {
    // If the key wasn't found, put it on the spelling table
    spelling_table.push_back(key);
    auto value = std::make_pair(Token::ID, num_spelling_table_entries);
    auto pair = std::make_pair(key, value);
    word_map.insert(pair);
    last_spelling_table_entry = num_spelling_table_entries++;
    return word_map.find(key);
  }
  return it;
}

std::string Administrator::IntToString(const int &num) {
  // Have to use static_cast<long long> because of a bug with gcc-4.3.4
  return std::to_string(static_cast<long long>(num));
}

bool Administrator::LexerPhase() {
  bool error_free = true;
#pragma warning(disable:4018)
  auto it = input_file_list_.begin();
  const auto end = input_file_list_.end();
  while (it != end) {
    std::string filename = (*it).substr((*it).find_last_of("/") + 1, (*it).length());
    Scanner scanner(*it, filename, this);
    if (!scanner.good()) {
      fprintf(stdout, "Error opening %s\n", filename.c_str());
      ++it;
      continue;
    }

    Token token = scanner.GetToken();
    std::string line_number, message;
    while (token.name() != Token::ENDFILE) {
      line_number = IntToString(scanner.line_number());
      message = std::string(line_number.size() + filename.size() + 2, ' ');
      message += line_number + "> " + token.toString() + "\n";
      messenger_.PrintMessage(message);
		  token = scanner.GetToken();
      if (token.name() == Token::ERROR) error_free = false;
	  }
    // Print the ENDFILE token
    message = std::string(filename.size() + line_number.size() + 2, ' ');
    message += line_number + "> " + token.toString() + "\n";
    messenger_.PrintMessage(message);
    // ---
    messenger_.PrintErrors();
    ++it;
  }
#pragma warning(default:4018)
  return error_free;
}

bool Administrator::ParserPhase() {
#pragma warning(disable:4018)
  auto it = input_file_list_.begin();
  const auto end = input_file_list_.end();
  while (it != end) {
    std::string filename = (*it).substr((*it).find_last_of("/") + 1, (*it).length());
    Parser parser(*it, filename, this);
    ASTNode *root = NULL;
    if (!parser.good()) {
      fprintf(stderr, "Error parsing %s\n", filename.c_str());
      ++it;
      continue;
    } else {
      root = parser.Parse();
      if (root && show_trace_) {
        messenger_.PrintMessage("\nAbstract Syntax Tree\n");
        root->Accept(new ASTNodePrintVisitor(&messenger_));
      }
    }
    messenger_.PrintErrors();
    ++it;
  }
#pragma warning(default:4018)
  return true;
}

bool Administrator::SemanticAnalysisPhase() {
  // Run the parser to generate an AST
  auto it = input_file_list_.begin();
  const auto end = input_file_list_.end();
  while (it != end) {
    std::string filename = (*it).substr((*it).find_last_of("/")+1, (*it).length());
    Parser parser(*it, filename, this);
    if (!parser.good()) {
      fprintf(stderr, "Error parsing file: %s\n", filename.c_str());
      ++it;
      continue;
    }
    bool trace = show_trace_;
    messenger_.set_show_trace(false);
    // Get the AST for the built in functions
    // disable trace
    Parser p(kBuiltInFunctions, this);
    ASTNode *new_root = p.Parse();
    ASTNode *root = parser.Parse();
    if (!root) {
      // If the root node is NULL then the parser encountered errors. Print
      // those errors and continue on to the next source file.
      messenger_.PrintErrors();
      ++it;
      continue;
    } else {
      // Append the original root not to the end of the new_root
      // Get to the last declaration in new_root
	    DeclarationNode *node = dynamic_cast<ProgramNode*>(new_root)->declaration_node();
	    while (node->next_node()) {
		    node = dynamic_cast<DeclarationNode*>(node->next_node());
	    }
	    DeclarationNode *next = dynamic_cast<ProgramNode*>(root)->declaration_node();
      node->set_next_node(next);
      // Begin semantic analysis
      messenger_.set_show_trace(trace);
      SemanticAnalyzer sem(new_root, filename, this);
      sem.InitTraversal();
      sem.FullTraversal();
    }
    messenger_.PrintErrors();
    ++it;
  }
  return true;
}

bool Administrator::TupleGenerationPhase() {
  auto it = input_file_list_.begin();
  const auto end = input_file_list_.end();
  while (it != end) {
    // Get the AST for the built in functions
    // disable trace
    bool trace = show_trace_;
    messenger_.set_show_trace(false);
    std::string filename = (*it).substr((*it).find_last_of("/") + 1, (*it).length());
    Parser parser(*it, filename, this);
    if (!parser.good()) {
      fprintf(stderr, "Error parsing file: %s\n", filename.c_str());
      ++it;
      continue;
    }
    Parser p(kBuiltInFunctions, this);
    ASTNode *new_root = p.Parse();
    ASTNode *root = parser.Parse();
    if (!root) {
      // If the root node is NULL then the parser encountered errors. Print
      // those errors and continue on to the next source file.
      messenger_.PrintErrors();
      ++it;
      continue;
    }
    else {
      // Append the original root not to the end of the new_root
      // Get to the last declaration in new_root
      DeclarationNode *node = dynamic_cast<ProgramNode*>(new_root)->declaration_node();
      while (node->next_node()) {
        node = dynamic_cast<DeclarationNode*>(node->next_node());
      }
      DeclarationNode *next = dynamic_cast<ProgramNode*>(root)->declaration_node();
      node->set_next_node(next);
      // Begin semantic analysis
      SemanticAnalyzer sem(new_root, filename, this);
      sem.InitTraversal();
      sem.FullTraversal();
      if (!sem.error_free()) {
        messenger_.PrintErrors();
        ++it;
        continue;
      }
      // Remove the system calls from the AST (read/write int/bool)
      node = dynamic_cast<ProgramNode*>(new_root)->declaration_node();
      while (node->identifier() < 5) {
        node = dynamic_cast<DeclarationNode*>(node->next_node());
      }
      // Generate the quadruples
      messenger_.set_show_trace(trace);
      CodeGenerator gen(new ProgramNode(node), filename, this);
      gen.GenerateCode();
      messenger_.PrintMessage(gen.output());
    }
    messenger_.PrintErrors();
    ++it;
  }
  return true;
}

int Administrator::last_spelling_table_entry = 0;

int Administrator::num_spelling_table_entries = 0;

std::string Administrator::reserved_words_list[] = {
	"and",
	"bool",
	"branch",
	"case",
	"continue",
	"default",
	"else",
	"end",
	"exit",
	"if",
	"int",
	"loop",
	"mod",
  "not",
	"or",
	"ref",
	"return",
	"void",
};

Administrator::SpellingTable Administrator::spelling_table = 
  Administrator::CreateSpellingTable();

Administrator::WordMap Administrator::word_map = 
  Administrator::CreateWordMap();
