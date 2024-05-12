#pragma once

#include "common.h"
#include "memory.h"

#define MIN_ARR_THRESHOLD 8
#define ARR_GROWTH_FACTOR 2

#define ALLOCATE(type, count) \
    (type *)reallocate(NULL, 0, sizeof(type) * (count))

/**
 * Macro to calculate the new capacity for dynamic memory growth.
 * If the current capacity is less than 8 (a byte), the new capacity will be 8.
 * Otherwise, the new capacity will be ARR_GROWTH_FACTOR times the current capacity.
 *
 * @param capacity The current capacity of the dynamic memory.
 * @return The new capacity for dynamic memory growth.
 */
#define GROW_CAPACITY(capacity) \
    ((capacity) < MIN_ARR_THRESHOLD ? MIN_ARR_THRESHOLD : (capacity) * ARR_GROWTH_FACTOR)

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

/**
 * @brief Macro for freeing an array of a specified type.
 *
 * This macro is used to free memory allocated for an array of a specified type.
 * It takes three parameters: the type of the array elements, the pointer to the
 * array, and the old count of elements in the array. The macro internally calls
 * the `reallocate` function to free the memory.
 *
 * @param type The type of the array elements.
 * @param pointer The pointer to the array.
 * @param oldCount The old count of elements in the array.
 */
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

/**
 * @brief Macro for growing an array dynamically.
 *
 * This macro takes in the type of the array elements, a pointer to the array,
 * the current count of elements in the array, and the desired new count of elements.
 * It reallocates the memory for the array to accommodate the new count of elements,
 * and returns a pointer to the reallocated memory.
 *
 * @param type The type of the array elements.
 * @param pointer A pointer to the array.
 * @param oldCount The current count of elements in the array.
 * @param newCount The desired new count of elements.
 * @return A pointer to the reallocated memory for the array.
 */
#define GROW_ARRAY(type, pointer, oldCount, newCount)      \
    (type *)reallocate(pointer, sizeof(type) * (oldCount), \
                       sizeof(type) * (newCount))

void *reallocate(void *pointer, size_t oldSize, size_t newSize);
void freeObjects();