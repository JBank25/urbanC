#pragma once

#include "object.h"
#include "vm.h"

#include <stdbool.h>

bool Compiler_Compile(const char *source, Chunk *chunk);
