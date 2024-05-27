#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "value.h"

#include <stdlib.h>

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif // DEBUG_PRINT_CODE

typedef struct
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

/**
 * Struct which we use to wrap the three properties contained in a single
 * row of the parser table.
 */
typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;
Chunk *compilingChunk;

static Chunk *currentChunk()
{
    return compilingChunk;
}

static void errorAt(Token *token, const char *message)
{
    // surpress any further errors if error has already occurred (will be in panicMode)
    if (parser.panicMode)
        return;

    // set panicMode flag in the event more errors occur during compilation
    parser.panicMode = true;

    // print where error occurred
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR)
    {
        // Nothing.
    }
    else
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void error(const char *message)
{
    // often the case we'll report an error at location of token
    // we just consumed. This function (error) rather than errorAtCurrent will
    // be used for that
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}

static void advance()
{
    // save current token
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = scanToken();
        // NO lexical errors, rather special error tokens will be created and left to parser to report
        if (parser.current.type != TOKEN_ERROR)
            break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}

/**
 * @brief Return true if current token has given type
 *
 * @param type - token type we are checking for
 * @return true
 * @return false
 */
static bool check(TokenType type)
{
    return parser.current.type == type;
}

static bool match(TokenType type)
{
    // if current token has given type, consume it and return true
    // else return false
    if (!check(type))
    {
        return false;
    }
    advance();
    return true;
}

static void emitByte(uint8_t byte)
{
    // write opcode or operand to prev line so runtime errors are associated w it
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

/**
 * @brief
 *
 * @param value
 * @return uint8_t
 */
static uint8_t makeConstant(Value value)
{
    // add value to constant array
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler()
{
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError)
    {
        disassembleChunk(currentChunk(), "code");
    }
#endif // DEBUG_PRINT_CODE
}

static void expression();
static void statement();
static void declaration();
static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary()
{
    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType)
    {
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    case TOKEN_BANG_EQUAL:
        emitBytes(OP_EQUAL, OP_NOT);
    case TOKEN_EQUAL_EQUAL: // single instruction for ==
        emitByte(OP_EQUAL);
    case TOKEN_GREATER: // single instr for >
        emitByte(OP_GREATER);
    case TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
    case TOKEN_LESS: // single instr for <
        emitByte(OP_LESS);
    case TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
    default:
        return; // Unreachable.
    }
}

/**
 * @brief When parser encounters false, nil, or true in prefix position
 * it will call this new parser function
 *
 */
static void literal()
{
    switch (parser.previous.type)
    {
    case TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TOKEN_NIL:
        emitByte(OP_NIL);
        break;
    case TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    default:
        return; // Unreachable.
    }
}

static void expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

static void expressionStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression. ");
    emitByte(OP_POP);
}

static void printStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

/**
 * @brief Compile a single declaration
 *
 */
static void declaration()
{
    statement();
}

static void statement()
{
    if (match(TOKEN_PRINT))
    {
        printStatement();
    }
    else
    {
        expressionStatement();
    }
}
static void grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/**
 * @brief This function is used to compile number literals.
 *
 */
static void number()
{
    // convert the value at parser.previous.start to a double
    double value = strtod(parser.previous.start, NULL);
    // wrap it in Value before storing it in constant table
    emitConstant(NUMBER_VAL(value));
}

/**
 * @brief When parser hits string token this function will be called
 *
 */
static void string()
{
    // The + 1 and - 2 parts trim the leading and trailing quotation marks
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

/**
 * @brief PREFIX EXPRESSION
 *
 */
static void unary()
{
    // leading - has been consumed and is sitting in parser.previous
    TokenType operatorType = parser.previous.type;

    // Compile the operand (recursive). ONLY expressions at a certain precedence
    // level OR higher should be compiled!
    parsePrecedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType)
    {
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    default:
        return; // Unreachable.
    }
}

/**
 * This is a table that will allow us to, for a given token type, find the:
 *      The function to compile a PREFIX expression starting w/ a token of that type
 *      The functiom to copmile an INFIX expression whose left operand if followed by a token of that type
 *      The precedence of an INFIX expression that uses that token as an operator
 */
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {binary, NULL, PREC_COMPARISON},
    [TOKEN_LESS] = {binary, NULL, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {binary, NULL, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

/**
 * @brief Start at a given precedence and parse anything which is >=
 *
 * @param precedence
 */
static void parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL)
    {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

/**
 * @brief Get the Rule object
 *
 * @param type - type (TOKEN_DOT, TOKEN_MINUS, etc) used to index into the rules array
 * @return ParseRule*
 */
static ParseRule *getRule(TokenType type)
{
    return &rules[type];
}

bool compile(const char *source, Chunk *chunk)
{
    initScanner(source); // initialize the state of scanner
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();

    while (!match(TOKEN_EOF))
    {
        declaration();
    }

    endCompiler();
    // return false if an error occurred
    return !parser.hadError;
}