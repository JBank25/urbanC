#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "scanner.h"

#include <stdlib.h>

typedef struct
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

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
        // NO lexical errors, rather soecuak error tokens will be created and left to parser to report
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

static void emitByte(uint8_t byte)
{
    // write opcode or operand to prev line so runtime errors are associated w it
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value)
{
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void endCompiler()
{
    emitReturn();
}

static void number()
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(value);
}

bool compiler(const char *source, Chunk *chunk)
{
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    // expect to be at end of source code after we compile
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    // return false if an error occurred
    return !parser.hadError;
}