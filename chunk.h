#pragma once

#include "common.h"
#include "value.h"

// "Chunk" are what we will call a sequence of byte code

// Each instruction has a one-byte opcode which determines what kind of instruction
// we are dealing with. They are defined here
typedef enum
{
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_RETURN,
} OpCode;

// Bytecode is a series of instructions. Hence the need for this to be dynamically
// sized.
typedef struct
{
    // num of elements we currently have allocated
    uint32_t count;
    // num of allocated entries that are actually being used
    uint32_t capacity;
    // store chunk's constants
    ValueArray constants;
    uint8_t *code;
    int *lines;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);