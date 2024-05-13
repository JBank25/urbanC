#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>

void initTable(Table *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table *table)
{
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

/**
 * @brief Adds a given key/value to a hash table. If the key already exists
 * in the table then it is overridden
 *
 * @param table - hash table to add new key/value to
 * @param key - Key of value going into hash table
 * @param value - Value going into hash table
 *
 * @return true if the key creates a new entry into the hash table
 * false otherwise.
 */
bool tableSet(Table *table, ObjString *key, Value value)
{
    // finds entry in hashtable, this var will hold a ptr to that memory location
    Entry *entry = findEntry(table->entries, table->capacity, key);

    bool isNewKey = (entry->key == NULL);
    if (isNewKey)
        table->count++;

    entry->key = key;     // populate the hash table with the key
    entry->value = value; // and value
    return isNewKey;
}