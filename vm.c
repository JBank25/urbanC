#include "vm.h"

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"

#include <stdarg.h>

// Globally declared virtual machine
VM vm;

static void resetStack()
{
    vm.stackTop = vm.stack; // reset stack ptr to first element
}

// VARIADIC OOOOOOOOOHHHHHHHHH
static void runtimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);         // create ptr to spot on stack where variable args were stored
    vfprintf(stderr, format, args); // this is what printf uses under the hood too
    va_end(args);                   // for portability
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

static Value peek(int distance)
{
    // return value froms stakc but don't pop it
    return vm.stackTop[-1 - distance];
}

/**
 * @brief nil and false are falsey, all other values behave like true
 *
 * @param value
 * @return true
 * @return false
 */
static bool isFalsey(Value value)
{
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
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
// funny looking syntax here, but gives you a way to contain multiple statements
// inside a block that also permits a semicolon at the end.
#define BINARY_OP(valueType, op)                        \
    do                                                  \
    {                                                   \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) \
        {                                               \
            runtimeError("Operands must be numbers.");  \
            return INTERPRET_RUNTIME_ERROR;             \
        }                                               \
        double b = AS_NUMBER(pop());                    \
        double a = AS_NUMBER(pop());                    \
        push(valueType(a op b));                        \
    } while (false)

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
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_EQUAL:
            Value b = pop();
            Value a = pop();
            push(BOOL_VAL(valueEquals(a, b))); // can == on ANY pair of objects
            break;
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_NEGATE:
            // ensure Value type being used for negation is a number
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        case OP_ADD:
            BINARY_OP(NUMBER_VAL, +);
            break;
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;
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
#undef BINARY_OP
}

InterpretResult interpret(const char *source)
{
    Chunk chunk; // create new empty chunk
    initChunk(&chunk);

    // fill chunk w bytecode assuming no errors
    if (!compile(source, &chunk))
    {
        // if errors free chunk and ERROR
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    // else compiled chunk will be executed by vm
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
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
