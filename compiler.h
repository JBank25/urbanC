#pragma once

#include "vm.h"

#include <stdbool.h>

bool compile(const char *source, Chunk *chunk);
