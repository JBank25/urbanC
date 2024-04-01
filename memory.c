#include "memory.h"

#include <stdlib.h>

/**
 * Reallocates a block of memory to a new size.
 *
 * @param pointer The pointer to the memory block to be reallocated.
 * @param oldSize The size of the current memory block.
 * @param newSize The desired size of the new memory block.
 * @return A pointer to the reallocated memory block, or NULL if newSize is 0.
 */
void *reallocate(void *pointer, size_t oldSize, size_t newSize)
{
    // free the allocated memory and return NULL
    if (newSize == 0)
    {
        free(pointer);
        return NULL;
    }

    // Cases:
    // 0 to non-zero: new block will need to be allocated
    // non-zero to size LESS than oldSize: shrink existing allocation
    // non-zero to size GREATER than oldSize: grow existing allocation
    // let C standard library handle reallocation to new size.
    // if space
    void *result = realloc(pointer, newSize);

    // if allocation fails
    if (result == NULL)
    {
        exit(1);
    }
    return result;
}