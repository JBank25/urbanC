#include "chunk.h"
#include "memory.h"

#include <stdlib.h>

void initChunk(Chunk *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
}

/**
 * Writes a byte to the chunk's code array.
 * If the chunk's capacity is not enough to hold the new byte,
 * the capacity is increased and the code array is reallocated.
 *
 * @param chunk The chunk to write the byte to.
 * @param byte The byte to write.
 */
void writeChunk(Chunk *chunk, uint8_t byte)
{
    if (chunk->capacity < chunk->count + 1)
    {
        uint32_t oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    initChunk(chunk);
}