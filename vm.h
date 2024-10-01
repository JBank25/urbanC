#pragma once

#include "chunk.h"

#include "table.h"
#include "value.h"

#define STACK_MAX 256

typedef struct
{
    Chunk *chunk;
    uint8_t *ip; // location of next byte to be executed. Will point to location in bytecode array to be executed
    Value stack[STACK_MAX];
    /* points at element just PAST element containing top, stack is EMPTY when pointing
     *  at 0 element in array
     */
    Value *stackTop;
    Table globals;
    Table strings; // STRING INTERNING
    Obj *objects;  // VM store a ptr to head of LL
} VM;

/**
 * Vm will run a chunk and respond with a value from here
 */
typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void Vm_InitVm();
void Vm_FreeVm();

/**
 * @brief Takes user code and populated a chunk with bytecode. Then executes the chunk.
 *
 * @param source - user code
 * @return InterpretResult - INTERPRET_OK if no errors, INTERPRET_COMPILE_ERROR if compilation error, INTERPRET_RUNTIME_ERROR if runtime error
 */
InterpretResult Vm_Interpret(const char *source);
void Vm_Push(Value value);
Value Vm_Pop();