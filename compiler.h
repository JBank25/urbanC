#pragma once

#include "object.h"
#include "vm.h"

#include <stdbool.h>

bool compile(const char *source, Chunk *chunk);
