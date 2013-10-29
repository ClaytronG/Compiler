// Author: Clayton Green (kgreen1@unbc.ca)
// Last Updated: September 19, 2013
//
// The Administrator runs the compiler and holds the global data structures
// like the word map and the spelling table.

#ifndef	KOMPILER_ADMINISTRATOR_H_
#define KOMPILER_ADMINISTRATOR_H_

#ifdef __GNUC__
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif 

#include <vector>

#include "messenger.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"

//
class Administrator {
 public:
  typedef std::tr1::unordered_map<std::string, std::pair<Token::TokenName, int>> WordMap;
  typedef std::vector<std::string> SpellingTable;

  // Creates an administrator with a list of c13 files to compile.
  explicit Administrator(const std::vector<std::string> file_list);
  // Creates an administrator with a list of  c13 files to compile and defines
  // an optional file to output trace and error messages to.
  Administrator(const std::vector<std::string> file_list, 
                const std::string &error_file);
  
  // Runs all phases of the compilation process.
	bool Compile();
  
  // 
  static SpellingTable CreateSpellingTable();

  // Creates a word map containing all of C13's reserved words.
	static WordMap CreateWordMap();

  //
  static int GetSpellingTableIndex(const std::string &key);

  // Returns an iterator to a token stored in the word map using the given key.
  // If the key is not found then it gets added to the word map and the 
  // iterator is returned.
	static WordMap::const_iterator GetWord(const std::string &key);
  
  // Returns the string representation of an integer.
  static std::string IntToString(const int &num);

  // Processes each input file up to the Lexical Analyser, or Scanner, and returns
  // true if there were no errors.
  bool LexerPhase();

  // Gets the pointer the administrator's messenger.
  inline Messenger* messenger() {
    return &messenger_;
  }

  // Processes each input file up to the Parser and returns true if there were
  // no errors.
  bool ParserPhase();

  // Processes each input file up to the Semantic Analyser and returns true if
  // there were no errors.
  bool SemanticAnalysisPhase();

  // Sets the compiler to print trace messages if trace is true.
  void ShowTrace(const bool trace) {
    show_trace_ = true;
    messenger_.set_show_trace(trace);
  }

  // Processes each input file up to the Tuple Generation phase and returns true if
  // there were no errors.
  bool TupleGenerationPhase();

  // Index of the most recent element placed in the SpellingTable.
  static int last_spelling_table_entry;

  // Keeps track of the total number of entries in the spelling table.
  static int num_spelling_table_entries;

  // Strings of reserved words to be used as keys in the word_map_.
	static std::string reserved_words_list[];

  // List of string representations of identiers. ID Tokens contain an index
  // that points to their lexeme in this table.
	static SpellingTable spelling_table;

  // A hash map that contains C13's reserved words and identifiers.
	static WordMap word_map;

 private:
  // List of .cs13 files to be compiled.
  std::vector<std::string> input_file_list_;

  // Administrator controls the messenger.
  Messenger messenger_;

  //
  bool show_trace_;

  //DISALLOW_COPY_AND_ASSIGN(Administrator);
};

#endif	// KOMPILER_ADMINISTRATOR_H_
