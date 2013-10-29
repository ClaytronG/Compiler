// Author: Clayton Green (kgreen1@unbc.ca)

#include <iostream>
#include <map>

#include "administrator.h"
#include "ast_node.h"
#include "ast_node_print_visitor.h"
#include "messenger.h"

Administrator::Administrator(const std::vector<std::string> file_list)
  : input_file_list_(file_list), messenger_(), show_trace_(false) {
}

Administrator::Administrator(const std::vector<std::string> file_list,
                             const std::string &error_file)
  : input_file_list_(file_list), messenger_(error_file), show_trace_(false) {
}

bool Administrator::Compile() {
  // TODO: Finish all compilation stages
  return false;
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
  for (int i = 0; i < input_file_list_.size(); ++i) {
    // TODO: get filename from path
    std::string filename = "filename";
    Scanner scanner(input_file_list_.at(i), filename, *this);
    if (!scanner.good()) {
      fprintf(stdout, "Error opening %s\n", filename.c_str());
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

    messenger_.PrintErrors();
  }
#pragma warning(default:4018)
  return error_free;
}

bool Administrator::ParserPhase() {
#pragma warning(disable:4018)
  for (int i = 0; i < input_file_list_.size(); ++i) {
    // TODO: get filename from file path
    std::string filename = "filename";
    Parser parser(input_file_list_.at(i), filename, this);
    ASTNode *root = NULL;
    if (!parser.good()) {
      fprintf(stderr, "Error parsing %s\n", filename.c_str());
    } else {
      root = parser.Parse();
    }
    if (show_trace_ && parser.error_free()) {
      messenger_.PrintMessage("\nAbstract Syntax Tree\n");
      root->Accept(new ASTNodePrintVisitor(&messenger_));
    }
    messenger_.PrintErrors();
  }
#pragma warning(default:4018)
  return true;
}

bool Administrator::SemanticAnalysisPhase() {
  // TODO: implement semantic analyser.
  return false;
}

bool Administrator::TupleGenerationPhase() {
  // TODO: implement tuple generation
  return false;
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
