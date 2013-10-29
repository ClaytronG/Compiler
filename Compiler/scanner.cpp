// Author: Clayton Green (kgreen1@unbc.ca)

#include <iostream>
#include <stdlib.h>

#include "scanner.h"

#include "administrator.h"

Scanner::Scanner(const std::string &file_path, const std::string &filename, Administrator &administrator)
  : filename_(filename), done_(false), administrator_(administrator) {
  // Get the source as a string and remove whitespace before and after the 
  // source code
  std::ifstream in(file_path, std::ios_base::in | std::ios_base::binary);
  if (in) {
    in.seekg(0, std::ios::end);
    source_.resize((unsigned int)in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&source_[0], source_.size());
    in.close();    
    std::size_t startpos = source_.find_first_not_of(" ");
    std::size_t endpos = source_.find_last_not_of(" ");
    source_ = source_.substr(startpos, endpos+1);
    current_char_ = source_.begin();
		line_number_ = 1;
		PrintLine();
    good_ = true;
  } else {
    fprintf(stdout, "Error opening file: %s\n", filename_.c_str());
    good_ = false;
  }
}

Token Scanner::GetToken() {
  if (done_) {
    return Token(Token::ENDFILE);
  }

	// Check to see if there is white space
  while (IsWhitespace(PeekChar())) {
		if (PeekChar() == '\n') {
      GetChar();
			++line_number_;
			PrintLine();
		} else {
      GetChar();
    }
  }
  
  char peek_char = PeekChar();
	do {
    if (IsLetter(peek_char)) {
			// Then it starts with a letter
			// It's an ID or another keyword
			std::string lexeme;
			lexeme += GetChar();
      peek_char = PeekChar();
			if (IsLetter(peek_char) || IsDigit(peek_char) || peek_char == '_' ||
				  peek_char == '$') {
				while (IsLetter(peek_char) || IsDigit(peek_char) || peek_char == '_' ||
               peek_char == '$') {
					lexeme += GetChar();
          peek_char = PeekChar();
				}
      }
      return Token(Token::ID, lexeme);
    } else if (IsDigit(peek_char)) {
			// Then it's a number
			std::string number;
			number += GetChar();
      peek_char = PeekChar();
      while(IsDigit(peek_char))
			{
				number += GetChar();
        peek_char = PeekChar();
			}
			return Token(Token::NUM, static_cast<int>(atof(number.c_str())));
		}
    switch (peek_char) {
		case '+':
			GetChar(); // eat it!
			return Token(Token::PLUS);
			break;
		case '-':
			GetChar(); // eat it again!
			if (PeekChar() == '-') {
        // Read in the rest of the line
        while (PeekChar() != '\n') {
          GetChar();
          if (done_) {
            break;
          }
        }
        return GetToken();
			}
			return Token(Token::MINUS);
			break;
		case '*':
			GetChar(); // eat it as well!
			return Token(Token::MULT);
			break;
		case '/':
			GetChar(); // eat it also!
			if (PeekChar() == '=') {
				GetChar();
				return Token(Token::NEQ);
			}	else if (PeekChar() == '*')	{
				GetChar();
				if (EatComment() == -1)	{
				  return Token(Token::ENDFILE);
				}	else {
				  return GetToken();
				}
			}
			return Token(Token::DIV);
			break;
		case '&':
			GetChar();
			if (PeekChar() == '&') {
				GetChar();
				return Token(Token::ANDTHEN);
			}
      
      // TODO: How do I access Administrator's messenger
      administrator_.messenger()->AddError(filename_, line_number_, 
                                            "Invalid Token '&'");
			return Token(Token::ERROR);
			break;
		case '|':
			GetChar();
			if (PeekChar() == '|') {
				GetChar();
				return Token(Token::ORELSE);
			}
      
      // TODO: How do I access Administrator's messenger
      administrator_.messenger()->AddError(filename_, line_number_,
                                            "Invalid Token '|'");
			return Token(Token::ERROR);
			break;
		case '<':
			GetChar();
			if (PeekChar() == '=') {
				GetChar();
				return Token(Token::LTEQ);
			}
			return Token(Token::LT);
			break;
		case '>':
			GetChar();
			if (PeekChar() == '=') {
				GetChar();
				return Token(Token::GTEQ);
			}
			return Token(Token::GT);
			break;
		case '=':
			GetChar();
			return Token(Token::EQ);
			break;
		case ':':
			GetChar();
			if (PeekChar() == '=') {
				GetChar();
				return Token(Token::ASSIGN);
			}
			return Token(Token::COLON);
			break;
		case ';':
			GetChar();
			return Token(Token::SEMI);
			break;
		case ',':
			GetChar();
			return Token(Token::COMMA);
			break;
		case '(':
			GetChar();
			return Token(Token::LPAREN);
			break;
		case ')':
			GetChar();
			return Token(Token::RPAREN);
			break;
		case '[':
			GetChar();
			return Token(Token::LSQR);
			break;
		case ']':
			GetChar();
			return Token(Token::RSQR);
			break;
		case '{':
			GetChar();
			return Token(Token::LCRLY);
			break;
		case '}':
			GetChar();
			return Token(Token::RCRLY);
			break;
		case '_':
    case '$':
      {
        std::string temp;
        temp += GetChar();
        while(!IsWhitespace(PeekChar())) {
          temp += GetChar();
        }
        
        // TODO: How do I access Administrator's messenger
        std::string message = "Invalid Token '";
        message += temp;
        message += "'";
        administrator_.messenger()->AddError(filename_, line_number_, 
                                              message);
			  return Token(Token::ERROR);
			  break;
      }
		default:
			// Check if we're at the end
      if (done_) {
				return Token(Token::ENDFILE);
			} else {
        char temp = GetChar();
        std::string message = "Invalid Token '";
        message += temp;
        message += "'";

        // TODO: How do I access Administrator's messenger
        administrator_.messenger()->AddError(filename_, line_number_, 
                                              message);
        return Token(Token::ERROR);
      }
      break;
		}
	} while(true);
  
  // TODO: How do I access Administrator's messenger
  std::string message = "An unexpected error has occurred.";
  administrator_.messenger()->AddError(filename_, line_number_, message);
	return Token(Token::ERROR);
}

int Scanner::EatComment() {
	int count = 1;
  int start_comment_line = line_number_;
	bool plus = false, minus = false, inComm = true;
	while(count > 0) {
		if (done_) {
      // TODO: How do I access Administrator's messenger
      std::string message = "Reached EOF before comment was closed.";
      administrator_.messenger()->AddError(filename_, start_comment_line,
                                            message);
			return -1;
		}	else if (PeekChar() == '\n') {
			GetChar();
      ++line_number_;
			PrintLine();
		}	else if (inComm) {
			if (PeekChar() == '/') {
				GetChar();
				plus = true;
				minus = false;
				inComm = false;
			}	else if (PeekChar() == '*')	{
				GetChar();
				minus = true;
				plus = false;
				inComm = false;
			}	else {
				GetChar();
			}
		} else if (plus) {
			if (PeekChar() == '*') {
				GetChar();
				++count;
				plus = false;
				minus = false;
				inComm = true;
			} else {
				plus = false;
				minus = false;
				inComm = true;
			}
		}	else if (minus)	{
			if (PeekChar() == '/') {
				GetChar();
				count--;
				minus = false;
				plus = false;
				inComm = true;
			}	else {
				minus = false;
				plus = false;
				inComm = true;
			}
		}
	}
	return 0;
}

bool Scanner::IsControlCode(const char &character) const {
  // A space and newline and carrage return don't count as control characters
  if (IsWhitespace(character)) {
    return false;
  }
  return ((int)character >= 0 && (int)character < 32 || 
          (int)character == 127);
}

bool Scanner::IsDigit(const char &character) const {
	return (character >= '0' && character <= '9');
}

bool Scanner::IsLetter(const char& character) const {
	return (character >= 'A' && character <= 'Z' || 
          character >= 'a' && character <= 'z');
}

bool Scanner::IsWhitespace(const char& character) const {
  return ((int)character == 9 ||  // TAB
          (int)character == 10 || // LF
          (int)character == 13 || // CR
          (int)character == 32);  // SPACE
}

char Scanner::PeekChar() {
  if (current_char_ == source_.end()) {
    done_ = true;
    return -1;
  }
  if (IsControlCode(*current_char_)) {
    ++current_char_;
    return PeekChar();
  }
  return *current_char_;
}

char Scanner::GetChar() {
  if (current_char_ == source_.end()) {
    done_ = true;
    return -1;
  }
  return *(current_char_++);
}

void Scanner::PrintLine() {
  std::string::const_iterator originalPosition = current_char_;
  // Skip the leading whitespace and control characters
  while(current_char_ != source_.end() && *current_char_ != '\n') {
    if (!IsWhitespace(*current_char_) && !IsControlCode(*current_char_)) {
      break;
    }
    GetChar();
  }
  // Print the rest of the line skipping invisible control characters
  std::string line;
  while(current_char_ != source_.end() && *current_char_ != '\n') {
    if (!IsControlCode(*current_char_)) {
      line += *current_char_;
    }
    GetChar();
  }
  std::string message = filename_;
  message += " ";
  message += Administrator::IntToString(line_number_);
  message += "> ";
  message += line;
  message += "\n";
  // TODO: How do I access Administrator's messenger
  administrator_.messenger()->PrintMessage(message);
  current_char_ = originalPosition;
}
