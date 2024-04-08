#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[])
{
    initVM();

    Chunk chunk;
    initChunk(&chunk);
    // storing constant 1.2 in out ValueArray
    int constant = addConstant(&chunk, 1.2);
    // Storing OP_CONSTANT into chunk, letting us know the next byte in chunk is a constant
    writeChunk(&chunk, OP_CONSTANT, 123);
    // storing the index of the constant value
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_ADD, 123);

    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_DIVIDE, 123);

    writeChunk(&chunk, OP_NEGATE, 123);
    writeChunk(&chunk, OP_RETURN, 123);
    disassembleChunk(&chunk, "test chunk");
    // VM begins work when we ask it to interpret some chunk of byte code
    interpret(&chunk);
    freeVM();
    freeChunk(&chunk);
    return 0;
}