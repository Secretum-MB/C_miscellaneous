/** Makes available my implementation of hash tables with chaining
 **/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H


struct hashTable;
typedef struct hashTable hashTable;


/* node to store an element within the hash table.
 * The key property is a string and is what is hashed.
 * You may add keys of other types as you wish but will
 * then need to provide a bridge that converts your key
 * to a string and store it in key property before using
 * hash table functions, see bottom of file.
 */
typedef struct nodeHashTable {
  char  *key;
  int    key_int;
  double key_double;
  int    value;
  struct nodeHashTable *next;
  struct nodeHashTable *prev;
} nodeHashTable;


/* initializes on the Heap a node with a string key. */
static
nodeHashTable* nodeHashTable_string(char *key, int value)
{
  nodeHashTable *new = (nodeHashTable *)malloc(sizeof(*new));
  if (new == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  new->key = (char*)malloc(sizeof(*key) * strlen(key));
  if (new->key == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  new->key_int = 0;
  new->key_double = 0.0;
  new->value = value;
  new->next = new->prev = NULL;
  memcpy(new->key, key, sizeof(*key) * strlen(key));

  return new;
}


/* Create a new, empty, hash table.
 * @return returns a pointer to the hash table on the heap
 */
hashTable* hashTableBuild();

/* Free all memory allocated for the hash table, including all nodes
 * @NOTE if you wish to preserve the nodes, call free(table) instead
 * @NOTE frees nodes assuming memory was allocated seperately to both key and to struct
 */
void hashTableFree(hashTable *table);

/* Free memory allocated to a single node.
 * @NOTE assumes memory was allocated seperately to both key and to struct only
 */
void hashTableFreeNode(nodeHashTable *node);

/* Prints to stdout a textual representation of
 * the hash table and its linked lists
 */
void printHashTable(hashTable *table);


/*              Simple functionality
 *          No need to use above node struct
 *          (do not mix and match between the two)
 *          Dictionary is [String: Int] type
 */

/* Hash table basic insert
 * @param table the address of the pointer to the hash table
 * @param key the key of the element to insert
 * @param value the value of the element to insert
 * @NOTE if key already exists in table it is overwritten
 */
void hashTableInsert(hashTable **table, char *key, int value);

/* Hash table basic search for membership.
 * @param table pointer to the hash table
 * @param key the key of the element to find
 * @return pointer to the found node or NULL.
 */
nodeHashTable* hashTableSearch(hashTable *table, char *key);

/* Hash table basic delete
 * @param table the address of the pointer to the hash table
 * @param key the key of the element to delete
 * @return returns 1 if delete was made, otherwise 0
 */
int hashTableDelete(hashTable **table, char *key);


/*              Functions for working with nodes
 *          Assumes node has been allocated and its key
 *          property been appropriately set and ready for hashing.
 */

/* Hash table insert
 * @param table the address of the pointer to the hash table
 * @param node the node to be inserted into the hash table
 * @return if node with same key exists, it is overwritten and pointer
 *   to it is returned, so user may free it, if desired. Otherwise, NULL.
 */
nodeHashTable* hashTableInsertNode(hashTable **table, nodeHashTable *node);

/* Hash table search for membership.
 * @param table pointer to the hash table
 * @param key the key that was used to initialize the node and to be found
 * @param keyConvert function that will produce hashable key from alternative key
 *   @param key alternative key to convert to hashable key
 *   @return array containing hashable key
 *   @NOTE expects array to be on heap, hashTableSearchNode will free memory
 *   @NOTE for nodes with string key, keyConvert should be NULL
 * @return pointer to the found node or NULL.
 */
nodeHashTable* hashTableSearchNode(hashTable *table, void* key,
				   char* (*keyConvert)(void* key));

/* Hash table delete - does not free the deleted node from memory
 * @param table the address of the pointer to the hash table
 * @param node the node to be deleted from the hash table
 * @CAUTION assumes node is in hash table, behavior undefined otherwise
 */
void hashTableDeleteNode(hashTable **table, nodeHashTable *node);


/*              Bridging Functions
 *          Functions to allocate nodes with keys of type other
 *          than strings. These functions allocate nodes on the heap
 *          and set the node's key property suitable for hashing.
 */

/*                 INTEGERS                       */

/* Determine a node's hashable key from an integer key type
 * @param key alternative key utilized by node / hashtable
 * @return pointer to array containing the hashable key
 */
static
char* keyConvertFromInt(void* key)
{
  int key_size_needed = snprintf(NULL, 0, "%d", *(int*)key) + 1;
  char *hashable_key = (char*)malloc(key_size_needed);
  snprintf(hashable_key, key_size_needed, "%d", *(int*)key);
  return hashable_key;
}

/* initializes on the Heap a node with an integer key. */
static
nodeHashTable* nodeHashTable_int(int key, int value)
{
  nodeHashTable *new = (nodeHashTable *)malloc(sizeof(*new));
  if (new == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  int key_size_needed = snprintf(NULL, 0, "%d", key) + 1;
  new->key = (char*)malloc(key_size_needed);
  if (new->key == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  new->key_int = key;
  new->key_double = 0.0;
  new->value = value;
  new->next = new->prev = NULL;

  char *hashable_key = keyConvertFromInt(&key);
  memcpy(new->key, hashable_key, key_size_needed);

  free(hashable_key);
  return new;
}

/*                 DOUBLES                        */

/* Determine a node's hashable key from a double key type
 * @param key alternative key utilized by node / hashtable
 * @return pointer to array containing the hashable key
 */
static
char* keyConvertFromDouble(void* key)
{
  int key_size_needed = snprintf(NULL, 0, "%.8f", *(double*)key) + 1;
  char *hashable_key = (char*)malloc(key_size_needed);
  snprintf(hashable_key, key_size_needed, "%.8f", *(double*)key);
  return hashable_key;
}

/* initializes on the Heap a node with an double key. */
static
nodeHashTable* nodeHashTable_double(double key, int value)
{
  nodeHashTable *new = (nodeHashTable *)malloc(sizeof(*new));
  if (new == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  int key_size_needed = snprintf(NULL, 0, "%.8f", key) + 1;
  new->key = (char*)malloc(key_size_needed);
  if (new->key == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  new->key_double = key;
  new->key_int = 0;
  new->value = value;
  new->next = new->prev = NULL;

  char *hashable_key = keyConvertFromDouble(&key);
  memcpy(new->key, hashable_key, key_size_needed);

  free(hashable_key);
  return new;
}

/*                 STRUCT_t                       */


#endif

