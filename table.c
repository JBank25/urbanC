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
    table->count = 0; // Number of entries + number of tombstones
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
    Entry *tombstone = NULL;
    for (;;) // <- loop to probe in event of collision, no infinite loop due load factor checking
    {
        // grab the entry
        Entry *entry = &entries[index];
        if (entry->key == NULL) // this could mean tombstone is at this entry or it is empty
        {
            if (IS_NIL(entry->value))
            {
                // Empty entry. Return the tombstone if we have found one so we can reuse that
                // reclaimed space, else just return the entry itself
                return tombstone != NULL ? tombstone : entry;
            }
            else
            {
                // We found a tombstone.
                if (tombstone == NULL)
                    tombstone = entry; // set the tombstone value for later use, we may end up returning it
            }
        }
        else if (entry->key == key)
        {
            // We found the key.
            return entry;
        }
        // if collision occurred, start probing
        index = (index + 1) % capacity;
    }
}

/**
 * @brief Find an entry in the given hash table. If the entry exists, return true and populate
 * value ptr with the correct value. Else return False
 *
 * @param table - hash table we should search in
 * @param key - key being used to search in the hash table
 * @param value - value we should populate w the correct ptr if it exists in the table
 * @return true - if a value exists in "table" for "key"
 * @return false - else
 */
bool tableGet(Table *table, ObjString *key, Value *value)
{
    // if no entries are populated yet, then the "key" definitely doesn'y exist in "table"
    if (table->count == 0)
        return false;
    // search for the entry in "table" given the "key" and "capacity"
    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;

    // return true and the value if it exists
    *value = entry->value;
    return true;
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

    // tombstones are not copied over to new table. Count will need to be recalculated so we reset it
    table->count = 0;

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
        table->count++; // increment count every time we find non-tombstone entry
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
    //
    if (isNewKey && IS_NIL(entry->value))
    {
        table->count++; // only increase count when it is a new ley/val
    }

    entry->key = key;     // populate the hash table with the key
    entry->value = value; // and value
    return isNewKey;
}

/**
 * Deletes an entry from the table based on the given key. Note that we mark deleted entries
 * with a "tombstone". This is so if when we are searching for an entry using the probe sequence
 * we do not prematurely give up the search. Just setting deleted entries to NULL is kinda like
 * removing a node from a LL and not connecting the nodes that came before and after it (creating a
 * disjoint chain).
 *
 * @param table The table from which to delete the entry.
 * @param key The key of the entry to delete.
 * @return true if the entry was successfully deleted, false otherwise.
 */
bool tableDelete(Table *table, ObjString *key)
{
    // if no entries then nothing to delete
    if (table->count == 0)
        return false;

    // Find the entry.
    Entry *entry = findEntry(table->entries, table->capacity, key);
    // if no entry exists then nothing to delete
    if (entry->key == NULL)
        return false;

    // Place a tombstone in the entry.
    entry->key = NULL;
    entry->value = BOOL_VAL(true);
    return true;
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

/**
 * @brief find an entry in the string table
 *
 * @param table - table to search through
 * @param chars - raw character array of the key we are looking for
 * @param length
 * @param hash
 * @return ObjString*
 */
ObjString *tableFindString(Table *table, const char *chars, int length, uint32_t hash)
{
    // if no entries in table yet, no strings are interned, return NULL
    if (table->count == 0)
        return NULL;

    uint32_t index = hash % table->capacity;
    for (;;)
    {
        Entry *entry = &table->entries[index];
        if (entry->key == NULL)
        {
            // Stop if we find an empty non-tombstone entry.
            if (IS_NIL(entry->value))
                return NULL;
        }
        else if (entry->key->length == length &&
                 entry->key->hash == hash &&
                 memcmp(entry->key->chars, chars, length) == 0)
        {
            // We found it.
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}