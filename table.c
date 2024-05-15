#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>

// Should this go into header? Maybe not, only user in this file
#define TABLE_MAX_LOAD 0.75

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
 * Finds an entry in the given array of entries with the specified key.
 * This function will be used to both find existing entries in the hash
 * table and for deciding where to insert new ones.
 *
 * @param entries   The array of entries to search.
 * @param capacity  The capacity of the array.
 * @param key       The key to search for.
 * @return          A pointer to the found entry OR ptr to empty entry which can then be populated.
 *                  caller may want to read the value, update it, or create a new entry
 */
static Entry *findEntry(Entry *entries, int capacity, ObjString *key)
{
    // modulo to map key's hash code into an index within the arrays bounds
    uint32_t index = key->hash % capacity;
    for (;;) // <- loop to probe in event of collision, no infinite loop due load factor checking
    {
        // grab the entry
        Entry *entry = &entries[index];
        // if key already exists OR the entry is empty, return it to be populated, updated, etc
        if (entry->key == key || entry->key == NULL)
        {
            return entry;
        }
        // if collision occurred, start probing
        index = (index + 1) % capacity;
    }
}

/**
 * Adjusts the capacity of the table to the specified value. This function ALSO copies over the
 * existing values from the hash table into the new one. Recall that in our hash function we
 * take the hashed key % array size to get the index. Meaning when the array size changes so too
 * do the buckets we should allocate them into.
 *
 * @param table The table to adjust the capacity of.
 * @param capacity The new capacity of the table.
 */
static void adjustCapacity(Table *table, int capacity)
{
    Entry *entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++)
    {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }

    // Copying over ALL the values in the hash table before we adjust capacity to the
    // new hash table
    for (int i = 0; i < table->capacity; i++)
    {
        Entry *entry = &table->entries[i];
        if (entry->key == NULL) // if we find an empty bucket, continue nothing to store here
            continue;

        // for buckets that do have values we find the correct bucket for the adjusted capacity table
        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key; // and store the kay and value there
        dest->value = entry->value;
    }

    table->entries = entries;   // store the array
    table->capacity = capacity; // and its capacity into hash tables struct
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
    // TABLE_MAX_LOAD is how we manage the tables load factor. Could be say
    // .75 so we only increase when the array is at least 75% full
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
    {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    // finds entry in hashtable, this var will hold a ptr to that memory location
    Entry *entry = findEntry(table->entries, table->capacity, key);

    bool isNewKey = (entry->key == NULL); // true if the key/val is new
    if (isNewKey)
    {
        table->count++; // only increase count when it is a new ley/val
    }

    entry->key = key;     // populate the hash table with the key
    entry->value = value; // and value
    return isNewKey;
}

/**
 * @brief Copies all the entries of one hash table into another.
 *
 * @param from - Hash table we are copying values from
 * @param to - Hash table we are bopying values to
 */
void tableAddAll(Table *from, Table *to)
{
    for (int i = 0; i < from->capacity; i++)
    {
        Entry *entry = &from->entries[i];
        if (entry->key != NULL)
        {
            // utilize table set. "to" is the table being added to, key and value are what will be stored there
            tableSet(to, entry->key, entry->value);
        }
    }
}