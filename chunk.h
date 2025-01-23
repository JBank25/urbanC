#pragma once

#include "common.h"
#include "value.h"

// "Chunk" are what we will call a sequence of byte code

/**
 *  Each instruction has a one-byte opcode which determines what kind of instruction
 *  we are dealing with. They are defined here
 */
typedef enum
{
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
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
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_RETURN,
} OpCode;

/**
 * Bytecode is a series of instructions. Hence the need for this to be dynamically
 * sized
 */
typedef struct
{
    uint32_t count;       // num of elements we currently have allocated
    uint32_t capacity;    // num of elements we are able to accomodate
    ValueArray constants; // store chunk's constants, every chunk will have constant pool
    uint8_t *code;
    int *lines; // each entry in this array is the line number of the corresponding instruction
} Chunk;

/**
 * @brief Initializes a Chunk structure (holds bytecode).
 *
 * This function sets the count and capacity of the Chunk to 0, and assigns
 * NULL to the code pointer.
 *
 * @param chunk A pointer to the Chunk structure to be initialized.
 */
void Chunk_InitChunk(Chunk *chunk);

/**
 * @brief Frees the memory allocated for a Chunk object.
 *
 * This function frees the memory allocated for the code, lines, and constants arrays
 * within the Chunk object. It also initializes the Chunk object to its initial state.
 *
 * @param chunk A pointer to the Chunk object to be freed.
 */
void Chunk_FreeChunk(Chunk *chunk);

/**
 * @brief Writes a byte to the chunk's code array and updates the corresponding line number.
 * If the chunk has capacity for the byte then the byte is written to the code array at
 * the index count. If no capacity is available then the code array is expanded and the byte
 * is written to the new index count.
 *
 * @param chunk The chunk to write to.
 * @param byte The byte to write.
 * @param line The line number associated with the byte.
 */
void Chunk_WriteChunk(Chunk *chunk, uint8_t byte, int line);

/**
 * @brief Adds a constant value to the chunk's constant array.
 *
 * @param chunk The chunk to add the constant to.
 * @param value The value to add as a constant.
 * @return The index where the last constant was appended.
 */
int Chunk_AddConstant(Chunk *chunk, Value value);