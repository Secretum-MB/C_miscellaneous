/*
  Breadth-First Search (BFS)
  Given an directed or undirected graph and a source vertex, it will "explore" every reachable
  vertex in the graph. It begins on the souce node, it then visits all of the source vertex's
  neighbors and takes note that they have been visited, then proceeds to visit all of their neighbors,
  and so on until no vertex has neighbors that have not been visited.

  The algorithm returns the means to build a breadth-first tree. Either by mutating the vertices'
  sattelite data or by returning a data structure that cointains the necessary information.
  In this implementation a hash table is returned - BFS populates with 'predecessor': needed for tree.
  This tree will be rooted at the souce vertex of the BFS function and contain all vertices that
  are reachable from it. The simple path in the tree from the root (source) to any other vertex
  is the shortest possible path in the graph between these two vertices! Careful, the path between
  any two vertices, where neither is the source, is not neccessarily the shortest path between them.

  Applications include garbage collection for modern programming languages, network broadcasting packets,
  web crawling, and much more.

  Running time: Adjacency-list: O(V+E); Adjacency-matrix: O(V^2).
*/


#include <stdlib.h>
#include <stdio.h>

#include "../../Headers/graph.h"
#include "../../Headers/Graphs/hashTables.h"


/*                                  */
/*       Queue needed by BFS        */
/*                                  */

typedef struct queueNode {
  graph_vertex     *vertex;
  struct queueNode *next;
} queueNode;


typedef struct queue_t {
  queueNode *head;
  queueNode *tail;
} queue_t;


static
void queueEnqueue(queue_t *queue, graph_vertex *vertex)
{
  queueNode *new = (queueNode*)malloc(sizeof(*new));
  new->vertex = vertex;
  new->next   = NULL;

  if (!queue->tail) {
    queue->head = queue->tail = new;
    return;
  }
  queue->tail->next = new;
  queue->tail = new;
}


static
queueNode* queueDequeue(queue_t *queue)
{
  queueNode *front = queue->head;
  if ( queue->head) { queue->head = queue->head->next; }
  if (!queue->head) { queue->tail = NULL; }

  return front;
}


/*                                            */
/*       Breadth-First Search                 */
/*                                            */

// depth refers to the minimum required moves to reach the given vertex; root is 0. Strictly optional.
// predecessor is the int id of the vertex that preceded the vertex in the BFS path; root is -1.
hashTable* breadthFirstSearch(graph_t *graph, graph_vertex *vertex)
{
  // instantiate queue
  queue_t *queue = &(queue_t) { .head = NULL, .tail = NULL };
  queueEnqueue(queue, vertex);

  // mark vertex as been seen/added to queue
  hashTable *seen = hashTableBuild();
  int depth = 0, predecessor = -1;
  hashTableInsertNode(&seen, nodeHashTable_int(vertex->id, depth, predecessor));

  while (queue->head) {
    queueNode    *dequeued = queueDequeue(queue);
    graph_vertex *current  = dequeued->vertex;
    depth = hashTableSearchNode(seen, &current->id, keyConvertFromInt)->value + 1;
    predecessor = current->id;

    adjacencyListNode_t *edge = graph->list[current->id];
    while (edge) {
      if (!hashTableSearchNode(seen, &edge->vertex->id, keyConvertFromInt)) {
	queueEnqueue(queue, edge->vertex);
	hashTableInsertNode(&seen, nodeHashTable_int(edge->vertex->id, depth, predecessor));
      }
      edge = edge->next;
    }
    free(dequeued);
  }
  return seen;
}


bool vertexReachable(graph_t *graph, graph_vertex *vertex, graph_vertex *vertex_two)
{
  hashTable *table = breadthFirstSearch(graph, vertex);
  nodeHashTable *found = hashTableSearchNode(table, &vertex_two->id, keyConvertFromInt);
  if (found) { hashTableFree(table); return true; }
  hashTableFree(table); return false;
}


void breadthFirstApply(graph_t *graph, graph_vertex *source,
		       void (*apply)(graph_vertex *vertex, int depth, void *arg), void *arg)
{
  // instantiate queue
  queue_t *queue = &(queue_t) { .head = NULL, .tail = NULL };
  queueEnqueue(queue, source);

  // mark vertex as been seen/added to queue
  hashTable *seen = hashTableBuild();
  int depth = 0, predecessor = -1;
  hashTableInsertNode(&seen, nodeHashTable_int(source->id, depth, predecessor));

  while (queue->head) {
    queueNode    *dequeued = queueDequeue(queue);
    graph_vertex *current  = dequeued->vertex;
    depth = hashTableSearchNode(seen, &current->id, keyConvertFromInt)->value + 1;  // apply() may want to use it
    predecessor = current->id;
    apply(current, depth - 1, arg);

    adjacencyListNode_t *edge = graph->list[current->id];
    while (edge) {
      if (!hashTableSearchNode(seen, &edge->vertex->id, keyConvertFromInt)) {
	queueEnqueue(queue, edge->vertex);
	hashTableInsertNode(&seen, nodeHashTable_int(edge->vertex->id, depth, predecessor));
      }
      edge = edge->next;
    }
    free(dequeued);
  }
  hashTableFree(seen);
}


void printVertex(graph_vertex *vertex, int depth, void *NA)
{
  printf("Vertex: id:%d\tvalue: %d\tdepth:%d\n", vertex->id, vertex->value, depth);
}

// shows that we can have apply function discriminate on the vertex
static
void printOddOnly(graph_vertex *vertex, int depth, void* NA)
{
  if (vertex->id % 2 != 0) {
    printf("ODD VERTEX: %d\n", vertex->id);
  }
}

// here, we rely on a parameter from the caller to be our third parameter
void scaleVertexValue(graph_vertex *vertex, int depth, void *scaler)
{
  vertex->value *= *(int*)scaler;
}


void graphShortestPathEnum(hashTable *BFStree, int dest_id)
{
  if (!hashTableSearchNode(BFStree, &dest_id, keyConvertFromInt)) {
    printf("Vertex not reachable\n");
    return;
  }

  int predecessor = hashTableSearchNode(BFStree, &dest_id, keyConvertFromInt)->graph_predecessor;
  if (predecessor == -1) {
    printf("%d->", dest_id);
  } else {
    graphShortestPathEnum(BFStree, predecessor);
    printf("%d->", dest_id);
  }
}

