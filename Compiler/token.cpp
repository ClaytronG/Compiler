// Author: Clayton Green (kgreen1@unbc.ca)
// Last Updated: September 19, 2013

#include <stdlib.h>

#include "administrator.h"
#include "token.h"

const std::string Token::kBlitStrings[] = {
  "false",
  "true"
};

const std::string Token::kTokenStrings[] = {
	"AND",
	"BOOL",
	"BRANCH",
	"CASE",
	"CONTINUE",
	"DEFAULT",
	"ELSE",
	"END",
	"EXIT",
	"IF",
	"INT",
	"LOOP",
	"MOD",
  "NOT",
	"OR",
	"REF",
	"RETURN",
	"VOID",
	// -----------
	"ID",
	"NUM",
	"BLIT",
	"ENDFILE",
	"ERROR",	
	// ----------
	"PLUS",//"+",
	"MINUS",//"-",
	"MULT",//"*",
	"DIV",//"/",
	"ANDTHEN",//"&&",
	"ORELSE",//"||",
	"LT",//"<",
	"LTEQ",//"<=",
	"GT",//">",
	"GTEQ",//">=",
	"EQ",//"=",
	"NEQ",//"/=",
	"ASSIGN",//":=",
	"SEMI",//";",
  "COLON",//":"
	"COMMA",//",",
	"LPAREN",//"(",
	"RPAREN",//")",
	"LSQR",//"[",
	"RSQR",//"]",
	"LCRLY",//"{",
	"RCRLY"//"}"
};

Token::Token(const Token::TokenName name) : type_(STD_TOKEN), name_(name) {
}

Token::Token(Token::TokenName name, const std::string &lexeme) 
  : type_(WORD_TOKEN) {
  if (name == Token::ERROR)	{
	  name_ = name;
    type_ = STD_TOKEN;
	}	else {
    auto tokenInfo = Administrator::GetWord(lexeme);
	  name_ = std::get<0>(tokenInfo->second);
	  if (name_ == BLIT || name_ == ID) {
		  value_ = std::get<1>(tokenInfo->second);
    }
  }
}

Token::Token(Token::TokenName name, int value) : type_(NUM_TOKEN), name_(name),
    value_(value) {
}

std::string Token::toString() {
	std::string temp = "(";
  temp += kTokenStrings[name_];
  temp += ", ";
	if (type_ == TokenType::STD_TOKEN) {
    temp += ")";
	}	else if (type_ == TokenType::WORD_TOKEN) {
    if (name_ == TokenName::BLIT) {
      temp += kBlitStrings[value_];
      temp += ")";
    } else if (name_ == TokenName::ID) {
      temp += Administrator::IntToString(value_);
      temp += ") => ";
      temp += Administrator::spelling_table.at(value_);
    } else if (name_ == TokenName::NUM) {
      temp += Administrator::IntToString(value_);
      temp += ")";
    } else {
      temp += ")";
    }
	}	else if (type_ == TokenType::NUM_TOKEN) {
    temp += Administrator::IntToString(value_);
    temp += ")";
	}
	return temp;
}
