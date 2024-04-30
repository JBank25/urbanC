#include "scanner.h"

#include "common.h"

#include <stdio.h>
#include <string.h>

typedef struct
{
    const char *start;   // starting char of lexeme
    const char *current; // char currently on
    int line;            // line lexeme is on
} Scanner;

Scanner scanner;

void initScanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/**
 * Checks if a character is a digit.
 *
 * @param c The character to check.
 * @return true if the character is a digit, false otherwise.
 */
static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

/**
 * Checks if a character is an alphabetic character or an underscore.
 *
 * @param c The character to check.
 * @return true if the character is an alphabetic character or an underscore, false otherwise.
 */
static bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

/**
 * Checks if the scanner has reached the end of the input string.
 *
 * @return true if the scanner has reached the end of the input string, false otherwise.
 */
static bool isAtEnd()
{
    return *scanner.current == '\0';
}

/**
 * Advances the scanner's current position by one character and returns the character at the previous position.
 *
 * @return The character at the previous position.
 */
static char advance()
{
    scanner.current++;
    return scanner.current[-1];
}

/**
 * Returns the character currently being pointed to by the scanner.
 *
 * @return The character currently being pointed to by the scanner.
 */
static char peek()
{
    return *scanner.current;
}

/**
 * Returns the next character in the input stream without consuming it.
 * If the end of the input stream is reached, it returns the null character '\0'.
 *
 * @return The next character in the input stream, or '\0' if at the end of the stream.
 */
static char peekNext()
{
    if (isAtEnd())
        return '\0';
    return scanner.current[1];
}

/**
 * Checks if the current character matches the expected character.
 * If the current character does not match the expected character, the function returns false.
 * If the current character matches the expected character, the function advances the scanner's current position and returns true.
 *
 * @param expected The character to match against the current character.
 * @return True if the current character matches the expected character, false otherwise.
 */
static bool match(char expected)
{
    if (isAtEnd())
        return false;
    if (*scanner.current != expected)
        return false;
    scanner.current++;
    return true;
}

/**
 * Creates a token of the specified type.
 *
 * @param type The type of the token.
 * @return The created token.
 */
static Token makeToken(TokenType type)
{
    Token token;
    token.type = type;
    token.start = scanner.start; // recall scanner is tracking these data points
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char *message)
{
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message; // points to error message instead of source code
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

/**
 * Skips any whitespace characters in the input stream.
 * This function is used by the scanner to ignore spaces, tabs, and newlines.
 * It also handles single-line comments that start with '//'.
 */
static void skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case '/':
            if (peekNext() == '/')
            {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd())
                    advance();
            }
            else
            {
                return;
            }
            break;
        default:
            return;
        }
    }
}

/**
 * Checks if a given substring matches a keyword and returns the corresponding token type.
 *
 * @param start The starting index of the substring in the input string.
 * @param length The length of the substring.
 * @param rest The remaining characters of the input string.
 * @param type The token type to return if the substring matches a keyword.
 * @return The token type corresponding to the keyword if the substring matches, otherwise TOKEN_IDENTIFIER.
 */
static TokenType checkKeyword(int start, int length, const char *rest, TokenType type)
{
    // validate strings are of same len first THEN check that
    // the strings are the SAME
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0)
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

/**
 * @brief CREATE THE KEYWORDS FOR YOUR LANGUAGE HERE!!!!
 *
 * @return TokenType
 */
static TokenType identifierType()
{
    switch (scanner.start[0])
    {
    case 'a':
        return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c':
        return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e':
        return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'a':
                return checkKeyword(2, 3, "lse", TOKEN_FALSE);
            case 'o':
                return checkKeyword(2, 1, "r", TOKEN_FOR);
            case 'u':
                return checkKeyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;
    case 'i':
        return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n':
        return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o':
        return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p':
        return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
        return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
        return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'h':
                return checkKeyword(2, 2, "is", TOKEN_THIS);
            case 'r':
                return checkKeyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;
    case 'v':
        return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w':
        return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Scans for an identifier token.
 * This function scans characters until it reaches a non-alphanumeric character,
 * and then returns a token representing the identifier.
 *
 * @return The identifier token.
 */
static Token identifier()
{
    // continue until next char isnt a number or letter
    while (isAlpha(peek()) || isDigit(peek()))
        advance();
    return makeToken(identifierType());
}

/**
 * Scans and returns a token representing a number.
 * This function scans the input and identifies a number, including any fractional part.
 * It advances the input stream until it reaches the end of the number.
 *
 * @return The token representing the number.
 */
static Token number()
{
    while (isDigit(peek()))
        advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the ".".
        advance();

        while (isDigit(peek()))
            advance();
    }

    return makeToken(TOKEN_NUMBER);
}

/**
 * Scans for a string literal in the source code.
 *
 * This function scans the source code character by character until it finds
 * the closing double quote (") that marks the end of a string literal. It
 * also handles the case of an unterminated string, where the closing quote
 * is missing.
 *
 * @return The token representing the string literal.
 */
static Token string()
{
    // break when we reach closing " or when we reach the end
    while (peek() != '"' && !isAtEnd())
    {
        // support multi-line strings
        if (peek() == '\n')
            scanner.line++;
        advance();
    }
    // if end reached, no closing " was found ERROR
    if (isAtEnd())
        return errorToken("Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_STRING);
}

// ***************** STAR OF SCANNER SHOW *****************************
Token scanToken()
{
    skipWhitespace();
    // Each call to this func scans a complete token. Guaranteed to be at
    // beginning of a new token when we enter
    scanner.start = scanner.current;

    // Check if at the end of source code
    if (isAtEnd())
        return makeToken(TOKEN_EOF);

    char c = advance();

    if (isAlpha(c)) // check for identifiers
        return identifier();

    if (isDigit(c)) // check if token is digit, bit simpler than adding all digits to switch
        return number();

    switch (c)
    {
    case '(':
        return makeToken(TOKEN_LEFT_PAREN); // single char token
    case ')':
        return makeToken(TOKEN_RIGHT_PAREN); // single char token
    case '{':
        return makeToken(TOKEN_LEFT_BRACE); // single char token
    case '}':
        return makeToken(TOKEN_RIGHT_BRACE); // single char token
    case ';':
        return makeToken(TOKEN_SEMICOLON); // single char token
    case ',':
        return makeToken(TOKEN_COMMA); // single char token
    case '.':
        return makeToken(TOKEN_DOT); // single char token
    case '-':
        return makeToken(TOKEN_MINUS); // single char token
    case '+':
        return makeToken(TOKEN_PLUS); // single char token
    case '/':
        return makeToken(TOKEN_SLASH); // single char token
    case '*':
        return makeToken(TOKEN_STAR); // single char token
    case '!':                         // MAYBE double char token
        return makeToken(
            // two char punctionation here != check
            match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=': // MAYBE double char token
        return makeToken(
            match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<': // MAYBE double char token
        return makeToken(
            match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>': // MAYBE double char token
        return makeToken(
            match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"': //
        return string();
    }

    return errorToken("Unexpected character.");
}