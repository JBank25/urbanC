#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

ObjFunction *newFunction()
{
    // allocate mem and initialize obj header so VM knows what type of obj it is
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

static ObjString *allocateString(char *chars, int length, uint32_t hash)
{
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);
    return string;
}

// This is FNV-1a
static uint32_t hashString(const char *key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++)
    {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString *takeString(char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash); // look for string
    if (interned != NULL)                                                    // if we find it
    {
        FREE_ARRAY(char, chars, length + 1); // free memory for the string that was passed int
        return interned;                     // and return the FOUND string
    }
    return allocateString(chars, length, hash);
}

ObjString *copyString(const char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL)
        return interned;
    char *heapChars = ALLOCATE(char, length + 1); // allocate new arr on heap big enough for string and null terminator
    memcpy(heapChars, chars, length);             // copy chars
    heapChars[length] = '\0';                     // copy chars
    return allocateString(heapChars, length, hash);
}

/**
 * @brief - Print a function
 *
 * @param function - the function to print
 */
static void printFunction(ObjFunction *function)
{
    printf("<function: %s>", function->name->chars);
}

/**
 * @brief Print a given object
 *
 * @param value - the object to print
 */
void printObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
    // functions are first-class objects
    case OBJ_FUNCTION:
        printFunction(AS_FUNCTION(value));
        break;
    case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    }
}