#pragma once

#include "common.h"

/**
 * Macro to calculate the new capacity for dynamic memory growth.
 * If the current capacity is less than 8 (a byte), the new capacity will be 8.
 * Otherwise, the new capacity will be twice the current capacity.
 *
 * @param capacity The current capacity of the dynamic memory.
 * @return The new capacity for dynamic memory growth.
 */

#define MIN_ARR_THRESHOLD 8
#define ARR_GROWTH_FACTOR 2

#define GROW_CAPACITY(capacity) \
    ((capacity) < MIN_ARR_THRESHOLD ? MIN_ARR_THRESHOLD : (capacity) * ARR_GROWTH_FACTOR)

#define GROW_ARRAY(type, pointer, oldCount, newCount)      \
    (type *)reallocate(pointer, sizeof(type) * (oldCount), \
                       sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void *reallocate(void *pointer, size_t oldSize, size_t newSize);
