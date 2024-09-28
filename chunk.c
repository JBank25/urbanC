#include "chunk.h"
#include "memory.h"
#include "value.h"

#include <stdlib.h>

void Chunk_InitChunk(Chunk *chunk)
{
    // TODO: error check if ptr is NULL
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    // init ValueArray as well for constants in chunk
    initValueArray(&chunk->constants);
}

void Chunk_WriteChunk(Chunk *chunk, uint8_t byte, int line)
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

void Chunk_FreeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    Chunk_InitChunk(chunk);
}

int Chunk_AddConstant(Chunk *chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    // return index where last constant was appended so we can locate it later
    return chunk->constants.count - 1;
}