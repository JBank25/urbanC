#include "vm.h"

#include "common.h"
#include "debug.h"
#include "value.h"

// Globally declared virtual machine
VM vm;

static void resetStack()
{
    vm.stackTop = vm.stack; // reset stack ptr to first element
}

/**
 * @brief
 * MOST important function by far in program. Majority of execution will
 * be spent inside of here
 */
static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
// reads the next byte from the bytecode, treats the resulting number as an index, and
// looks up the corresponding Value in the chunk’s constant table
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        // Grab relative offset of ip from beginning of bytecide
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif // end DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        // grab byte pointed to by ip and advance ip
        // given opcode, get right C code that implements instruction's semantics
        switch (instruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            push(constant);
            printf("\n");
            break;
        }
        case OP_NEGATE:
        {
            push(-pop());
            break;
        }
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk *chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

void initVM()
{
    resetStack(); // VM state must be initialized
}

void freeVM()
{
}

void push(Value value)
{
    *vm.stackTop = value; // deref and save value into stack
    vm.stackTop++;        // move top of stack to next entry
}

Value pop()
{
    vm.stackTop--;       // move back once, recall the PREVIOUS element is the top value in stack
    return *vm.stackTop; // return "popped" value
}