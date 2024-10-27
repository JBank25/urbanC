#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void repl()
{
    char line[1024];
    for (;;)
    {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin))
        {
            printf("\n");
            break;
        }
    }
}

static char *readFile(const char *path)
{
    FILE *file = fopen(path, "rb"); // stdio File struct
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);     // seek to end BEFORE reading it
    size_t fileSize = ftell(file); // tells us how many bytes from start of the file
    rewind(file);                  // reset file back to beginning

    char *buffer = (char *)malloc(fileSize + 1); // +1 for NULL byte
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char *path)
{
    char *source = readFile(path);                 // read file of code
    InterpretResult result = Vm_Interpret(source); // EXECUTE (interpret) the code
    free(source);                                  // free result I guess its on Heap ??

    if (result == INTERPRET_COMPILE_ERROR)
        exit(65);
    if (result == INTERPRET_RUNTIME_ERROR)
        exit(70);
}

int main(int argc, const char *argv[])
{

    Vm_InitVm();

    // no args then drop into REPL
    // if (argc == 1)
    // {
    //     repl();
    // }
    // Should be path to a script to run
    // else if (argc == 2)
    // {
    runFile("./test.txt");
    // }
    // else
    // {
    //     fprintf(stderr, "Usage: clox [path]\n");
    //     exit(64);
    // }

    Vm_FreeVm();
    exit(0);
}
