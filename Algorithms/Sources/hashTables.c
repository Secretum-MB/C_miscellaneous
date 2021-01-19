/* I wanted to have nodes constructed within the hash table functions,
   therefore, deletion will need to search out the node with the key to
   delete before deleting it. This is not optimal performance, it's as slow
   as the search itself. [insert O(1); search/delete O(1) average assuming
   good hash function and appropriate table size]. If node construction outside
   then with a doubly linked list for chaining you can delete the node itself 
   instead of going through hashing and searching, achieving O(1) worst case.

   Another disadvantage to creating nodes within functions is that if you wanted
   to enhance the node struct, say adding more satellite data, you would have to 
   modify the functions!. You could avoid that and build generic functions if you
   just passed nodes into the functions.

   The advantage of my approach is that the interface/ or use of the data strucute
   is very simple and clean. No need to concern user with nodes.

   I took an unusual view of the delete function. instead of returning the node or
   its key, I am returning true if a delete was made, false if not. I originally 
   did this because multiple nodes could be accepted with the same key, so we wanted
   ability to delete same key until delete returned false. But I changed this so that
   Insert would replace the node that has the same key as the node being inserted.
   This is in functionality how Swift and Python treats duplicates being added.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1000


typedef enum bool {false, true} bool;


/* djb2 has algorithm by Dan Bernstein */
//
unsigned long hash(unsigned char *str) {

  unsigned long hash = 5381;
  int c;

  while ( (c = *str++) )
	hash = ((hash << 5) + hash) + c;

  return hash;
}


typedef struct nodeHashTable {
  char *key;
  int value;
  struct nodeHashTable *next;
} nodeHashTable;


void hashTableInsert(nodeHashTable **table, char *key, int value) {

  nodeHashTable *new = malloc(sizeof(*new));
  new->key = malloc(sizeof(*key) * strlen(key));
  if (new == NULL || new->key == NULL) {
	perror("malloc");
	fprintf(stderr, "failed to allocate memory");
	exit(EXIT_FAILURE);
  }

  memcpy(new->key, key, sizeof(char) * strlen(key));
  new->value = value;

  unsigned long hash_value = hash(key);
  int hash_pos = hash_value % MAX_SIZE;

  //prevent duplicate keys
  // replace node instead of changing its value: better in case the node struct changes
  nodeHashTable *prev = NULL;
  nodeHashTable *head;
  if ( (head = table[hash_pos]) != NULL) {
	while (head != NULL) {
	  if (strcmp(head->key, key) == 0) {
		if (prev == NULL) {
		  table[hash_pos] = new;
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
	head = table[hash_pos];
	table[hash_pos] = new;
	new->next = (struct nodeHashTable*) head;

  // no linked list yet in this slot
  } else {
	table[hash_pos] = new;
	new->next = NULL;
  }
}

bool hashTableSearch(nodeHashTable **table, char *key) {

  unsigned long hash_value = hash(key);
  int hash_pos = hash_value % MAX_SIZE;

  nodeHashTable *head;
  if ( (head = table[hash_pos]) == NULL) {
	return false;
  } else {
	while (head != NULL) {
	  if (strcmp(head->key, key) == 0) {
		return true;
	  }
	  head = head->next;
	}
  }
  return false;
}

bool hashTableDelete(nodeHashTable **table, char *key) {

  unsigned long hash_value = hash(key);
  int hash_pos = hash_value % MAX_SIZE;

  nodeHashTable *head;
  nodeHashTable *prev = NULL;
  if ( (head = table[hash_pos]) == NULL) {
	return false;
  } else {
	while (head != NULL) {
	  if (strcmp(head->key, key) == 0) {
		if (prev == NULL) {
		  table[hash_pos] = head->next;
		} else {
		  prev->next = head->next;
		}
		free(head->key);
		free(head);
		return true;
	  }
	  prev = head;
	  head = head->next;
	}
  }
  return false;
}

void HashTableFree(nodeHashTable **table) {

  for (int i = 0; i < MAX_SIZE; i++) {
	if (table[i] != NULL) {
	  nodeHashTable *prev = NULL;
	  nodeHashTable *head = table[i];
	  while (head != NULL) {
		free(head->key);
		prev = head;
		head = head->next;
		free(prev);
	  }
	}	
  }
}


void printHashTable(nodeHashTable **table) {
  for (int i = 0; i < MAX_SIZE; i++) {
	if (table[i] != NULL) {
	  nodeHashTable *head = table[i];
	  while (head != NULL) {
		printf("[%s:%d] -> ", head->key, head->value);
		head = head->next;
	  }
	  printf("\n");
	}
  }
}


// notice table is pointers to the nodes and we initialize table with NULL
// also, notice that at end we free all of the nodes from the heap.
//
int main(void) {

  nodeHashTable *table_one[MAX_SIZE];
  for (int i = 0; i < MAX_SIZE; i++) {
	table_one[i] = NULL;
  }

  char *key1 = "Fish";

  hashTableInsert(table_one, key1, 9);
  hashTableInsert(table_one, "Tacos", 3);
  hashTableInsert(table_one, "Burritos", 1);
  hashTableInsert(table_one, "Strawberries", 3);
  hashTableInsert(table_one, "Fish", 99);
  hashTableInsert(table_one, "Tomatoes", 0);
  hashTableInsert(table_one, "Grapes", -1);
  hashTableInsert(table_one, "Pears", 2);
  hashTableInsert(table_one, "Fish", 7);
  hashTableInsert(table_one, "Pork", 4);

  printf("find Fish:  %d \n", hashTableSearch(table_one, "Fish"));
  printf("find Pork:  %d \n", hashTableSearch(table_one, "Pork"));
  printf("find Toads: %d \n", hashTableSearch(table_one, "Toads"));
  printf("find FISH:  %d \n", hashTableSearch(table_one, "FISH"));
  printf("\n\n");

  printHashTable(table_one); printf("\n");

  printf("deleting Fish: %d \n", hashTableDelete(table_one, "Fish"));
  printf("deleting Pork: %d \n", hashTableDelete(table_one, "Pork"));
  printf("deleting Toads: %d \n", hashTableDelete(table_one, "Toads"));
  printf("deleting Fish: %d \n", hashTableDelete(table_one, "Fish"));
  printf("deleting Fish: %d \n", hashTableDelete(table_one, "Fish"));
  printf("deleting Fish: %d \n", hashTableDelete(table_one, "Fish"));
  printf("\n");

  printHashTable(table_one);

  HashTableFree(table_one);

  return EXIT_SUCCESS;
}

