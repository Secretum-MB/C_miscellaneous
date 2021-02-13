/*  Breadth-First Search (BFS)

  Given an directed or undirected graph and a source vertex, it will "explore" every reachable
  vertex in the graph. It begins on the souce node, it then visits all of the source vertex's
  neighbors and takes note that they have been visited, then proceeds to visit all of their neighbors,
  and so on until no vertex has neighbors that have not been visited.

  The algorithm returns the means to build a breadth-first tree. Either by mutating the vertices'
  sattelite data or by returning a data structure that cointains the necessary information.
  This tree will be rooted at the souce vertex of the BFS function and contain all vertices that
  are reachable from it. The simple path in the tree from the root (source) to any other vertex
  is the shortest possible path in the graph between these two vertices! Careful, the path between
  any two vertices, where neither is the source, is not neccessarily the shortest path between them.

  Running time: Adjacency-list: O(V+E); Adjacency-matrix: O(V^2).



 */


#include <stdlib.h>
#include <stdio.h>

#include "../../Headers/graph.h"
#include "/home/mathias/Documents/Projects/C_Programming/Libraries/hashTables.h"


/*                                                */
/*       Queue needed by Breadth-First Search     */
/*                                                */

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

/*                                                */
/*       End of Queue                             */
/*                                                */

/* HELPER FUNCTION ONLY FOR TESTING   */
void printQueue(queue_t *queue)
{
  if (queue->head) {
    printf("head: %d\ttail: %d\n",
	   queue->head->vertex->id, queue->tail->vertex->id);
  } else { printf("Queue is Empty\n"); }
  queueNode *curr = queue->head;
  while (curr) {
    printf("[%d]->", curr->vertex->id);
    curr = curr->next;
  }
  printf("\n\n");
}


// if trying to access results of hashtable for a vertex that was not reachable
// via BFS input vertex, hashTableSearch will blow up as it returns NULL; wrap
// your query in code that checks that destination vertex is in hashtable.
hashTable* breadthFirstSearch(graph_t *graph, graph_vertex *vertex)
{
  // instantiate queue
  queue_t *queue = &(queue_t) { .head = NULL, .tail = NULL };
  queueEnqueue(queue, vertex);

  // mark vertex as been seen/added to queue
  hashTable *seen = hashTableBuild();
  hashTableInsertNode(&seen, nodeHashTable_int(vertex->id, 0));

  int depth;
  while (queue->head) {
    queueNode    *dequeued = queueDequeue(queue);
    graph_vertex *current  = dequeued->vertex;
    depth = hashTableSearchNode(seen, &current->id, keyConvertFromInt)->value + 1;
    //printf("CURRENT VERTEX: %d\n", current->id);

    adjacencyListNode_t *edge = graph->list[current->id];
    while (edge) {
      if (!hashTableSearchNode(seen, &edge->vertex->id, keyConvertFromInt)) {
	queueEnqueue(queue, edge->vertex);
	hashTableInsertNode(&seen, nodeHashTable_int(edge->vertex->id, depth));
      }
      edge = edge->next;
    }
    free(dequeued);
    //printQueue(queue);
  }
  return seen;
}




void test_one()
{
  graph_t *graph = graphBuild(false, false);
  graph_vertex *v1 = graphVertexNew(1, 42);  graph_vertex *v2 = graphVertexNew(2, 42);
  graph_vertex *v3 = graphVertexNew(3, 42);  graph_vertex *v4 = graphVertexNew(4, 42);
  graph_vertex *v5 = graphVertexNew(5, 42);  graph_vertex *v6 = graphVertexNew(6, 42);
  graph_vertex *v7 = graphVertexNew(7, 42);  graph_vertex *v8 = graphVertexNew(8, 42);

  graphAddVertex(&graph, v1);  graphAddVertex(&graph, v2);
  graphAddVertex(&graph, v3);  graphAddVertex(&graph, v4);
  graphAddVertex(&graph, v5);  graphAddVertex(&graph, v6);
  graphAddVertex(&graph, v7);  graphAddVertex(&graph, v8);

  graphAddEdgeU(graph, v1, v2);  graphAddEdgeU(graph, v2, v3);  graphAddEdgeU(graph, v3, v4);
  graphAddEdgeU(graph, v4, v5);  graphAddEdgeU(graph, v5, v6);  graphAddEdgeU(graph, v5, v7);
  graphAddEdgeU(graph, v6, v8);  graphAddEdgeU(graph, v7, v8);  graphAddEdgeU(graph, v4, v6);

  hashTable *BFStree = breadthFirstSearch(graph, v6);

  printf("dist to v7: %d\n", hashTableSearchNode(BFStree, &v7->id, keyConvertFromInt)->value);

  hashTableFree(BFStree);
  graphFree(graph);
}




int main(void)
{
  //test_one();



  return EXIT_SUCCESS;
}

