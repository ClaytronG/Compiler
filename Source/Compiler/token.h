// Author: Clayton Green (kgreen1@unbc.ca)
//
// The Token class is a reserved word or a psuedo token of the C*13 language,
// or an identifer in the source code. Token types can be STD_TOKEN (psuedo
// tokens), WORD_TOKEN (identifiers and reserved words) or a NUM_TOKEN. When
// created a WORD_TOKEN is automatically added to the word list that the
// administrator holds.
//
// A token consists of two parts: a TOKEN NAME and an ATTRIBUTE VALUE.

#ifndef KOMPILER_TOKEN_H_
#define KOMPILER_TOKEN_H_

#include <ostream>
#include <string>

//
class Token {
 public:
  // A list of all of the symbolic names of the tokens in the C*13 language.
	enum TokenName {
    AND,
    BOOL,
    BRANCH,
    CASE,
    CONTINUE,
    DEFAULT,
    ELSE,
    END,
    EXIT,
    IF,
    INT,
    LOOP,
    MOD,
    NOT,
    OR,
    REF,
    RETURN,
    VOID,
    // -------------
    ID,			  // identifier
    NUM,		  // numeral
    BLIT,		  // boolean literal ('true', 'false') 
    ENDFILE,	// end of source text (added by scanner)
    ERROR,		// erroneous token
    // -------------
    PLUS,		  // '+'
    MINUS,		// '-'
    MULT,		  // '*'
    DIV,		  // '/'
    ANDTHEN,	// '&&'
    ORELSE,		// '||'
    LT,			  // '<'
    LTEQ,		  // '<='
    GT,			  // '>'
    GTEQ,		  // '>='
    EQ,			  // '='
    NEQ,		  // '/='
    ASSIGN,		// ':='
    SEMI,		  // ';'
    COLON,    // ':'
    COMMA,		// ','
    LPAREN,		// '('
    RPAREN,		// ')'
    LSQR,		  // '['
    RSQR,		  // ']'
    LCRLY,		// '{'
    RCRLY,	  // '}'
    LAST_TOKEN_NAME, // Used for iterating over the TokenName enum
    UNIVERSAL
	};

  // A token can be a STD_TOKEN, WORD_TOKEN, or a NUM_TOKEN. A STD_TOKEN is a
  // token in which its name describes it fully. For example a COLON token is 
  // obviously a token representing the character ':'. A WORD_TOKEN is a token 
  // where its lexeme holds additional information. And finally a NUM_TOKEN
  // represents a integer literal.
	enum TokenType {
		STD_TOKEN,
		WORD_TOKEN,
		NUM_TOKEN,
    LAST_TOKEN_TYPE // Used for iterating over the TokenType enum
	};

  // String representation of boolean literals (true, false).
  static const std::string kBlitStrings[];

  // String representation of tokens. These are used in reporting errors and 
  // producing trace output.
	static const std::string kTokenStrings[];

  // Construct a name token. Used for creating STD_TOKENS where the token name
  // fully describes the token.
  explicit Token(const TokenName name);
  // Construct a token where the lexeme holds additional information about the
  // token. Used for creating tokens corresponding to identifiers, boolean
  // literals and reserved words.
	Token(const TokenName name, const std::string &lexeme);
  // Construct a token with a value. Usually used to construct NUM_TOKENS.
	Token(const TokenName name, const int value);
  
  // Returns the symbolic name of the token (ID, BLIT, AND, etc.). Refer to 
  // TokenName for more information.
  inline TokenName name() const {
    return name_;
  }

  // Returns string representation of the token. Used mainly for trace
  // purposes.
	std::string toString();

  // Returns the type of token (STD_TOKEN, WORD_TOKEN, or NUM_TOKEN). Refer to
  // TokenType for more information.
  inline TokenType type() const {
    return type_;
  }
  
  // Returns the value of the token. The meaning of value_ depends on what
  // what type of token this is. See value_ declaration for more information.
  inline int value() const {
    return value_;
  }

 private:
  // Name describing the token. Refer to TokenName for all values.
  TokenName name_;
  
  // One of three Token types, depending on token name. Refer to TokenType for
  // all values.
  TokenType type_;

  // Value has different meanings depending on the Token Type.
  // If the token is an ID then value is its spelling table entry.
  // If the token is a NUM then value is its attribute value
  // If the token is a BLIT then value is its bool representation (0 false, 1 true)
  int value_;
};

#endif  // KOMPILER_SCANNER_H_