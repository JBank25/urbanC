#include "chunk.h"
#include "common.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

static int simpleInstruction(const char *name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

/**
 * Disassembles a chunk of bytecode.
 *
 * This function takes a Chunk object and a name as input parameters and prints the disassembled
 * instructions of the chunk to the console. It iterates over the bytecode instructions in the
 * chunk and calls the disassembleInstruction function to disassemble each instruction.
 *
 * @param chunk The Chunk object to be disassembled.
 * @param name  The name of the chunk.
 */
void disassembleChunk(Chunk *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (uint32_t offset = 0; offset < chunk->count;)
    {
        // rather than iterating offset by a fixed amount, we let disassembleInstruction increment
        // based on the size of instruction is has just disassembled
        offset = disassembleInstruction(chunk, offset);
    }
}

/**
 * Disassembles an instruction in the given chunk at the specified offset.
 *
 * @param chunk The chunk containing the instruction.
 * @param offset The offset of the instruction in the chunk.
 */
int disassembleInstruction(Chunk *chunk, int offset)
{
    // print byte offset at a given instruction
    printf("%04d ", offset);
    // grab instruction at offset
    uint8_t instruction = chunk->code[offset];
    // switch on instruction and use utility function for displaying it
    switch (instruction)
    {
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        // print if bug exists in compiler
        printf("Unknown opcode: %d\n", instruction);
        return offset + 1;
    }
}