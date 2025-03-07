#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

static int simpleInstruction(const char *name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

static int byteInstruction(const char *name, Chunk *chunk,
                           int offset)
{
    uint8_t slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

static int jumpInstruction(const char *name, int sign, Chunk *chunk, int offset)
{
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset,
           offset + 3 + sign * jump);
    return offset + 3;
}

/**
 * @brief Disassembles a constant instruction.
 *
 * @param name - name of the instruction
 * @param chunk - chunk containing the instruction
 * @param offset - offset of the instruction in the chunk
 * @return int - offset of the next instruction
 */
static int Debug_constantInstruction(const char *name, Chunk *chunk, int offset)
{
    // opcode (instruction) lives at offset 0, constant at offset 1 for OP_CONSTANT instruction
    uint8_t constant = chunk->code[offset + 1];
    printf("Instruction name: %-16s Constant Idx: %4d ", name, constant);
    printf("Constant Idx Val: '");
    // TODO: fix number color here
    Value_printValue(chunk->constants.values[constant], 32);
    printf("'\n");
    // OP_CONSTANT is a two-byte instruction, need to increment by 2 to get to next instruction
    return offset + 2;
}

void Print_Color(const char *text, int color_code)
{
    printf("\033[%dm%s\033[0m", color_code, text);
}

/**
 * Disassembles a chunk of bytecode.
 *
 * This function takes a Chunk object and a name as input parameters and prints the disassembled
 * instructions of the chunk to the console. It iterates over the bytecode instructions in the
 * chunk and calls the Debug_disassembleInstruction function to disassemble each instruction.
 *
 * @param chunk The Chunk object to be disassembled.
 * @param name  The name of the chunk.
 */
void disassembleChunk(Chunk *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (uint32_t offset = 0; offset < chunk->count;)
    {
        // rather than iterating offset by a fixed amount, we let Debug_disassembleInstruction increment
        // based on the size of instruction is has just disassembled
        offset = Debug_disassembleInstruction(chunk, offset);
    }
}

/**
 * Disassembles an instruction in the given chunk at the specified offset.
 *
 * @param chunk The chunk containing the instruction.
 * @param offset The offset of the instruction within the chunk.
 *
 * @return offset of the next instruction
 */
int Debug_disassembleInstruction(Chunk *chunk, int offset)
{
    // print byte offset of the instruction within the chunk
    printf("Byte offset: %04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
    {
        printf("%-15s", "|");
    }
    else
    {
        printf("Line Num: %-4d ", chunk->lines[offset]);
    }

    // grab the instruction (opcode) at the offset
    uint8_t instruction = chunk->code[offset];

    // switch on instruction and use utility function for displaying it
    switch (instruction)
    {
    case OP_CONSTANT:
        return Debug_constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);
    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case OP_SET_GLOBAL:
        return Debug_constantInstruction("OP_SET_GLOBAL", chunk, offset);
    case OP_POP:
        return simpleInstruction("OP_POP", offset);
    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);
    case OP_DEFINE_GLOBAL:
        return Debug_constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
    case OP_GET_LOCAL:
        return byteInstruction("OP_GET_LOCAL", chunk, offset);
    case OP_SET_LOCAL:
        return byteInstruction("OP_SET_LOCAL", chunk, offset);
    case OP_GET_GLOBAL:
        return Debug_constantInstruction("OP_GET_GLOBAL", chunk, offset);
    case OP_JUMP:
        return jumpInstruction("OP_JUMP", 1, chunk, offset);
    case OP_JUMP_IF_FALSE:
        return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
    case OP_LOOP:
        return jumpInstruction("OP_LOOP", -1, chunk, offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        // print if bug exists in compiler
        printf("Unknown opcode: %d\n", instruction);
        return offset + 1;
    }
}