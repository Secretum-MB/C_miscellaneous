/*
  Depth-First Search (DFS)
  This graph traversal algorithm will explore the entire graph, even vertices
  that are disjoined from the rest. From its arbitrary starting point it will
  continue exploring along the edges, going deeper into the graph whenever it
  finds an unexplored vertex to visit. When no unexplored vertices remain on
  this path, it will backtrack until it again has a unexplored vertex to visit.
  When it has visited all connected vertices, it will pick the next vertex in
  the graph's set of verices and repeat above, until all of graph is explored.

  Returns a dictionary that contains one or more DFS-forests. A forest is a
  network of connected vertices - in the order in which they were visited.
  The dictionary's key is the vertix, the value: the parent's vertix id.
  (or -1 in case the key vertex was entry point into forest)
  Pairing this with further edge classification that DFS performs allows
  useful functionality - such as cycle detection and topological sort.

  DFS allows for edge classification:
  - Tree edge: edge connecting two trees in the forest. See DFS-forest dict.
  - Backward edge: connecting a vertex with another that is a predecessor in forest
     (by definition: loops (vertix with edge to itself) are backward edges)
  - Forward edge:  connecting a vertex with another that is a successor in forest
  - Cross edge: all other edges. May connect forests together or sibling vertices
     (vertices that are neither successors or predecessors of the other)
  Undirected graphs do not have forward or cross edges (of least importance anyway)
  - this follows quite plainly from logic; draw simple graphs to see why

  Performance: O(V + E)
  in directed graphs: edges visited once; in undirected graphs: twice.

  Useful applications of DFS: cycle detection and Topological sort

  Cycle Detection:
  graph has cycle if and only if DFS finds an backward edge.
  Also, cycle must exist if graph has a backward edge.
  There are multiple ways of implementing cycle detection. A common approach
  is to color the vertices as you visit them - up to three colors. This is not
  necessary, I use an array where each index corresponds to the vertix id of
  that number. The array stores a 0 by default and this indicates unvisited or
  that the vertix has been completely explored (the forest in which it is part
  has been completed by DFS). The other value stored is 1, indicating that this
  vertix is under consideration currently (its forest is being constructed).
  This structure is needed to identify backward edges. If an edge is found that
  leads to a vertex which has the status of 1, this edge then leads back to a
  predecessor in the forest, which is the definition of a backward edge.
  In order to identify the vertices in a cycle once you have found the backward
  edge, travel back along the tree edges in the DFS-forest until you reach the
  vertix identified as the predecessor vertix by the backward edge.

  Topological Sort:
  Topological sort is ran only on directed acyclic graphs (DAGs). A directed
  graph is a DAG if DFS states that is has no back edges. Topological sort
  produces a linear ordering of all vertices in the graph such that if edge (u,v)
  is in the graph then vertix u comes before vertix v in that ordering.
  There may be multiple valid sortings - the algorithm returns one such sorting.
  Summary: 1) run DFS; 2) output vertix in reverse of finishing times
  In my implementation a vertix is finished when its position in the above mentioned
  array is changed back to 0. Step two above is accomplished by placing the finished
  vertices at the head of a linked list as they are finished.
  Applications include job scheduling (determining ordering of tasks where edges
  represent constraints in regard to the task's precedence).
*/


#include <stdlib.h>
#include <stdio.h>

#include "../../Headers/graph.h"
#include "../../Headers/Graphs/hashTables.h"


/*                                            */
/*       Depth-First Search                   */
/*                                            */

static
void _depthFirstSearch_visit(graph_t *graph, int parent_id, hashTable **parents)
{
  adjacencyListNode_t *edge = graph->list[parent_id];
  while (edge) {
    if (!hashTableSearchNode( (*parents), &edge->vertex->id, keyConvertFromInt)) {
      hashTableInsertNode( &(*parents), nodeHashTable_int(edge->vertex->id, parent_id, parent_id));
      _depthFirstSearch_visit(graph, edge->vertex->id, parents);
    }
    edge = edge->next;
  }
}


hashTable* depthFirstSearch(graph_t *graph)
{
  hashTable *parent = hashTableBuild();

  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    if (!hashTableSearchNode(parent, &curr->id, keyConvertFromInt)) {
      hashTableInsertNode(&parent, nodeHashTable_int(curr->id, -1, -1));
      _depthFirstSearch_visit(graph, curr->id, &parent);
    }
    curr = curr->next;
  }
  return parent;
}


/*                      */
/*   Cycle Detection    */
/*                      */

static
void _DFS_visit_cycle_detect(graph_t *graph, int parent_id, hashTable **parents, int *progress_array, int *num_cycles)
{
  adjacencyListNode_t *edge = graph->list[parent_id];
  while (edge) {
    if (progress_array[edge->vertex->id] == 1 &&
	hashTableSearchNode( (*parents), &parent_id, keyConvertFromInt)->graph_predecessor != edge->vertex->id) { // for undirected graphs
      *num_cycles += 1;
    }
    if (!hashTableSearchNode( (*parents), &edge->vertex->id, keyConvertFromInt)) {
      progress_array[edge->vertex->id] = 1;
      hashTableInsertNode( &(*parents), nodeHashTable_int(edge->vertex->id, parent_id, parent_id));
      _DFS_visit_cycle_detect(graph, edge->vertex->id, parents, progress_array, num_cycles);
      progress_array[edge->vertex->id] = 0;
    }
    edge = edge->next;
  }
}


int graphExistsCycle(graph_t *graph)
{
  hashTable *parent = hashTableBuild();
  int num_cycles = 0;
  int in_progress[graph->listSize];
  for (size_t i = 0; i < graph->listSize; i++) { in_progress[i] = 0; }

  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    if (!hashTableSearchNode(parent, &curr->id, keyConvertFromInt)) {
      in_progress[curr->id] = 1;
      hashTableInsertNode(&parent, nodeHashTable_int(curr->id, -1, -1));
      _DFS_visit_cycle_detect(graph, curr->id, &parent, in_progress, &num_cycles);
      in_progress[curr->id] = 0;
    }
    curr = curr->next;
  }
  hashTableFree(parent);
  return num_cycles;
}


/*                      */
/*   Cycle Enumeration  */
/*                      */

static
void _print_cycle_path(hashTable *parents, int cycle_origin, int cycle_end)
{
  if (cycle_origin == cycle_end) { printf("%d->", cycle_end); }
  else {
    int parent = hashTableSearchNode(parents, &cycle_end, keyConvertFromInt)->graph_predecessor;
    _print_cycle_path(parents, cycle_origin, parent);
    printf("%d->", cycle_end);
  }
}


static
void _DFS_visit_cycle_enumerate(graph_t *graph, int parent_id, hashTable **parents, int *progress_array)
{
  adjacencyListNode_t *edge = graph->list[parent_id];
  while (edge) {
    if (progress_array[edge->vertex->id] == 1 &&
	hashTableSearchNode( (*parents), &parent_id, keyConvertFromInt)->graph_predecessor != edge->vertex->id) { // for undirected graphs
      printf("Cycle found: ");
      _print_cycle_path( (*parents), edge->vertex->id, parent_id);
      printf("\n");
    }

    if (!hashTableSearchNode( (*parents), &edge->vertex->id, keyConvertFromInt)) {
      progress_array[edge->vertex->id] = 1;
      hashTableInsertNode( &(*parents), nodeHashTable_int(edge->vertex->id, parent_id, parent_id));
      _DFS_visit_cycle_enumerate(graph, edge->vertex->id, parents, progress_array);
      progress_array[edge->vertex->id] = 0;
    }
    edge = edge->next;
  }
}


void graphCycleEnum(graph_t *graph)
{
  hashTable *parents = hashTableBuild();
  int in_progress[graph->listSize];
  for (size_t i = 0; i < graph->listSize; i++) { in_progress[i] = 0; }

  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    if (!hashTableSearchNode(parents, &curr->id, keyConvertFromInt)) {
      in_progress[curr->id] = 1;
      hashTableInsertNode(&parents, nodeHashTable_int(curr->id, -1, -1));
      _DFS_visit_cycle_enumerate(graph, curr->id, &parents, in_progress);
      in_progress[curr->id] = 0;
    }
    curr = curr->next;
  }
  hashTableFree(parents);
}


/*                      */
/*   Topological Sort   */
/*                      */

static
void _DFS_visit_topological_sort(graph_t *graph, int parent_id, hashTable **parents, int *progress_array, graph_vertex **sorted_head)
{
  adjacencyListNode_t *edge = graph->list[parent_id];
  while (edge) {
    if (!hashTableSearchNode( (*parents), &edge->vertex->id, keyConvertFromInt)) {
      progress_array[edge->vertex->id] = 1;
      hashTableInsertNode( &(*parents), nodeHashTable_int(edge->vertex->id, parent_id, parent_id));
      _DFS_visit_topological_sort(graph, edge->vertex->id, parents, progress_array, sorted_head);
      progress_array[edge->vertex->id] = 0;

      // vertex has finished being explored: add to head of list
      if ( (*sorted_head) == NULL) {
	*sorted_head = graphVertexNew(edge->vertex->id, edge->vertex->value);
	(*sorted_head)->next = NULL;
      } else {
	graph_vertex *new = graphVertexNew(edge->vertex->id, edge->vertex->value);
	new->next = *sorted_head;
	*sorted_head = new;
      }
    }
    edge = edge->next;
  }
}


graph_vertex* topologicalSort(graph_t *graph)
{
  hashTable *parent = hashTableBuild();
  int in_progress[graph->listSize];
  for (size_t i = 0; i < graph->listSize; i++) { in_progress[i] = 0; }
  graph_vertex *sorted_head = NULL;

  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    if (!hashTableSearchNode(parent, &curr->id, keyConvertFromInt)) {
      in_progress[curr->id] = 1;
      hashTableInsertNode(&parent, nodeHashTable_int(curr->id, -1, -1));
      _DFS_visit_topological_sort(graph, curr->id, &parent, in_progress, &sorted_head);
      in_progress[curr->id] = 0;

      // vertex has finished being explored: add to head of list
      if (sorted_head == NULL) {
	sorted_head = graphVertexNew(curr->id, curr->value);
	sorted_head->next = NULL;
      } else {
	graph_vertex *new = graphVertexNew(curr->id, curr->value);
	new->next = sorted_head;
	sorted_head = new;
      }
    }
    curr = curr->next;
  }
  hashTableFree(parent);
  return sorted_head;
}

