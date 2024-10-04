#include "vm.h"

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Globally declared virtual machine
VM vm;

static void Vm_ResetStack()
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
    Vm_ResetStack();
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

static void concatenate()
{
    ObjString *b = AS_STRING(Vm_Pop());
    ObjString *a = AS_STRING(Vm_Pop());

    int length = a->length + b->length;             // calculate length of the two strings to be concatenated
    char *chars = ALLOCATE(char, length + 1);       // allocate char array for the whole new string
    memcpy(chars, a->chars, a->length);             // copy chars from first string into allocated memory
    memcpy(chars + a->length, b->chars, b->length); // same process for second string
    chars[length] = '\0';                           // NULL TERMINATE YOUR STRINGS

    ObjString *result = takeString(chars, length);
    Vm_Push(OBJ_VAL(result));
}

/**
 * @brief
 * MOST important function by far in program. Majority of execution will
 * be spent inside of here
 */
static InterpretResult Vm_Run()
{
#define READ_BYTE() (*vm.ip++)
// reads the next byte from the bytecode, treats the resulting number as an index, and
// looks up the corresponding Value in the chunk’s constant table
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
// yank next two bytes from chunk and build 16 bit integer from them
#define READ_SHORT() \
    (vm.ip += 2, (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]))
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
        double b = AS_NUMBER(Vm_Pop());                 \
        double a = AS_NUMBER(Vm_Pop());                 \
        Vm_Push(valueType(a op b));                     \
    } while (false)
#define READ_STRING() AS_STRING(READ_CONSTANT())

    for (;;)
    {
/*
    With this enabled the VM will print out the current state of the stack and
    the current instruction being executed. This is useful for debugging the VM
*/
#ifdef DEBUG_TRACE_EXECUTION
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Num Values on stack: %lu\n", (vm.stackTop - vm.stack));
        Print_Color(buffer, STACK_ANSI_COLOR_CYAN); // 32 is the color code for green
        // printf("Num Values on stack: %lu\n", (vm.stackTop - vm.stack));
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
        {

            Print_Color("[ ", STACK_ANSI_COLOR_CYAN);
            printValue(*slot, STACK_ANSI_COLOR_CYAN);
            Print_Color(" ]", STACK_ANSI_COLOR_CYAN);
        }
        printf("\n");
        // Grab relative offset of ip from beginning of bytecide
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif // end DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        /* grab byte pointed to by ip and advance ip
           given opcode, get right C code that implements instruction's semantics
           First byte of instruction will always be opcode. This process is bytecode dispacth
           MOST IMPORTANT PART OF INTERPRETER from a performance perspective
        */
        switch (instruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            Vm_Push(constant);
            printf("\n");
            break;
        }
        case OP_NIL:
            Vm_Push(NIL_VAL);
            break;
        case OP_TRUE:
            Vm_Push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            Vm_Push(BOOL_VAL(false));
            break;
        case OP_POP:
            Vm_Pop();
            break;
        case OP_GET_LOCAL:
        {
            uint8_t slot = READ_BYTE();
            Vm_Push(vm.stack[slot]);
            break;
        }
        case OP_SET_LOCAL:
        {
            uint8_t slot = READ_BYTE();
            vm.stack[slot] = peek(0);
            break;
        }
        case OP_GET_GLOBAL:
        {
            ObjString *name = READ_STRING();
            Value value;
            if (!tableGet(&vm.globals, name, &value))
            {
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            Vm_Push(value);
            break;
        }
        case OP_DEFINE_GLOBAL:
        {
            ObjString *name = READ_STRING();
            tableSet(&vm.globals, name, peek(0));
            Vm_Pop();
            break;
        }
        case OP_SET_GLOBAL:
        {
            ObjString *name = READ_STRING();
            if (tableSet(&vm.globals, name, peek(0)))
            {
                tableDelete(&vm.globals, name);
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_EQUAL:
        {
            Value b = Vm_Pop();
            Value a = Vm_Pop();
            Vm_Push(BOOL_VAL(valueEquals(a, b))); // can == on ANY pair of objects
            break;
        }
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_ADD:
        {
            // String contatencation SUPPORTED NICE
            if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
            {
                concatenate();
            }
            else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
            {
                double b = AS_NUMBER(Vm_Pop());
                double a = AS_NUMBER(Vm_Pop());
                Vm_Push(NUMBER_VAL(a + b));
            }
            else
            {
                runtimeError(
                    "Operands must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_NEGATE:
            // ensure Value type being used for negation is a number
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            Vm_Push(NUMBER_VAL(-AS_NUMBER(Vm_Pop())));
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
            Vm_Push(BOOL_VAL(isFalsey(Vm_Pop())));
            break;
        case OP_PRINT:
        {
            // TODO: fix color printing here
            printValue(Vm_Pop(), 31);
            printf("\n");
            break;
        }
        case OP_JUMP:
        {
            uint16_t offset = READ_SHORT();
            vm.ip += offset;
            break;
        }
        case OP_JUMP_IF_FALSE:
        {
            // offset tells us how much to increment ip in the event that the if statement is false.
            // offset will jump over the body of the conditional
            uint16_t offset = READ_SHORT();
            if (isFalsey(peek(0)))
                vm.ip += offset;
            break;
        }
        case OP_LOOP:
        {
            uint16_t offset = READ_SHORT();
            vm.ip -= offset;
            break;
        }
        case OP_RETURN:
        {
            // Exit interpreter
            return INTERPRET_OK;
        }
        }
    }
#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef BINARY_OP
#undef READ_STRING
}

InterpretResult Vm_Interpret(const char *source)
{
    // create new empty chunk
    Chunk chunk;
    Chunk_InitChunk(&chunk);

    // take user program and fill chunk w bytecode
    if (!Compiler_Compile(source, &chunk))
    {
        // if errors in the program, free chunk and ERROR
        Chunk_FreeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    // else compiled chunk will be executed by vm
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    // execute chunk
    InterpretResult result = Vm_Run();

    Chunk_FreeChunk(&chunk);
    return result;
}

void Vm_InitVm()
{
    vm.objects = NULL;
    initTable(&vm.strings);
    Vm_ResetStack(); // VM state must be initialized
}

void Vm_FreeVm()
{
    initTable(&vm.globals);
    freeTable(&vm.strings);
    freeObjects();
}

void Vm_Push(Value value)
{
    // TODO: Errork checking here, dynamic expand stack??
    *vm.stackTop = value; // deref and save value into stack
    vm.stackTop++;        // move top of stack to next entry
}

Value Vm_Pop()
{
    // TODO: Errork checking here
    vm.stackTop--;
    return *vm.stackTop;
}