#include "chunk.h"
#include "memory.h"
#include "value.h"

#include <stdlib.h>

/**
 * Initializes a Chunk structure (holds bytecode).
 *
 * This function sets the count and capacity of the Chunk to 0, and assigns
 * NULL to the code pointer.
 *
 * @param chunk A pointer to the Chunk structure to be initialized.
 */
void initChunk(Chunk *chunk)
{
    // TODO: error check if ptr is NULL
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    // init ValueArray as well for constants in chunk
    initValueArray(&chunk->constants);
}

/**
 * Writes a byte to the chunk's code array and updates the corresponding line number.
 *
 * @param chunk The chunk to write to.
 * @param byte The byte to write.
 * @param line The line number associated with the byte.
 */
void writeChunk(Chunk *chunk, uint8_t byte, int line)
{
    // TODO: error check if ptr is NULL

    // if we need capacity we must expand the ptr
    if (chunk->capacity < chunk->count + 1)
    {
        uint32_t oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

/**
 * Frees the memory allocated for a Chunk object.
 *
 * This function frees the memory allocated for the code, lines, and constants arrays
 * within the Chunk object. It also initializes the Chunk object to its initial state.
 *
 * @param chunk A pointer to the Chunk object to be freed.
 */
void freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

/**
 * Adds a constant value to the chunk's constant array.
 *
 * @param chunk The chunk to add the constant to.
 * @param value The value to add as a constant.
 * @return The index where the last constant was appended.
 */
int addConstant(Chunk *chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    // return index where last constant was appended so we can locate it later
    return chunk->constants.count - 1;
}