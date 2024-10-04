#pragma once

/**
 * enum identifying the type of token we are working with.
 */
typedef enum
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,
    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    // Keywords.
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_FUN,
    TOKEN_IF,
    TOKEN_NIL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_SUPER,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,

    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

/*
    Simple example here, start will point to the original source string start like this
    print hello;<NULL>
          ^
        strt
*/
typedef struct
{
    TokenType type;    // Tells us type of token. Is token a number? identifier? operator?
    const char *start; // ptr to first char in the original source string that is our token
    int length;        // length in chars of the token
    int line;          // line number associated with the token
} Token;

/**
 * @brief Our scanner has a state, thus we should initialize it.
 *
 * @param source - This is the very first char on the very first line of the source code that
 *                 will be scanned
 */
void Scanner_InitScanner(const char *source);

/**
 * @brief - This function will start at a new token when called.
 *
 * @return Token
 */
Token Scanner_ScanToken();
