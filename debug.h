#pragma once

#include "chunk.h"

#define ANSI_COLOR_RED 31
#define ANSI_COLOR_GREEN 32
#define ANSI_COLOR_YELLOW 33
#define ANSI_COLOR_BLUE 34
#define ANSI_COLOR_MAGENTA 35
#define STACK_ANSI_COLOR_CYAN 36

/**
 * @brief Prints text in color to the console.
 *
 * @param text - text to print
 * @param color_code - ANSI color code
 */
void Print_Color(const char *text, int color_code);
void disassembleChunk(Chunk *chunk, const char *name);
int disassembleInstruction(Chunk *chunk, int offset);