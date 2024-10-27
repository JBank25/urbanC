#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "value.h"

#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif // DEBUG_PRINT_CODE

#define MAX_STACK_FRAMES 64

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

typedef void (*ParseFn)(bool canAssign);

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

typedef struct
{
    Token name; // name of the variable
    int depth;  // scope depth of the block where the local was declared
} Local;

typedef struct
{
    Local locals[UINT8_COUNT]; // flat array of all locals in scope during each point in compilation
    int localCount;            // counts number of locals are in scope
    int scopeDepth;            // number of blocks surrounding current bit of code we're compiling
} Compiler;

Parser parser;
Compiler *current = NULL;
Chunk *compilingChunk;

static void printStackTrace()
{
    void *stack_traces[MAX_STACK_FRAMES];
    int trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
    char **messages = backtrace_symbols(stack_traces, trace_size);

    for (int i = 2; i < trace_size; ++i)
    {
        printf("%s\n", messages[i]);
    }

    free(messages);
}

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
    printStackTrace();
    // often the case we'll report an error at location of token
    // we just consumed. This function (error) rather than errorAtCurrent will
    // be used for that
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}

/**
 * @brief Save current token in parser to previous. Then grab NEXT token using
 *
 */
static void advance()
{
    // save current token
    parser.previous = parser.current;

    for (;;)
    {
        // grab next token and set parsers current token to it
        parser.current = Scanner_ScanToken();
        // NO lexical errors, rather special error tokens will be created and left to parser to report
        if (parser.current.type != TOKEN_ERROR)
            break;

        errorAtCurrent(parser.current.start);
    }
}

/**
 * @brief Consume token of expected type or throw error
 *
 * @param type - type of token we are expecting
 * @param message - error message to display if token is not of expected type
 */
static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        // advance to the next token
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

/**
 * @brief Check if current token is of type. Return true and advance to next
 * token if it is, else return false
 *
 * @param type - TokenType we are checking the current token in parser for
 * @return true
 * @return false
 */
static bool match(TokenType type)
{
    // if current token has given type, consume it and return true
    // else return false
    bool isCurTokenType = check(type);
    if (!isCurTokenType)
    {
        return false;
    }
    advance();
    return true;
}

static void emitByte(uint8_t byte)
{
    // write opcode or operand to prev line so runtime errors are associated w it
    Chunk_WriteChunk(currentChunk(), byte, parser.previous.line);
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
    int constant = Chunk_AddConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

/**
 * @brief This function is used to patch the offset of a jump instruction. Called by
 * emitJump and before patchJump.
 *
 * @param instruction - placeholder instruction to be patched
 * @return int - offset of the jump instruction
 */
static int emitJump(uint8_t instruction)
{
    // emit opcode byte bc multiple instructions use this function 'if' and
    emitByte(instruction);
    // 16 bit offset lets us jump up to 65,535 bytes forward or backward
    // SHOULD be plents
    emitByte(0xff);
    emitByte(0xff);

    return currentChunk()->count - 2;
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void emitLoop(int loopStart)
{
    // emit new loop instruction.
    emitByte(OP_LOOP);

    int offset = currentChunk()->count - loopStart + 2;
    if (offset > UINT16_MAX)
        error("Loop body too large.");

    emitByte((offset >> 8) & 0xff);
    emitByte(offset & 0xff);
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

/**
 * @brief This function is used to patch the offset of a jump instruction. Called by
 * emitJump AFTER emitJump
 *
 * @param offset
 */
static void patchJump(int offset)
{
    // -2 to adjust for the bytecode for the jump offset itself.
    int jump = currentChunk()->count - offset - 2;

    if (jump > UINT16_MAX)
    {
        error("ASKING TOO MUCH OF BRANCH. Too much code to jump over.");
    }

    currentChunk()->code[offset] = (jump >> 8) & 0xff;
    currentChunk()->code[offset + 1] = jump & 0xff;
}

static void initCompiler(Compiler *compiler)
{
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    for (int i = 0; i < UINT8_COUNT; i++)
    {
        memset(&compiler->locals[i], 0, sizeof(Token) + sizeof(int));
    }
    current = compiler;
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

/**
 * @brief Helper function to increment depth height once we have
 * entered a block
 *
 */
static void beginScope()
{
    current->scopeDepth += 1;
}

/**
 * @brief Helper function to decrement depth height once we have
 * left a block
 *
 */
static void endScope()
{
    current->scopeDepth -= 1;

    // when a block ends we must be rid of the local variables created within it
    while (current->localCount > 0 &&
           current->locals[current->localCount - 1].depth >
               current->scopeDepth)
    {
        emitByte(OP_POP);
        current->localCount--;
    }
}

static void expression();
static void statement();
static void declaration();
static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static uint8_t identifierConstant(Token *name)
{
    char *newString = copyString(name->start, name->length);
    uint8_t stringIdxConstTable = makeConstant(OBJ_VAL(newString));
    return stringIdxConstTable;
}

/**
 * @brief
 *
 * @param a
 * @param b
 * @return true if identifiers are equal
 * @return false else
 */
static bool identifiersEqual(Token *a, Token *b)
{
    // quick fail check if lengths are different
    if (a->length != b->length)
        return false;

    return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(Compiler *compiler, Token *name)
{
    // walk the list of locals in current scope
    for (int i = compiler->localCount - 1; i >= 0; i--)
    {
        Local *local = &compiler->locals[i];
        // if we find same name as identifier token, they must be the same
        if (identifiersEqual(name, &local->name))
        {
            if (local->depth == -1)
            {
                error("Can't read local variable in its own initializer.");
            }
        }
    }

    return -1;
}

static void addLocal(Token name)
{

    // VM only supports up to 256 local variables in scope at a time
    if (current->localCount == UINT8_COUNT)
    {
        error("Too many local variables in function.");
        return;
    }

    Local *local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1; // locals put in uninitialized state at first
}

static void declareVariable()
{
    // ONLY do this for locals, return if we're in global scope
    if (current->scopeDepth == 0)
    {
        return;
    }

    Token *name = &parser.previous;

    // local vars appended to array when added. Start at end and work
    // backward looking for existing var with same name
    for (int i = current->localCount - 1; i >= 0; i--)
    {
        Local *local = &current->locals[i];

        if (local->depth != -1 && local->depth < current->scopeDepth)
        {
            break;
        }

        if (identifiersEqual(name, &local->name))
        {
            error("Already a variable with this name in this scope.");
        }
    }

    addLocal(*name);
}

/**
 * @brief Consume identifier token for variable name, add its lexeme to the
 * chunk's constant table as a string, return the constant table index where
 * it was added
 *
 * @param errorMessage
 * @return uint8_t
 */
static uint8_t parseVariable(const char *errorMessage)
{
    consume(TOKEN_IDENTIFIER, errorMessage);
    declareVariable();
    // exit if we are in local scope. no need to stuff variable's name
    // in constant table
    if (current->scopeDepth > 0)
        return 0;
    return identifierConstant(&parser.previous);
}

static void markInitialized()
{
    current->locals[current->localCount - 1].depth =
        current->scopeDepth;
}

static void defineVariable(uint8_t global)
{
    if (current->scopeDepth > 0)
    {
        // mark local initialized once is has been
        markInitialized();
        return;
    }
    emitBytes(OP_DEFINE_GLOBAL, global);
}

/**
 * @brief Called after the left hand side of the AND operator has been compiled.
 *
 * @param canAssign
 */
static void and_(bool canAssign)
{
    // if left operand is false, jump to the end of the AND expression
    int endJump = emitJump(OP_JUMP_IF_FALSE);

    emitByte(OP_POP);
    parsePrecedence(PREC_AND);

    patchJump(endJump);
}

static void binary(bool canAssign)
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
        break;
    case TOKEN_EQUAL_EQUAL: // single instruction for ==
        emitByte(OP_EQUAL);
        break;
    case TOKEN_GREATER: // single instr for >
        emitByte(OP_GREATER);
        break;
    case TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TOKEN_LESS: // single instr for <
        emitByte(OP_LESS);
        break;
    case TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    default:
        return; // Unreachable.
    }
}

/**
 * @brief When parser encounters false, nil, or true in prefix position
 * it will call this new parser function
 *
 */
static void literal(bool canAssign)
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

static void block()
{
    // compile until end of block or file
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF))
    {
        declaration();
    }
    consume(TOKEN_RIGHT_BRACE, "Expect '}' after block");
}

/**
 * @brief
 *
 */
static void varDeclaration()
{
    // keyword followed by var name is compiled by parseVariable
    uint8_t global = parseVariable("Expect variable name.");

    // look for '=' followed by initializer expression
    if (match(TOKEN_EQUAL)) // will advance to next token if true
    {
        expression();
    }
    else
    {
        // if no '=' then initialize the var to nil
        emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

    defineVariable(global);
}

static void expressionStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression. ");
    emitByte(OP_POP);
}

static void forStatement()
{
    // any variables declared should be scoped to the for loop
    beginScope();

    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    /****START INITIALIZAER CLAUSE****/
    if (match(TOKEN_SEMICOLON))
    {
        // No initializer.
        // for(;...;...;) is valid
    }
    else if (match(TOKEN_VAR))
    {
        // for(var i = 0;...;...) is valid
        varDeclaration();
    }
    else
    {
        expressionStatement();
    }
    /**** END INITIALIZAER CLAUSE****/

    int loopStart = currentChunk()->count;

    /****START CONDITION CLAUSE****/
    int exitJump = -1;
    // clause is optional, if it omitted, the next token MUST be a semicolon
    if (!match(TOKEN_SEMICOLON))
    {
        // if clause is present, compile the expression
        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        // Jump out of the loop if the condition is false.
        exitJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // Condition.
    }
    /**** END CONDITION CLAUSE*****/

    /****START INCREMENT CLAUSE****/
    // clause is optional, if it omitted, the next token MUST be a right paren
    if (!match(TOKEN_RIGHT_PAREN))
    {
        // emit an unconditional jump to the start of the loop so we don't execute the increment yet
        // will hop over the increment clause to the body of the loop
        int bodyJump = emitJump(OP_JUMP);
        int incrementStart = currentChunk()->count;
        expression();
        emitByte(OP_POP);
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop(loopStart);
        loopStart = incrementStart;
        patchJump(bodyJump);
    }
    /**** END INCREMENT CLAUSE*****/

    statement();
    emitLoop(loopStart);

    if (exitJump != -1)
    {
        patchJump(exitJump);
        emitByte(OP_POP); // Condition.
    }

    // end scope for variables declared in for loop
    endScope();
}

/**
 * @brief Ever notice in an if statement in a language like C, the '(' does not
 * do anything? It's just there to make the code more readable, it more easily
 * shows where the condition starts and ends as well as where the body starts.
 *
 * How do we know how far to jump forward to skip the body of the if statement
 * if the condition is false? We use a method called backpatching. We emit the
 * jump instruction first with a placeholder offset. Then we compile the body.
 * Once the body is compiled, we know how far to jump. We go back and fill in
 * the placeholder with the correct offset.
 *
 */
static void ifStatement()
{
    // compile the condition expression between the parentheses
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    // placeholde offset for jump instruction. thenJump is the location
    // of the JUMP instruction
    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    // compile the body of the if statement
    statement();

    // backpatch the jump instruction with correct offset
    patchJump(thenJump);

    emitByte(OP_POP);

    // support for else. Need to account for case where if is TRUE and its body is executed and be
    // careful not to fall thru and execute the body of the else code as well
    if (match(TOKEN_ELSE))
        statement();

    // same as the thenJump above
    int elseJump = emitJump(OP_JUMP);

    // backpatch for the else as well
    patchJump(elseJump);
}

static void printStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

static void whileStatement()
{
    // jump all the way back to reeavluate the condition on each iteration.
    // start of the loop
    int loopStart = currentChunk()->count;
    // compile conditional expression within the parentheses
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    // placeholder for jump instruction
    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    // Needs to know how far back to jump so we can loop back to the start of the while loop.
    // jump all the way back to reeavluate the condition on each iteration.
    emitLoop(loopStart);

    // patch jump after compiling the body of while loop
    patchJump(exitJump);
    emitByte(OP_POP);
}

/**
 * @brief If we hit a compile error we should begin synchronizing.
 * We skip tokens indiscriminately until we reach something that
 * looks like a statement boundary. We recognize the boundary by
 * looking for a preceding token that can end a statement, like a
 * semicolon. Or we’ll look for a subsequent token that begins a
 * statement, usually one of the control flow or declaration
 * keywords.
 */
static void synchronize()
{
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF)
    {
        if (parser.previous.type == TOKEN_SEMICOLON)
            return;
        switch (parser.current.type)
        {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;

        default:; // Do nothing.
        }

        advance();
    }
}

/**
 * @brief Compile a single declaration (class, function, or var declaration)
 *
 */
static void declaration()
{
    if (match(TOKEN_VAR))
    {
        varDeclaration();
    }
    else
    {
        statement();
    }
    // if we hit compile error while parsing prev statement, start synchronizing
    if (parser.panicMode)
    {
        synchronize();
    }
}

/**
 * @brief Handle statements once the declaration function has found one
 *
 */
static void statement()
{
    if (match(TOKEN_PRINT))
    {
        printStatement();
    }
    else if (match(TOKEN_IF))
    {
        ifStatement();
    }
    else if (match(TOKEN_FOR))
    {
        forStatement();
    }
    else if (match(TOKEN_WHILE))
    {
        whileStatement();
    }
    else if (match(TOKEN_LEFT_BRACE))
    {
        // if this executes we have found a block statement
        beginScope();
        block();
        endScope();
    }
    else
    {
        expressionStatement();
    }
}
static void grouping(bool canAssign)
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/**
 * @brief This function is used to compile number literals.
 *
 */
static void number(bool canAssign)
{
    // convert the value at parser.previous.start to a double
    double value = strtod(parser.previous.start, NULL);
    // wrap it in Value before storing it in constant table
    emitConstant(NUMBER_VAL(value));
}

static void or_(bool canAssign)
{
    // if left operand is true, jump to the end of the OR expression
    int elseJump = emitJump(OP_JUMP_IF_FALSE);
    // if both operations are false jump to the end of body
    int endJump = emitJump(OP_JUMP);

    patchJump(elseJump);
    emitByte(OP_POP);

    parsePrecedence(PREC_OR);
    patchJump(endJump);
}

/**
 * @brief When parser hits string token this function will be called
 *
 */
static void string(bool canAssign)
{
    // The + 1 and - 2 parts trim the leading and trailing quotation marks
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

/**
 * @brief Grabs an identifier tokan and adds its lexeme to a chunks constant
 * table as a string. It then emits an instruction that loads the global var
 * with that name
 *
 * @param name
 */
static void namedVariable(Token name, bool canAssign)
{
    uint8_t getOp, setOp;
    // see if the local exists
    int arg = resolveLocal(current, &name);
    if (arg != -1)
    {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    else
    {
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }
    if (canAssign && match(TOKEN_EQUAL))
    {
        expression();
        emitBytes(setOp, (uint8_t)arg);
    }
    else
    {
        emitBytes(getOp, (uint8_t)arg);
    }
}

/**
 * @brief Parsing named variable
 *
 */
static void variable(bool canAssign)
{
    namedVariable(parser.previous, canAssign);
}

/**
 * @brief PREFIX EXPRESSION
 *
 */
static void unary(bool canAssign)
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
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_CALL},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE}, // [big]
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_CALL},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, and_, PREC_AND},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, or_, PREC_OR},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
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

    // only consume '=' if its in the context of a low-precedence expression
    bool canAssign = (precedence <= PREC_ASSIGNMENT);
    prefixRule(canAssign);

    while (precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(canAssign);

        if (canAssign && match(TOKEN_EQUAL))
        {
            error("Invalid assignment target.");
        }
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

bool Compiler_Compile(const char *source, Chunk *chunk)
{
    Scanner_InitScanner(source); // initialize the state of scanner
    Compiler compiler;
    initCompiler(&compiler);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();

    // compile til we hit EOF
    while (!match(TOKEN_EOF))
    {
        declaration();
    }

    endCompiler();
    // return false if an error occurred
    return !parser.hadError;
}