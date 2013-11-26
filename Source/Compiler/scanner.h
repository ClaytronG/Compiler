// Author: Clayton Green (kgreen1@unbc.ca)
// 
// The Scanner reads tokens from a source program file, groups them into
// lexically meaningful units called LEXEMES, and produces TOKENS that
// represent those lexems. The scanner transforms the source program into a 
// stream of tokens that are read by the Parser.

#ifndef KOMPILER_SCANNER_H_
#define KOMPILER_SCANNER_H_

#include <fstream>
#include <string>

#include "token.h"

class Administrator;

// 
class Scanner {
 public:
   
   Scanner(const std::string &built_in_functions_source, 
           Administrator *administrator);

  // Creates a scanner for a C13 source file.
  Scanner(const std::string &file_path, const std::string &filename, 
          Administrator *administrator);

  ~Scanner() {};

  // Gets the next token from the source file. The scanner reads in characters
  // until a valid token is created or an invalid character or sequence of
  // characters is read in.
  Token GetToken();
  
  // Returns the filename of the source file being scanned. This is used for
  // error and trace message purposes.
  inline std::string filename() const {
    return filename_;
  }

  // Returns true if the source file was successfully opened. It is the 
  // responsibility of the user to check if the Scanner is good before using.
  inline bool good() const {
    return good_;
  }

  // Returns the current line that the scanner is on of the source file.
	inline int line_number() const {
    return line_number_;
  }

 private:
  // Reads in and ignores all characters while in a comment. Supports nested
  // comments.
	int EatComment();

  // Checks if character is a control code in the ASCII set.
  bool IsControlCode(const char &character) const;

  // Checks if character is a number character, 0-9 in the ASCII set.
	bool IsDigit(const char &character) const;

  // Checks if character is an upper of lower case letter in the ASCII set.
	bool IsLetter(const char &character) const;

  // Checks if character is whitespace. Whitespace is a TAB\t(09), 
  // LINEFEED\n(10), CARRIAGE_RETURN\r(13), or SPACE (32).
  bool IsWhitespace(const char &character) const;

  // Returns the next character in the source file without moving onto the next
  // character.
  char PeekChar();

  // Returns the current character in the source file and moves onto the next
  // character.
  char GetChar();

  // Prints the current line of the source file. Preceding whitespace is
  // removed and control chacters are skipped.
  void PrintLine();
  
  //
  Administrator *administrator_;

  // Current character that the scanner is looking at. This gets changed by
  // GetChar, PeekChar and PrintLine and should never be modified by the user.
  std::string::const_iterator current_char_;

  // True if all of the characters in the source file have been read.
  bool done_;

  // Name of the source file being scanned. Has path removed and is just
  // filename and file extension
  const std::string filename_;

  // True if the source file was successfully opened.
  bool good_;

  // Current line of the source file.
	int line_number_;

  // The contents of the source file, with preceding and trailing whitespace
  // removed.
  std::string source_;


  //DISALLOW_COPY_AND_ASSIGN(Scanner);
};

#endif  // KOMPILER_SCANNER_H_