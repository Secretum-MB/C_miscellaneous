/*     HASH TABLES - chaining implementation
  Hash tables are the best implementation for the dictionary data structure.
  They support Insert, Search, Delete and can perform each in O(1) time.

  A hash table is an array of pointers to nodes which form a linked list. The
  shorter these linked lists are, the better the performance of Search/Delete.
  (If delete is made from a doubly linked list, where the node does not need
  to be found first, Delete always performs in O(1) time.)
  The length of the linked lists are referred to as alpha, or the load factor.
  Ideally, the size of the array equals the number of elements in the hash table.
  We use Table-Doubling to double the size of the array when the number of elements
  becomes equal to the size of the array. If deletion causes the number of elements
  to become less than or equal to 1/4 the array size, we cut the size of the table
  to 1/2 of its original size. (1/4 to avoid yo-yo'ing table size on single ins/del.)
  (These scalling factors important to avoid frequent re-sizings, see below.)

  Technically, constant time is achieved by Search/Delete per an amortized
  analysis. The reason is the table size, an array, needs to be strategically
  resized (grow/shrink) as the number of elements changes. This requires that all
  the elements in the original table be re-hashed and inserted into the new table.
  This requires that we loop over the original table and each element in its
  linked list. This process is slow but occurs seldom (as table doubles each time,
  growth is exponential and less likely to be needed as table grows), so we spread
  the costs of such re-sizeing accross all operations of the hash table.
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/hashTables.h"


struct hashTable {
  unsigned int numElements;
  unsigned int tableSize;
  unsigned int structSize;
  struct nodeHashTable* table[];
};


/* djb2 hash algorithm by Dan Bernstein */
static unsigned long hash(unsigned char *str)
{
  unsigned long hash = 5381;
  int c;

  while ( (c = *str++) )
    hash = ((hash << 5) + hash) + c;

  return hash;
}


/*                            */
/*    TABLE DOUBLING          */
/*                            */

static hashTable* hashTable_init(u_int size)
{
  // struct lives contigious in memory, flexible array member gets last bytes.
  // its size not explicit, this is why it must be declared last in struct!
  hashTable *new = malloc(sizeof(*new) + sizeof(struct nodeHashTable*) * size);
  new->numElements = 0;
  new->tableSize = size;
  new->structSize = (sizeof(*new) + sizeof(struct nodeHashTable*) * size);

  for (size_t i = 0; i < size; i++) {
    new->table[i] = NULL;
  }
  return new;
}


/* moves a node from one hashTable into hashTable identified in the parameter
 */
static void moveNode(hashTable *table, nodeHashTable *node)
{
  unsigned long hash_value = hash(node->key);
  unsigned int hash_pos = hash_value % table->tableSize;

  // node already exists in hash slot. Realize no duplicate keys are possible as
  // node comes from another, valid, hashTable
  nodeHashTable *head;
  if ( (head = table->table[hash_pos]) != NULL) {
    head = table->table[hash_pos];
    head->prev = node;
    table->table[hash_pos] = node;
    node->next = head;
    node->prev = NULL;
    table->numElements++;

  // no node exists at hash slot.
  } else {
    table->table[hash_pos] = node;
    node->next = node->prev = NULL;
    table->numElements++;
  }
}


static void hashTable_resize(hashTable **table, size_t size)
{
  hashTable *new_table = hashTable_init(size);

  // rehash all nodes in existing table and place into new_table
  for (size_t i = 0; i < (*table)->tableSize; i++) {
    if ( (*table)->table[i]) {
      nodeHashTable *curr = (*table)->table[i];
      nodeHashTable *next;

      while (curr != NULL) {
	next = curr->next;
	moveNode(new_table, curr);
	curr = next;
      }
    }
  }
  hashTable *old_table = *table;
  *table = new_table;
  free(old_table);
}


/*                            */
/*    HASH TABLE FUNCTIONS    */
/*                            */

/* Creates a new hash table. Returns a pointer to table. */
hashTable* hashTableBuild()
{
  return hashTable_init(8);
}


/* Basic insert function mimics modern language usability.
 * Provide it with a table and an string, int for key, value.
 */
void hashTableInsert(hashTable **table, u_char *key, int value)
{
  nodeHashTable *new = malloc(sizeof(*new));
  new->key = malloc(sizeof(*key) * strlen(key));
  if (new == NULL || new->key == NULL) {
    perror("malloc");
    fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  memcpy(new->key, key, sizeof(char) * strlen(key));
  new->value = value;
  new->key_int = 0;
  new->prev = NULL;

  unsigned long hash_value = hash(key);
  unsigned int hash_pos = hash_value % (*table)->tableSize;

  // prevent duplicate keys
  // replace node instead of changing its value: better in case the node struct changes
  nodeHashTable *prev = NULL;
  nodeHashTable *head;
  if ( (head = (*table)->table[hash_pos]) != NULL) {
    while (head != NULL) {
      if (strcmp(head->key, key) == 0) {
	if (prev == NULL) {
	  (*table)->table[hash_pos] = new;
	} else {
	  prev->next = new;
	}
	new->next = head->next;
	free(head->key);
	free(head);
	return;
      }
      prev = head;
      head = head->next;
    }

    // no duplicate found: insert at head of list
    head = (*table)->table[hash_pos];
    (*table)->table[hash_pos] = new;
    new->next = (struct nodeHashTable*) head;
    (*table)->numElements++;

  // no linked list yet in this slot
  } else {
    (*table)->table[hash_pos] = new;
    new->next = NULL;
    (*table)->numElements++;
  }

  // if table has been made full, double its size
  if ( (*table)->numElements == (*table)->tableSize) {
    hashTable_resize(table, (*table)->tableSize * 2);
  }
}


nodeHashTable* hashTableSearch(hashTable *table, u_char *key)
{
  unsigned long hash_value = hash(key);
  unsigned int hash_pos = hash_value % table->tableSize;

  nodeHashTable *head;
  if ( (head = table->table[hash_pos]) == NULL) {
    return NULL;
  } else {
    while (head != NULL) {
      if (strcmp(head->key, key) == 0) {
	return head;
      }
      head = head->next;
    }
  }
  return NULL;
}


int hashTableDelete(hashTable **table, u_char *key)
{
  unsigned long hash_value = hash(key);
  unsigned int hash_pos = hash_value % (*table)->tableSize;

  nodeHashTable *head;
  nodeHashTable *prev = NULL;
  if ( (head = (*table)->table[hash_pos]) == NULL) {
    return 0;
  } else {
    while (head != NULL) {
      if (strcmp(head->key, key) == 0) {
	(*table)->numElements--;
	if (prev == NULL) {
	  (*table)->table[hash_pos] = head->next;
	} else {
	  prev->next = head->next;
	}
	free(head->key);
	free(head);

	// resize table if numElements <= 1/4 tableSize
	if ((*table)->tableSize > 8
            && (*table)->numElements <= (*table)->tableSize / 4) {
	  hashTable_resize(table, (*table)->tableSize / 2);
	}
	return 1;
      }
      prev = head;
      head = head->next;
    }
  }
  return 0;
}


nodeHashTable* hashTableInsertNode(hashTable **table, nodeHashTable *node)
{
  unsigned long hash_value = hash(node->key);
  unsigned int hash_pos = hash_value % (*table)->tableSize;

  // prevent duplicate keys, if duplicates exist: replace node with current
  nodeHashTable *prev = NULL;
  nodeHashTable *head;
  if ( (head = (*table)->table[hash_pos]) != NULL) {
    while (head != NULL) {
      if (strcmp(head->key, node->key) == 0) {
	if (prev == NULL) {
	  (*table)->table[hash_pos] = node;
	  head->prev = node;
	} else {
	  prev->next = node;
	  node->prev = prev;
	  if (head->next) { head->next->prev = node; }
	}
	node->next = head->next;
	return head;
      }
      prev = head;
      head = head->next;
    }

    // no duplicate found: insert at head of list
    head = (*table)->table[hash_pos];
    (*table)->table[hash_pos] = node;
    node->next = head;
    head->prev = node;
    (*table)->numElements++;

  // no linked list yet in this slot
  } else {
    (*table)->table[hash_pos] = node;
    (*table)->numElements++;
  }

  // if table has been made full, double its size
  if ( (*table)->numElements == (*table)->tableSize) {
    hashTable_resize(table, (*table)->tableSize * 2);
  }
  return NULL;
}


nodeHashTable* hashTableSearchNode(hashTable *table, void* key,
				   u_char* (*keyConvert)(void*))
{
  u_char *hashable_key;
  if (keyConvert != NULL) {           // node with alternate key type
    hashable_key = keyConvert(key);
  } else {                            // nodes instantiated with string key
    hashable_key = (u_char*)key;
  }
  unsigned long hash_value = hash(hashable_key);
  unsigned int hash_pos = hash_value % table->tableSize;

  nodeHashTable *head;
  if ( (head = table->table[hash_pos]) == NULL) {}
  else {
    while (head != NULL) {
      if (strcmp(head->key, hashable_key) == 0) {
	if (keyConvert) { free(hashable_key); }
	return head;
      }
      head = head->next;
    }
  }
  if (keyConvert) { free(hashable_key); }
  return NULL;
}


/* fast implementation: does not check if node is in hashTable */
void hashTableDeleteNode(hashTable **table, nodeHashTable *node)
{
  if (!node->prev) {
    unsigned long hash_value = hash(node->key);
    unsigned long hash_pos = hash_value % (*table)->tableSize;

    (*table)->table[hash_pos] = node->next;
  } else {
    node->prev->next = node->next;
  }
  if (node->next) { node->next->prev = node->prev; }

  // resize table if numElements <= 1/4 tableSize
  (*table)->numElements--;
  if ((*table)->tableSize > 8
      && (*table)->numElements <= (*table)->tableSize / 4) {
    hashTable_resize(table, (*table)->tableSize / 2);
  }
}


/* frees table and all nodes within it */
void hashTableFree(hashTable *table)
{
  for (size_t i = 0; i < table->tableSize; i++) {
    if (table->table[i] != NULL) {
      nodeHashTable *prev = NULL;
      nodeHashTable *head = table->table[i];
      while (head != NULL) {
	free(head->key);
	prev = head;
	head = head->next;
	free(prev);
      }
    }
  }
  free(table);  // no need to free table->table; malloced together
}


void hashTableFreeNode(nodeHashTable *node)
{
  free(node->key);
  free(node);
}


void printHashTable(hashTable *table)
{
  printf("tbl_size: %d; num_elements: %d; struct_size: %d\n",
	 table->tableSize, table->numElements, table->structSize);

  for (size_t i = 0; i < table->tableSize; i++) {
    if (table->table[i] != NULL) {
      nodeHashTable *head = table->table[i];
      while (head != NULL) {
	printf("[%s:%d] -> ", head->key, head->value);
	head = head->next;
      }
      printf("\n");
    } else { printf("\\\n"); }
  }
}




/* Functions below demonstrate the various interface for the Hash Table */

static void sample_usage_of_hash_table_no_nodes()
{
  hashTable *table_two = hashTableBuild();

  hashTableInsert(&table_two, "Fish", 9);
  hashTableInsert(&table_two, "Tacos", 3);
  hashTableInsert(&table_two, "Burritos", 1);
  hashTableInsert(&table_two, "Strawberries", 3);
  hashTableInsert(&table_two, "Fish", 99);         // overwrites original Fish
  hashTableInsert(&table_two, "Tomatoes", 0);
  hashTableInsert(&table_two, "Grapes", -1);
  hashTableInsert(&table_two, "Pears", 2);
  hashTableInsert(&table_two, "Fish", 7);          // overwrites Fish again
  hashTableInsert(&table_two, "Pork", 4);
  hashTableInsert(&table_two, "Chicken", 12);

  printHashTable(table_two);
  printf("\n");

  printf("find Fish:  %p \n", hashTableSearch(table_two, "Fish"));
  printf("find Pork:  %p \n", hashTableSearch(table_two, "Pork"));
  printf("find Toads: %p \n", hashTableSearch(table_two, "Toads"));
  printf("find FISH:  %p \n", hashTableSearch(table_two, "FISH"));
  printf("\n");

  printf("deleting Fish:  %d \n", hashTableDelete(&table_two, "Fish"));
  printf("deleting Pork:  %d \n", hashTableDelete(&table_two, "Pork"));
  printf("deleting Toads: %d \n", hashTableDelete(&table_two, "Toads"));
  printf("deleting Fish:  %d \n", hashTableDelete(&table_two, "Fish"));
  printf("\n");

  printHashTable(table_two);
  hashTableFree(table_two);
}


static void sample_usage_of_hash_table_with_nodes()
{
  hashTable *table = hashTableBuild();

  for (int i = 0; i < 25; i++) {
    hashTableInsertNode(&table, nodeHashTable_int(i, i*2+3-15));
  }
  printHashTable(table);

  for (int i = 0; i < 17; i++) {
    nodeHashTable *to_delete = hashTableSearchNode(table, &i, keyConvertFromInt);
    hashTableDeleteNode(&table, to_delete);
    hashTableFreeNode(to_delete);
  }
  printHashTable(table);

  hashTableFree(table);
}

