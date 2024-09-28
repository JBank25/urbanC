#include "memory.h"
#include "object.h"
#include "vm.h"

#include <stdlib.h>

void *Memory_Reallocate(void *pointer, size_t oldSize, size_t newSize)
{
    /*
     * Cases:
     * 0 to non-zero: new block will need to be allocated
     * non-zero to size LESS than oldSize: shrink existing allocation
     * non-zero to size GREATER than oldSize: grow existing allocation
     * let C standard library handle reallocation to new size.
     * if space
     */
    if (newSize == 0)
    {
        free(pointer);
        return NULL;
    }

    // lean on C standard library to handle reallocation
    void *result = realloc(pointer, newSize);

    // if allocation fails
    if (result == NULL)
    {
        exit(1);
    }
    return result;
}

/**
 * @brief Return the bits you borrowed back to the OS. A destructor-like function.
 *
 * @param object - object whose mem we are freeing
 */
static void freeObject(Obj *object)
{
    switch (object->type)
    {
    case OBJ_FUNCTION:
    {
        ObjFunction *function = (ObjFunction *)object;
        freeChunk(&function->chunk);
        FREE(ObjFunction, object);
        break;
    }
    case OBJ_STRING:
    {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY(char, string->chars, string->length + 1);
        FREE(ObjString, object);
        break;
    }
    }
}

void freeObjects()
{
    Obj *object = vm.objects;
    while (object != NULL)
    {
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}