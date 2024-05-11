#pragma once

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

typedef enum
{
    OBJ_STRING,
} ObjType;

struct Obj
{
    ObjType type;
};

struct ObjString
{
    Obj obj;     // ObjString is ALSO an Obj, first field will thus be an Obj
    int length;  // Length of string
    char *chars; // ptr to heap allocated aray
};

ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
void printObject(Value value);